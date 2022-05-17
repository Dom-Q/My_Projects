package main

import (
	"bufio"
	"encoding/gob"
	"fmt"
	"net"
	"os"
	"sync"
	"strings"
	"strconv"
)

type Client struct{
	Mu sync.Mutex
	Server net.Conn
	Resp chan Client_resp
	Occupied bool
	ServerAddrs []string // LIst of addresses "name:port"
	Name string
}


type Client_msg struct{
	Command string
	Branch string
	Account string
	Amount int
}

type Client_resp struct{
	Success bool
	Msg string
}

const num_servers int = 5 // Change to 5 when done, modify for testing

func (cli *Client) parse_config(config_path string){
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
		
		// Add server address
		cli.ServerAddrs = append(cli.ServerAddrs, curr_host + ":" + curr_port)
	}

	config_file.Close()
}

func (cli *Client) connect() {
	var resp Client_resp
	var msg Client_msg
	msg.Command = "BEGIN"
	msg.Account = cli.Name
	for _, server := range cli.ServerAddrs{ //FIX THIS AND CHANGE THE VALUE IN THE STRUCT IF WRONG
		// Connect to server
		fd, derr := net.Dial("tcp", server)
		if derr != nil {
			// fmt.Println("Failed to reach host at " + server)
			continue
		}

		// Send Begin
		enc := gob.NewEncoder(fd)
		enc.Encode(&msg)

		// Check response
		dec := gob.NewDecoder(fd)
		dec.Decode(&resp)

		// If available begin transaction
		if resp.Success {

			// fmt.Println("Beginning transaction on server: " + server)

			cli.Server = fd
			cli.Resp <- resp
			go cli.server_listen()
			return
		} 
		// else {
		// 	fmt.Println("Failed to Connect to client/")
		// }
	}

	// Failed to connect to a server
	cli.Resp <- resp
}

func (cli *Client) server_listen() {
	for {
		// PUT CODE TO LISTEN TO SERVER HERE
		// PUT RESPONSE AS RESP
		var resp Client_resp
		dec := gob.NewDecoder(cli.Server)
		dec.Decode(&resp)

		// fmt.Println("Received from Server: " + resp.Msg)

		cli.Resp <- resp
	}
}

func (cli *Client) send_request(command string, account string, amount int) {
	msg := Client_msg{Command: command, Amount: amount}

	if command == "COMMIT" || command == "ABORT"{
		msg.Account = ""
		msg.Branch = ""
	} else {
		sep := strings.Split(account, ".")
		msg.Account = sep[1]
		msg.Branch = sep[0]
	}
	enc := gob.NewEncoder(cli.Server)
	enc.Encode(&msg)

	// fmt.Println("Sent message to server: " + msg.Command + " " + strconv.Itoa(amount))
} 


func main(){
	cli := Client{
		Resp: make(chan Client_resp),
		Occupied: false}

	// Get args
	args := os.Args
	if len(args) != 3 {
		fmt.Println("Incorrect Args.")
		return
	}
	cli.Name = args[1]
	config_path := args[2]

	// Parse config, get IPs
	cli.parse_config(config_path)

	reader := bufio.NewReader(os.Stdin)
	
	for {
		// Read input
		// fmt.Print("Type Command: ")
		command, _ := reader.ReadString('\n')
		// Flush new line
		if strings.HasSuffix(command, "\n") {
			command = strings.ReplaceAll(command, "\n", "")
		}
		
		// fmt.Println("Client input: " + command) // Test input

		if command == "BEGIN"{
			//connect to server and see if it is availible
			if cli.Occupied {
				fmt.Println("OK")
				continue
			}
			
			go cli.connect()
			
			resp := <-cli.Resp
			if resp.Success{
				fmt.Println("OK")
				cli.Occupied = true
			} else {
				fmt.Println("ABORT")
				return
			}
		} else if cli.Occupied {
			if command == "COMMIT"{
				go cli.send_request(command, "", 0)
				resp := <-cli.Resp 
				if resp.Success {
					fmt.Println("COMMIT OK")
					return
				} else {
					fmt.Println("ABORTED")
					return
				}
			}
			if command == "ABORT"{
				go cli.send_request(command, "", 0)
				//resp will always be true but wait to make sure that the Abort is done before responding
				<-cli.Resp 
				fmt.Println("ABORTED")
				return
			}

			command_ := strings.Fields(command)

			// fmt.Print("Command Args: ")
			// fmt.Println(command_)

			if len(command_) < 2 {
				fmt.Println("Invalid Command. Try again.")
				continue
			}

			if command_[0] == "DEPOSIT"{
				if len(command_) != 3 {
					fmt.Println("Invalid Command. Try again.")
					continue
				}

				// Convert amount to int
				amt, bad := strconv.Atoi(command_[2])
				if bad != nil {
					fmt.Println("Bad Deposit Input")
					return
				}

				// fmt.Println("Initiating Deposit.")

				go cli.send_request(command_[0], command_[1], amt)

				resp := <- cli.Resp

				// DONT WANNA RETURN HERE RIGHT?
				if resp.Success {
					fmt.Println("OK")
				} else {
					fmt.Println("ABORTED")
					return
				}
			}
			if command_[0] == "BALANCE"{
				if len(command_) != 2 {
					fmt.Println("Invalid Command. Try again.")
					continue
				}

				go cli.send_request(command_[0], command_[1], 0)
				resp := <- cli.Resp
				if resp.Success {
					fmt.Println(resp.Msg)
				} else {
					fmt.Println("NOT FOUND, ABORTED")
					return
				}
			}
			if command_[0] == "WITHDRAW"{
				if len(command_) != 3 {
					fmt.Println("Invalid Command. Try again.")
					continue
				}

				amt, bad := strconv.Atoi(command_[2])
				if bad != nil {
					fmt.Println("Bad Deposit Input")
					return
				}
				go cli.send_request(command_[0], command_[1], amt)
				resp := <- cli.Resp
				if resp.Success{
					fmt.Println("OK")
				} else {
					fmt.Println("NOT FOUND, ABORTED")
					return
				}
			}
		}

	}
	
}