Compilation


•	To compile a node: make mp1_node

Running


•	From the ‘mp1’ directory run ./mp1_node <identifier>  <configuration file path>. Immediately after running, given a valid identifier and file path, each node will await the connections of or connect to all other nodes listed in the configuration file. Execution will only begin once connected to all nodes in the configuration file. The first line of the configuration file needs to specify total number of nodes in the network. Subsequent lines provide a node’s identifier, hostname, and port number to connect to.

Commands
  
•	DEPOSIT alphabetical account name numerical amount
  
  
• TRANSFER account name -> account name
