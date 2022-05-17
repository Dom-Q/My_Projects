package main

import (
	"bufio"
	"encoding/gob"
	"fmt"
	"net"
	"os"
	"strings"
	"sync"
	// "time"
)

type Server struct {
	Servers        map[string]*net.Conn // remote address -> remote file descriptor
	Addresses      map[string]string   // identifier -> remote address
	Hostname       string              // this server hostname
	Port           string              // this server port
	Name_with_port string
	Id             string			   // Branch Name

	Client *net.Conn					   // FD of client being handled

	Uncommitted	   map[string][]string // Map accounts -> List of waiting transactions
									   // Avoid Buildup of dead accounts 
	Mu_Uncommitted sync.Mutex		   // Just use reg map and lock, minimal concurrency with this map

	//channels
	Cli_chan   chan Client_msg //channel for Client messages
	Trans_chan chan Message    //channel for server messages

	Tr Transaction

	AccountData sync.Map // Account data stored at this server (Account Name->Account)
	InTransaction bool
	CommitLock  sync.Mutex
}
type Client_msg struct {
	Command string	// Deposit, Withdraw, Balanc, Commit
	Branch string   // Target branch
	Account string	// Account name
	Amount  int		// Amount to dep/withdraw
}

type Client_resp struct {
	Success bool
	Msg     string
}

type Message struct{
	Msg_type 	string // Request, Response, Probe
	Transaction string // Client Transaction ID
	Server string 	   // Sending server branch name/ID
	Request string 	   // Balance, Withdraw, deposit, upgrade, commit, abort
	Acc string 		   // Account name (without branch)
	// New_Balance int	   // Value to update to for commits

	// Response 
	Balance int		   // Balance of Acc
	Acc_Exists bool    // True if Acc exists
	Status bool		   // Successfully executed request, debugging

	CommitVals []Commits
}

type Commits struct{
	Name string
	Val  int
}

type Transaction struct{
	Name 	 string
	Accounts map[string]Account // Branch.Name -> Account Obj
	WaitFor string
}

type Account struct{
	RW		 *sync.RWMutex // Read Write Lock for the Account
	Upgrade  *sync.Mutex   // Grab for upgrade
	Branch   string        // Associated branch
	Balance  int			 // Balance of Account (name will be key of map)
	LockType string 	  // "R" or "W"
	Readers  int
	Committed bool		  // True if account has new, but uncommitted - May need to add conditional wait 
	ReaderLock *sync.Mutex
}

// type Msg struct{ // For testing
// 	Name string
// 	Count int
// }

func main() {
	// Get Args (assigned branch, config file)
	cmd_args := os.Args
	if len(cmd_args) != 3 {
		fmt.Println("Incorrect Args.")
		return
	}
	// Get assigned branch
	branch := cmd_args[1]
	config_path := cmd_args[2]

	// Server Vars
	const num_servers int = 5 // Change for testing, number of server in file
	Branch := Server{
		Addresses: make(map[string]string),
		Servers: make(map[string]*net.Conn),
		Uncommitted: make(map[string][]string),
		Id: branch,
		Cli_chan: make(chan Client_msg),
		Trans_chan: make(chan Message),
		InTransaction: false}

	// Open file
	config_file, ferr := os.Open(config_path)
	if ferr != nil {
		fmt.Println("Couldnt open configuration file.")
		return
	}

	// Parse config file, add address to map
	config_reader := bufio.NewReader(config_file)
	parsed := 0
	for parsed < num_servers {
		curr_line, rerr := config_reader.ReadString('\n')
		parsed++
		if rerr != nil && parsed != num_servers {
			fmt.Println("Failed to properly parse file. Ensure configuration is proper.")
			return
		}

		div := strings.Split(curr_line, " ")
		curr_host := div[1]
		curr_port := div[2]

		if strings.HasSuffix(curr_port, "\n") {
			curr_port = strings.ReplaceAll(curr_port, "\n", "")
		}
		// Check self
		if div[0] == branch {
			Branch.Hostname = curr_host
			Branch.Port = curr_port
			Branch.Name_with_port = Branch.Hostname + ":" + Branch.Port
		}
		Branch.Addresses[div[0]] = curr_host + ":" + curr_port
	}
	config_file.Close()

	// Initial setup
	// Listen on port for servers
	listener, lierr := net.Listen("tcp", Branch.Name_with_port)
	if lierr != nil {
		fmt.Println(lierr)
		return
	}

	num_connected := 1
	var setup_lock sync.Mutex
	wait := make(chan bool)
	// Accept connections
	go func(waiter chan bool) {
		for {
			// fmt.Println("Awaiting connections...")
			c, lerr := listener.Accept()
			if lerr != nil {
				fmt.Println("Error accepting connection")
				return
			}
			setup_lock.Lock()
			if num_connected < num_servers {
				c_ptr := new(net.Conn)
				*c_ptr = c
				// Get remote IP
				new_name := strings.Split(c.RemoteAddr().String(), ":")
				// Find in address map
				var new_idx string
				for k, v := range Branch.Addresses {
					c_addr := strings.Split(v, ":")
					name_list, _ := net.LookupAddr(new_name[0])
	
					for _, dname := range name_list {
						dname = strings.TrimSuffix(dname, ".")
						if c_addr[0] == dname {
							new_idx = dname + ":" + new_name[1]
							Branch.Addresses[k] = new_idx
							break
						}
					}
				}
	
				// fmt.Println("Accepted a connection form " + c.RemoteAddr().String())
				num_connected++
				if num_connected >= num_servers {
					waiter<-true
				}
				Branch.Servers[new_idx] = c_ptr
				setup_lock.Unlock()
				continue
			} else {
				setup_lock.Unlock()
				
			// Start listening for clients
				// fmt.Println("Client Accepted.")

				var data_in Client_msg
				decoder := gob.NewDecoder(c)
				dec_err := decoder.Decode(&data_in)
				if dec_err != nil {
					fmt.Println("Failed to read Begin.")
					c.Close()
					continue
				}

				// fmt.Println("Client Comand: " + data_in.Command)

				// Server in use
				if Branch.InTransaction {
					var busy Client_resp
					busy.Success = false
					t_encoder := gob.NewEncoder(c)
					t_encoder.Encode(&busy)
					c.Close()
				} else {
					client_ptr := new(net.Conn)
					*client_ptr = c
					var busy Client_resp
					Branch.Client = client_ptr
					Branch.InTransaction = true
					busy.Success = true
					t_encoder := gob.NewEncoder(*client_ptr)
					t_encoder.Encode(&busy)

					go Branch.Coordinator(data_in.Account)
				}
			} 
		}
		wait<-true
	}(wait)

	// Send connections
	go func(waiter chan bool) {
		for k, v := range Branch.Addresses {
			// fmt.Println(k)
			// fmt.Println(v)
			// Check if already connected or self
			present := false
			ips, _ := net.LookupIP(v)
			setup_lock.Lock()
			for _, ip := range ips {
				if _, ok := Branch.Servers[ip.String()]; ok {
					present = true
					break
				}
			}

			if k == branch || present || num_connected == num_servers {
				setup_lock.Unlock()
				continue
			}

			// Dial
			c, derr := net.Dial("tcp", v)
			if derr != nil {
				setup_lock.Unlock()
				continue
			}

			c_ptr := new(net.Conn)
			*c_ptr = c

			// Get remote IP
			new_name := strings.Split(c.RemoteAddr().String(), ":")
			var new_idx string
			// Find in address map
			for k, v := range Branch.Addresses {
				c_addr := strings.Split(v, ":")
				name_list, _ := net.LookupAddr(new_name[0])

				for _, dname := range name_list {
					// fmt.Println(dname)
					// fmt.Println(c_addr[0])
					dname = strings.TrimSuffix(dname, ".")
					if c_addr[0] == dname {
						new_idx = dname + ":" + new_name[1]
						Branch.Addresses[k] = new_idx
						break
					}
				}
			}

			// fmt.Println(c.RemoteAddr().String() + " Accepted you.")

			num_connected++
			Branch.Servers[new_idx] = c_ptr
			if num_connected >= num_servers{
				waiter<-true
			}
			setup_lock.Unlock()
		}
		
	}(wait)

	// Wait for connections to finish
	<-wait

	// Place Server Network Testing HERE
	// End Setup

	// TRANSACTION HANDLING
	// Create threads for awaiting messages from servers
	for _, con := range Branch.Servers {
		go Branch.server_comm(con)
	}

	// fmt.Println("Servers Connected.")

	<-wait
	<-wait

	// Server network test
	// fmt.Println("Address: ")
	// for k1, v1 := range addresses{
	// 	fmt.Print(k1)
	// 	fmt.Print(" ")
	// 	fmt.Println(v1)
	// }

	// fmt.Println("Servers: ")
	// for k1, _ := range servers{
	// 	fmt.Println(k1)
	// }

	// // Test reads/writes
	// for _, descriptor := range servers {
	// 	go func (desc net.Conn) {
	// 		dec := gob.NewDecoder(desc)
	// 		var acc_in Msg
	// 		dec.Decode(&acc_in)

	// 		fmt.Print("Read data: ")
	// 		// desc.Read(buf)
	// 		// data := string(buf[:])

	// 		fmt.Println(acc_in)
	// 	} (descriptor)
	// }

	// for _, descriptor := range servers {
	// 	go func (desc net.Conn) {
	// 		// data := []byte{'H', 'I'}
	// 		// desc.Write(data)

	// 		enc := gob.NewEncoder(desc)
	// 		acc_out := Msg{"dom", 1000}

	// 		fmt.Print("Write data: ")
	// 		fmt.Println(acc_out)

	// 		enc.Encode(&acc_out)

	// 	} (descriptor)
	// }
	// time.Sleep(5*time.Second)

	// End Setup

}
