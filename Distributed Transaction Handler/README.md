Distributed Transaction Handler
-------------------------------
-> Distributed system of multiple charded servers for reliably handling concurrent transactions executed by clients

Build Instructions:

➢ Server
To run a server: From the ~/mp3/src directory run make server
To run a server: From the ~/mp3/src directory run ./server <server_id> ./config.txt
 - <server_id> should match the name associated with the calling server in the config file

➢ Client
To run a client: From the ~/mp3/src directory run make client
To run a client: From the ~/mp3/src directory run ./client <client_id> ./config.txt
