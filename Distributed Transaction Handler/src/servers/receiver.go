package main

import (
	// "fmt"
	"sync"
)

func (host *Server) resp_good(req Message, bal int) {
	respOut := Message{
		Msg_type: "RESPONSE",
		Server: host.Id,
		Acc: req.Acc,
		Status: true,
		Balance: bal,
		Acc_Exists: true}
	host.server_write(req.Server, respOut)
}

func (host* Server) handle_deposit(message Message) {
	// Load account
	cast, ok := host.AccountData.Load(message.Acc)

	if ok {
		acc := cast.(*Account)	
		// Account Committed
		if acc.Committed {
			locked := host.LockAccount(message.Acc, "W")
			if !locked {
				respOut := Message{
					Msg_type: "RESPONSE",
					Server: host.Id,
					Acc: message.Acc,
					Status: false,
					Acc_Exists: true}
				host.server_write(message.Server, respOut)
				return
			}

			host.resp_good(message, acc.Balance)
		
		// Uncommitted Account
		} else {
			// Add to uncommited queue
			host.Mu_Uncommitted.Lock()
			if q, ok := host.Uncommitted[message.Acc]; ok {
				inserted := append(q, message.Transaction)
				host.Uncommitted[message.Acc] = inserted
			} else {
				new_queue := []string{message.Transaction}
				host.Uncommitted[message.Acc] = new_queue
			}
			host.Mu_Uncommitted.Unlock()
			
			// Acquire write lock
			locked := host.LockAccount(message.Acc, "W")
			if !locked {
				respOut := Message{
					Msg_type: "RESPONSE",
					Server: host.Id,
					Acc: message.Acc,
					Status: false,
					Acc_Exists: true}
				host.server_write(message.Server, respOut)
				return
			}

			host.Mu_Uncommitted.Lock()
			if acc.Committed {
				delete(host.Uncommitted, message.Acc)
				host.Mu_Uncommitted.Unlock()
				// fmt.Println(message)
				host.resp_good(message, acc.Balance)
			} else {
				// Otherwise erase yourself from list of queued transactions, add val locally as creator
				for i, transaction := range host.Uncommitted[message.Acc] {
					if transaction == message.Transaction {
						arr := host.Uncommitted[message.Acc]
						host.Uncommitted[message.Acc] = append(arr[:i], arr[i+1:]...)
					}
				}
				host.Mu_Uncommitted.Unlock()
				host.resp_good(message, 0)
			}
		}

	// New account, create uncommitted
	} else {

		new_acc := new(Account)
		rw_ptr := new(sync.RWMutex)
		upgrade_ptr := new(sync.Mutex)
		read_lock := new(sync.Mutex)
		*new_acc = Account{RW: rw_ptr, Upgrade: upgrade_ptr, ReaderLock: read_lock, Branch: host.Id, Balance: 0, LockType: "W", Committed: false}

		new_acc.RW.Lock()
		new_cast, was_loaded := host.AccountData.LoadOrStore(message.Acc, new_acc)
		stored_acc := new_cast.(*Account)

		// Someone else already stored
		if was_loaded {

			// Wait on lock, once obtained see if account was committed
			host.LockAccount(message.Acc, "W")

			host.Mu_Uncommitted.Lock()
			if stored_acc.Committed {
				delete(host.Uncommitted, message.Acc)
				host.Mu_Uncommitted.Unlock()

				host.resp_good(message, stored_acc.Balance)
			} else {
				// Otherwise erase yourself from list of queued transactions, add val locally as creator
				for i, transaction := range host.Uncommitted[message.Acc] {
					if transaction == message.Transaction {
						arr := host.Uncommitted[message.Acc]
						host.Uncommitted[message.Acc] = append(arr[:i], arr[i+1:]...)
					}
				}
				host.Mu_Uncommitted.Unlock()

				host.resp_good(message, 0)
			}

		// First to load
		} else {
			host.resp_good(message, stored_acc.Balance)
		}
	}
}

func (host* Server) handle_withdraw_balance(message Message) {
	// Load account
	cast, ok := host.AccountData.Load(message.Acc)
	// If account present acquire lock
	if ok {
		acc := cast.(*Account)
		if message.Request == "WITHDRAW"{
			host.LockAccount(message.Acc, "W")
		} else {
			host.LockAccount(message.Acc, "R")
		}
		// fmt.Println("COMMITED BALANCE: ", acc.Balance)
		// If account uncommitted, unlock and abort
		if !acc.Committed {
			respOut := Message{
				Msg_type: "RESPONSE",
				Server: host.Id,
				Acc: message.Acc,
				Status: false,
				Acc_Exists: false}
			host.server_write(message.Server, respOut)
		} else {
			host.resp_good(message, acc.Balance)
		}
	} else {
		respOut := Message{
			Msg_type: "RESPONSE",
			Server: host.Id,
			Acc: message.Acc,
			Status: false,
			Acc_Exists: false}
		host.server_write(message.Server, respOut)
	}
}

func (host* Server) handle_release(message Message) {
	// Load account
	cast, ok := host.AccountData.Load(message.Acc)
	acc := cast.(*Account)

	if ok {
		correct_bal := acc.Balance
		released := host.Release(message.Acc, false)
		if released {
			host.resp_good(message, correct_bal)
		} else {
		respOut := Message{
			Msg_type: "RESPONSE",
			Server: host.Id,
			Acc: message.Acc,
			Status: false,
			Acc_Exists: true}
		host.server_write(message.Server, respOut)
		}
	} else {
		respOut := Message{
			Msg_type: "RESPONSE",
			Server: host.Id,
			Acc: message.Acc,
			Status: false,
			Acc_Exists: false}
		host.server_write(message.Server, respOut)
	}
}

func (host* Server) handle_commit(message Message) {
	// If in map, should have lock
	// Update new balance and release lock
	// host.AccountData.Store(message.Acc, acc)
	for _, next := range message.CommitVals{
		cast, ok := host.AccountData.Load(next.Name)
		if ok {
			acc := cast.(*Account)

			acc.Balance = next.Val

			host.Release(next.Name, true)
			// new_cast, _ := host.AccountData.Load(message.Acc)
			// updated := new_cast.(*Account)
			// Error check, should always work
		} else {
			respOut := Message{
				Msg_type: "RESPONSE",
				Server: host.Id,
				Acc: message.Acc,
				Status: false,
				Acc_Exists: true}
			host.server_write(message.Server, respOut)
			return
		}
	}
	host.Print_Accs()
	host.resp_good(message, message.Balance)
}

func (host* Server) handle_upgrade(message Message) {
	respOut := Message{
		Msg_type: "RESPONSE",
		Server: host.Id,
		Acc: message.Acc,
	}

	// Load account
	cast, ok := host.AccountData.Load(message.Acc)
	acc := cast.(*Account)

	if ok {
		// If exists and is read lock, upgrade
		if acc.LockType == "R" {
			upgraded := host.Upgrade(message.Acc)
			if upgraded {
				respOut.Status = true
				respOut.Balance = acc.Balance
				respOut.Acc_Exists = true
				host.server_write(message.Server, respOut)
			}
		} else {
			respOut.Acc_Exists = true
			respOut.Status = false
			host.server_write(message.Server, respOut)
		}
	} else {
		respOut.Acc_Exists = false
		respOut.Status = false
		host.server_write(message.Server, respOut)
	}

}

func (host *Server) handle_probe(message Message) {

}


func (host *Server) handle_request(message Message) {
	// Upgrade Lock
	if message.Request == "UPGRADE" {
		host.handle_upgrade(message)
	// Deposit - acquire write lock and return data, creat uncommitted account if non existent
	} else if message.Request == "DEPOSIT" {
		host.handle_deposit(message)
	// Withdraw/Balanve - acquire lock and return balance, abort if non existent
	} else if message.Request == "WITHDRAW" || message.Request == "BALANCE" {
		host.handle_withdraw_balance(message)
	// Abort - Release Locks
	} else if message.Request == "ABORT" {
		host.handle_release(message)
	// Commit - Update values then release locks
	} else if message.Request == "COMMIT" {
		host.CommitLock.Lock()
		host.handle_commit(message)
		host.CommitLock.Unlock()
	}
}
