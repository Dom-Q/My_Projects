package main

import (
	"encoding/gob"
	"fmt"
	"io"
	"net"
	"strconv"
	"strings"
	"sync"
	// "testing/quick"
	// "time"
	// "sync"
)

// CHALENGES
//	- Lock upgrading (2 lock) - will need to acquire Both for writes AND reads for atomicity
//  - Creating new account (committed var)
//  - How to grab locks - need to figure out order and stuff, left "get lock" comments
//  - ACCOUNTS NEED TO BE STORED AS POINTERS IN GLOBAL MAP

// Pass server id
// Return true if successful
func (host *Server) server_write(id string, message Message) bool {
	// fmt.Print("Seding request to server " + id + ": ")
	// fmt.Println(msg_out)
	// fmt.Print("Message address = ")
	// fmt.Println(&msg_out)

	// fmt.Print("Server ptr = ")
	// fmt.Println(host.Servers[host.Addresses[id]])

	encoder := gob.NewEncoder(*(host.Servers[host.Addresses[id]]))
	err := encoder.Encode(&message)

	if err != nil {
		fmt.Println(err)
	}

	// fmt.Print("Sent request to server " + id + ": ")
	// fmt.Println(msg_out)

	return err == nil
}

// Pass message to send to client
// Return true if sucessfully read
func (host *Server) client_write(message Client_resp) bool {
	encoder := gob.NewEncoder(*host.Client)
	err := encoder.Encode(&message)

	if err != nil {
		fmt.Println(err)
	}

	// fmt.Println("Sent message '" + message.Msg + "' to client.")

	return err == nil
}

// Acquire and modify lock in global map
// Args:
//  - key == account name
//  - ltype == "R" for read "W" for write
func (host *Server) LockAccount(key string, ltype string) bool {
	obj, ok := host.AccountData.Load(key)
	if !ok {
		// fmt.Println("Attempt to lock non-existent object.")
		return false
	}

	acc := obj.(*Account)
	if ltype == "R" {
		// Make infinite, set limit for testing

		// tries := 0
		// for tries < 50 {
		// 	acc.RW.RLock()
		// 	is_free := acc.Upgrade.TryLock()

		// 	// If RW lock acquired, see if someone else is upgrading. Abort if so
		// 	if is_free {
		// 		acc.LockType = "R"
		// 		acc.Upgrade.Unlock()
		// 		return true
		// 	} else {
		// 		acc.RW.RUnlock()

		// 		// Add small delay before trying to reacquire to allow upgrader to finish?
		// 		time.Sleep(time.Millisecond)
		// 	}
		// 	tries++
		// }

		acc.Upgrade.Lock()
		acc.RW.RLock()
		acc.Upgrade.Unlock()
		
		acc.ReaderLock.Lock()
		acc.Readers++
		acc.LockType = "R"
		acc.ReaderLock.Unlock()

		// fmt.Println("Couldnt get lock after 50 attempts.")
	} else {
		// tries := 0
		// for tries < 0 {
		// 	acc.RW.Lock()
		// 	is_free := acc.Upgrade.TryLock()

		// 	// If RW lock acquired, see if someone else is upgrading. Abort if so
		// 	if is_free {
		// 		acc.LockType = "W"
		// 		acc.Upgrade.Unlock()
		// 		return true
		// 	} else {
		// 		acc.RW.Unlock()

		// 		// Add small delay before trying to reacquire to allow upgrader to finish?
		// 		time.Sleep(time.Millisecond)
		// 	}
		// 	tries++
		// }

		acc.Upgrade.Lock()
		acc.RW.Lock()
		acc.Upgrade.Unlock()

		acc.LockType = "W"

		// fmt.Println("Couldnt get lock after 50 attempts.")
	}
	return true
}

// Call only if read lock is already held
func (host *Server) Upgrade(account string) bool {
	val, ok := host.AccountData.Load(account)
	acc := val.(*Account)
	if ok {
		// Try lock just for error checking
		// "Upgrade" lock should always be available when upgrading

		// attempt := acc.Upgrade.TryLock()
		// if attempt {
		// 	acc.RW.RUnlock()
		// 	acc.RW.Lock()
		// 	acc.LockType = "W"
		// 	acc.Upgrade.Unlock()
		// 	return true
		// } else {
		// 	fmt.Println("Failed to upgrade lock")
		// }

		acc.Upgrade.Lock()
		acc.RW.RUnlock()
		acc.RW.Lock()
		acc.LockType = "W"
		acc.Upgrade.Unlock()
		return true

	} else {
		// fmt.Println("Tried to upgrade nonexistent account.")
	}
	return false
}


func (host *Server) Print_Accs() {
	host.AccountData.Range(func(key , value interface{}) bool{
		data := value.(*Account)
		if data.Balance > 0 && data.Committed {
			fmt.Print(data.Branch + ".")
			fmt.Print(key)
			fmt.Print(" = ")
			fmt.Println(data.Balance)
		}
		return true
	})
}

// Release Locks
// Args:
// 1.) account name
// 2.) locktype ("R" or "W")
// 3.) commit (true if committed, false if aborted)
func (host *Server) Release(account string, commit bool) bool {
	val, ok := host.AccountData.Load(account)
	acc := val.(*Account)
	// fmt.Println(account, " = ", acc.Balance)
	if ok {
		// Change commit status if necessary
		if commit {
			acc.Committed = true
		}

		// Read Lock
		if acc.LockType == "R" {
			acc.ReaderLock.Lock()
			acc.Readers--
			if acc.Readers == 0 {
				acc.LockType = ""
			}
			acc.ReaderLock.Unlock()
			acc.RW.RUnlock()

			// Write Lock
		} else {
			acc.ReaderLock.Lock()
			if acc.Readers == 0 {
				acc.LockType = ""
			} else {
				acc.LockType = "R"
			}

			acc.ReaderLock.Unlock()
			acc.RW.Unlock()
		}

		// fmt.Println("Released lock " + acc.LockType + " on " + account)

		return true
	}
	return false
}

// Thread for handling client
func (host *Server) client_listen() {
	for {

		// fmt.Println("Awaiting Client Command.")

		var data_in Client_msg
		decoder := gob.NewDecoder(*host.Client)
		err := decoder.Decode(&data_in)
		// fmt.Println(err)
		if err != nil {
			// fmt.Print("Failed read from client: ")
			if err == io.EOF {
				data_in.Command = "DISCONNECT"
				host.Cli_chan <- data_in
			}
			return
		}

		// fmt.Println("New Command from Client: " + data_in.Command)
		if data_in.Command == "COMMIT" || data_in.Command == "ABORT" {
			// fmt.Println("End client listen.")
			host.Cli_chan <- data_in
			return
		}
		// Parse message send thread
		host.Cli_chan <- data_in
		if data_in.Command == "ABORT" || data_in.Command == "COMMIT" {
			return
		}
	}
}

// Thread for listening to servers
func (host *Server) server_comm(target_server *net.Conn) {
	for {
		// Wait to receive data
		var latest_message Message
		decoder := gob.NewDecoder(*target_server)
		err := decoder.Decode(&latest_message)
		if err != nil {
			// fmt.Print("Failed read from server: ")
			// fmt.Println(err)
			// if err == io.EOF {
			// 	fmt.Println("Disconnected from server " + (*target_server).RemoteAddr().String())
			// }
			return
		}

		// fmt.Print("Received message from server: ")
		// fmt.Println(latest_message)


		// Parse message send thread
		if latest_message.Msg_type == "RESPONSE" {
			host.Trans_chan <- latest_message
		} else if latest_message.Msg_type == "REQUEST" {
			go host.handle_request(latest_message)
		} else if latest_message.Msg_type == "PROBE" {
			go host.handle_probe(latest_message)
		}
	}
}

func (host *Server) Coordinator(name string) {
	// Store Transaction
	host.Tr = Transaction{Name: name, Accounts: make(map[string]Account), WaitFor: ""}
	// host.Trans_chan = make(chan Message)
	go host.client_listen()

	for {
		// Wait for client messages
		msg := <-host.Cli_chan

		// Handle Deposit
		if msg.Command == "DISCONNECT" {
			host.abort()
			// fmt.Println("Client Disconnected.")
			return
		}
		if msg.Command == "DEPOSIT" {
			host.deposit(msg)
			continue
		}
		if msg.Command == "WITHDRAW" {
			host.withdraw(msg)
			continue
		}
		if msg.Command == "BALANCE" {
			host.balance(msg)
			continue
		}
		if msg.Command == "ABORT" {
			host.abort()
			client_out := Client_resp{Success: true, Msg: "ABORTED"}
			host.client_write(client_out)
			return
		}
		if msg.Command == "COMMIT" {
			host.commit()
			return
		}
	}
}

// Hierarchy:
// If Account in host.Tr.Accounts
// 	 -> Make local change
// Else If operation.Branch == this branch
// 	 -> Acquire lock from host global map
// 	 -> Add to local map
//   -> Execute local transaction
// Else
// 	 -> Acquire lock/account data from remote branch
//   -> Add to local map
//   -> Execute local transaction
func (host *Server) deposit(operation Client_msg) {
	var response Message
	var req_out Message
	req_out.Msg_type = "REQUEST"
	req_out.Transaction = host.Tr.Name
	req_out.Server = host.Id
	req_out.Acc = operation.Account

	// fmt.Print("Message: ")
	// fmt.Println(req_out)

	// Case 1: Already in map
	local_key := operation.Branch + "." + operation.Account
	if val, ok := host.Tr.Accounts[local_key]; ok {

		// Have Write Lock already, update locally
		if val.LockType == "W" {
			val.Balance += operation.Amount
			host.Tr.Accounts[local_key] = val

			// Lock not "W", need upgrade
		} else {
			// Local upgrade
			if operation.Branch == host.Id {
				host.Upgrade(operation.Account)
				val.Balance += operation.Amount
				val.LockType = "W"
				host.Tr.Accounts[local_key] = val

				// Otherwise need remote to upgrade
			} else {
				req_out.Request = "UPGRADE"
				go host.server_write(operation.Branch, req_out)

				resp_in := <-host.Trans_chan
				if resp_in.Status {
					val.Balance += operation.Amount
					val.LockType = "W"
					host.Tr.Accounts[local_key] = val
				}
			}
		}

		// Case 2: Account not in local copy but at local branch
	} else if host.Id == operation.Branch {
		cast, ok := host.AccountData.Load(operation.Account)

		// If account in global map
		if ok {
			acc := cast.(*Account)

			// If Present but uncommitted
			if !acc.Committed {
				// Add to uncommited queue
				host.Mu_Uncommitted.Lock()
				if q, qok := host.Uncommitted[operation.Account]; qok {
					inserted := append(q, host.Tr.Name)
					host.Uncommitted[operation.Account] = inserted
				} else {
					new_queue := []string{host.Tr.Name}
					host.Uncommitted[operation.Account] = new_queue
				}
				host.Mu_Uncommitted.Unlock()

				// Acquire write lock
				host.LockAccount(operation.Account, "W")

				// If account was committed while waiting:
				// - delete form uncommitted
				// - Add copy to local map
				// - Update val locally
				host.Mu_Uncommitted.Lock()
				if acc.Committed {
					delete(host.Uncommitted, operation.Account)
					host.Mu_Uncommitted.Unlock()
					new_acc := *acc
					new_acc.Balance += operation.Amount
					host.Tr.Accounts[local_key] = new_acc
				} else {
					// Otherwise erase yourself from list of queued transactions, add val locally as creator
					for i, transaction := range host.Uncommitted[operation.Account] {
						if transaction == host.Tr.Name {
							arr := host.Uncommitted[operation.Account]
							host.Uncommitted[operation.Account] = append(arr[:i], arr[i+1:]...)
						}
					}
					host.Mu_Uncommitted.Unlock()
					new_acc := *acc
					new_acc.Balance = operation.Amount
					host.Tr.Accounts[local_key] = new_acc
				}

				// If Present and committed
			} else {
				// Get lock and add locally
				host.LockAccount(operation.Account, "W")
				new_acc := *acc
				new_acc.Balance += operation.Amount
				host.Tr.Accounts[local_key] = new_acc
			}

			// If not in map, create uncommitted account
		} else {

			// fmt.Println("Add to global map.")

			new_acc := new(Account)
			rw_ptr := new(sync.RWMutex)
			upgrade_ptr := new(sync.Mutex)
			read_lock := new(sync.Mutex)
			*new_acc = Account{RW: rw_ptr, Upgrade: upgrade_ptr, ReaderLock: read_lock,Branch: host.Id, Balance: 0, LockType: "W", Committed: false}

			new_acc.RW.Lock()

			new_cast, was_loaded := host.AccountData.LoadOrStore(operation.Account, new_acc)
			stored_acc := new_cast.(*Account)

			// Someone else already committed
			if was_loaded {

				// Wait on lock, once obtained see if account was committed
				host.LockAccount(operation.Account, "W")

				host.Mu_Uncommitted.Lock()
				if stored_acc.Committed {
					delete(host.Uncommitted, operation.Account)
					host.Mu_Uncommitted.Unlock()
					acc_copy := *stored_acc
					acc_copy.Balance += operation.Amount
					host.Tr.Accounts[local_key] = acc_copy
				} else {
					// Otherwise erase yourself from list of queued transactions, add val locally as creator
					for i, transaction := range host.Uncommitted[operation.Account] {
						if transaction == host.Tr.Name {
							arr := host.Uncommitted[operation.Account]
							host.Uncommitted[operation.Account] = append(arr[:i], arr[i+1:]...)
						}
					}
					host.Mu_Uncommitted.Unlock()
					acc_copy := *stored_acc
					acc_copy.Balance = operation.Amount
					host.Tr.Accounts[local_key] = acc_copy
				}

				// First to load
			} else {
				// ALready have lock, add our copy locally
				acc_copy := *stored_acc
				acc_copy.Balance = operation.Amount
				host.Tr.Accounts[local_key] = acc_copy

				// fmt.Print("Created account " + operation.Account)
				// fmt.Println(" with balance " + strconv.Itoa(host.Tr.Accounts[local_key].Balance))
			}

		}

	// Case 3: Acquire Lock and fetch from remote server
	} else {
		req_out.Request = "DEPOSIT"

		// fmt.Print("Message to server: ")
		// fmt.Println(req_out)

		go host.server_write(operation.Branch, req_out)

		// Receive response and upgrade local
		response = <-host.Trans_chan
		new_local := Account{
			Branch:   operation.Branch,
			Balance:  response.Balance,
			LockType: "W",
			// Committed?
		}
		new_local.Balance += operation.Amount
		host.Tr.Accounts[local_key] = new_local
	}

	// Notify Client?
	cli_resp := Client_resp{Success: true, Msg: "OK"}
	go host.client_write(cli_resp)
	return
}

func (host *Server) withdraw(operation Client_msg) {
	var response Message

	var req_out Message
	req_out.Msg_type = "REQUEST"
	req_out.Transaction = host.Tr.Name
	req_out.Server = host.Id
	req_out.Acc = operation.Account

	// Case 1: Already in map
	local_key := operation.Branch + "." + operation.Account
	if val, ok := host.Tr.Accounts[local_key]; ok {
		// Have Write Lock already, update locally
		if val.LockType == "W" {
			val.Balance -= operation.Amount
			host.Tr.Accounts[local_key] = val

		// Lock not "W", need upgrade
		} else {
			// Local upgrade
			if operation.Branch == host.Id {
				host.Upgrade(operation.Account)
				val.Balance -= operation.Amount
				val.LockType = "W"
				host.Tr.Accounts[local_key] = val

			// Remote upgrade
			} else {
				req_out.Request = "UPGRADE"
				go host.server_write(operation.Branch, req_out)

				resp_in := <-host.Trans_chan
				if resp_in.Status {
					val.Balance -= operation.Amount
					val.LockType = "W"
					host.Tr.Accounts[local_key] = val
				}
			}
		}

		// Case 2: Account not in local copy but at local branch
	} else if host.Id == operation.Branch {
		// Check if exists in global map
		cast, ok := host.AccountData.Load(operation.Account)

		// If account non-existent, abort
		if !ok {
			// Cannot withdraw from non existant account
			cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
			host.client_write(cli_resp)
			host.abort()
			return

			// Else grab lock, update and add locally
		} else {
			realAcc := cast.(*Account)

			host.LockAccount(operation.Account, "W")

			// If account uncommitted, unlock and abort
			if !realAcc.Committed {
				host.Release(operation.Account, false)
				cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
				host.client_write(cli_resp)
				host.abort()
				return
			}

			acc_copy := *realAcc
			acc_copy.Balance -= operation.Amount
			host.Tr.Accounts[local_key] = acc_copy
		}

		// Case 3: Acquire Lock and fetch from remote server
	} else {
		req_out.Request = "WITHDRAW"
		go host.server_write(operation.Branch, req_out)

		// Receive response and upgrade local
		response = <-host.Trans_chan

		// Abort nonexistent
		if !response.Acc_Exists {
			cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
			host.client_write(cli_resp)
			host.abort()
			return
		}

		new_local := Account{
			Branch:   operation.Branch,
			Balance:  response.Balance,
			LockType: "W",
			// Committed?
		}

		new_local.Balance -= operation.Amount
		host.Tr.Accounts[local_key] = new_local
		// fmt.Println("LOCAL BALANCE: ", host.Tr.Accounts[local_key].Balance)
	}
	// Notify Client?
	cli_resp := Client_resp{Success: true, Msg: "OK"}
	host.client_write(cli_resp)
}

func (host *Server) balance(operation Client_msg) {
	var response Message
	var bal int
	var req_out Message
	req_out.Msg_type = "REQUEST"
	req_out.Transaction = host.Tr.Name
	req_out.Server = host.Id
	req_out.Request = "BALANCE"
	req_out.Acc = operation.Account

	// Case 1: Already in map
	local_key := operation.Branch + "." + operation.Account
	if val, ok := host.Tr.Accounts[local_key]; ok {
		// Just return balance
		bal = val.Balance

		// Case 2: Account not in local copy but at local branch
	} else if host.Id == operation.Branch {
		// Check if exists in global map
		cast, ok := host.AccountData.Load(operation.Account)

		// If account non-existent, abort
		if !ok {
			//cannot withdraw from non existant account
			cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
			host.client_write(cli_resp)
			host.abort()
			return

		// Else grab read lock
		} else {
			realAcc := cast.(*Account)

			host.LockAccount(operation.Account, "R")

			// If account uncommitted, unlock and abort
			if !realAcc.Committed {
				host.Release(operation.Account, false)
				cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
				host.client_write(cli_resp)
				host.abort()
				return
			} else {
				acc_copy := *realAcc
				host.Tr.Accounts[local_key] = acc_copy
				bal = acc_copy.Balance
			}
		}

		// Case 3: Acquire Lock and fetch from remote server
	} else {
		go host.server_write(operation.Branch, req_out)

		// Receive response and upgrade local
		response = <-host.Trans_chan

		// Abort nonexistent
		if !response.Acc_Exists {
			cli_resp := Client_resp{Success: false, Msg: "NOT FOUND, ABORTED"}
			host.client_write(cli_resp)
			host.abort()
			return
		}

		new_local := Account{
			Branch:   operation.Branch,
			Balance:  response.Balance,
			LockType: "R",
			// Committed?
		}
		host.Tr.Accounts[local_key] = new_local
		bal = new_local.Balance
	}
	report := local_key + " = " + strconv.Itoa(bal)
	cli_resp := Client_resp{Success: true, Msg: report}
	host.client_write(cli_resp)
}

// Abort Process:
// 1.) Parse Local Accounts and release all remote and local locks
// 2.) Clear Host Struct to end transaction
func (host *Server) abort() {
	// Iterate Local Transaction
	for acc, data := range host.Tr.Accounts {
		// Local Account
		div := strings.Split(acc, ".")
		acc_name := div[1]
		if data.Branch == host.Id {
			host.Release(acc_name, false)
			// Remote Account
		} else {
			msg_out := Message{
				Msg_type:    "REQUEST",
				Transaction: host.Tr.Name,
				Server:      host.Id,
				Request:     "ABORT",
				Acc:         acc_name}
			go host.server_write(data.Branch, msg_out)
			<-host.Trans_chan
		}
	}

	// End Transaction
	host.Tr = Transaction{}
	host.InTransaction = false

	// client_out := Client_resp{Success: true, Msg: "ABORTED"}
	// host.client_write(client_out)
}



// Commit Process:
// 1.) Commit all local and remote values to global map
// 2.) Release all locks
// 3.) Clear Server struct to end transaction
// - Careful about timing with threads, make sure all updates are done
func (host *Server) commit() {
	// Iterate Local Transaction

	for _, data := range host.Tr.Accounts {
		if data.Balance < 0 {
			host.abort()
			client_out := Client_resp{Success: false, Msg: "ABORTED"}
			host.client_write(client_out)
			return
		}
	}
	
	sendVals := make(map[string][]Commits)
	for acc, data := range host.Tr.Accounts {
		var toCommit Commits
		div := strings.Split(acc, ".")
		acc_name := div[1]
		
		toCommit.Name = acc_name
		toCommit.Val = data.Balance
		
		if _, ok := sendVals[data.Branch]; ok {
			new_list := sendVals[data.Branch]
			new_list = append(new_list, toCommit)
			sendVals[data.Branch] = new_list
		} else{
			new_list := []Commits{toCommit}
			sendVals[data.Branch] = new_list
		}
	}
	for k, v := range sendVals {
		// Local Commit
		if k == host.Id {
			host.CommitLock.Lock()
			for _, next := range v{
				cast, ok := host.AccountData.Load(next.Name)
				if ok {
					acc := cast.(*Account)
		
					acc.Balance = next.Val
		
					host.Release(next.Name, true)
					// new_cast, _ := host.AccountData.Load(message.Acc)
					// updated := new_cast.(*Account)
					// Error check, should always work
				} else {
					host.abort()
					return
				}
			}
			host.Print_Accs()
			host.CommitLock.Unlock()
		// Remote Commit
		} else {
			msg_out := Message{
				Msg_type:    "REQUEST",
				Transaction: host.Tr.Name,
				Server:      host.Id,
				Request:     "COMMIT",
				CommitVals:  v}
			go host.server_write(k, msg_out)
			<-host.Trans_chan
		}
	}


	// End Transaction
	host.Tr = Transaction{}
	host.InTransaction = false

	client_out := Client_resp{Success: true, Msg: "COMMIT OK"}
	host.client_write(client_out)
}
