#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <errno.h>

#include "stdio.h"
#include "ctype.h"
#include "sys/socket.h"
#include "sys/types.h"
#include <arpa/inet.h>
#include "string.h"
#include "unistd.h"
#include "netdb.h"

#include "message.h"

class Node{
    private:        
        // Data Structures 
        // PQ - sort 
        std::map<double, Message> PQ;
        // Map of accounts to balance
        std::map<std::string, int> accounts;
        
        // Data Members
        // Number of remote connections received
        int num_connections;
        
        // Total number of conecctions in group
        int total_connections;

        // Local Priority sequence number
        double priority_num;

        std::unordered_map<std::string, std::pair<Message,int>> sent;

        // Process identifier
        std::string id;
        int msg_cnt;
        // message -> frequency : track duplicates for R-Multicast
        std::unordered_map<std::string, int> reMsg;

        // message_id -> old priority val for indexing PQ
        std::unordered_map<std::string, double> old_priors;
        
        // void communicate(std::string client);
        // void stdin_listen();

        void b_multicast(std::string message);
        void send_individual(std::string data, std::string hostname);

        void check_for_deliverables();

    public:
        // Sockets for remote communcation
        std::unordered_map<std::string, int> remote_fds;
        std::string HOST_NAME_;
        std::string local_port;
        
        Node();
        
        void deposit(std::string acct_id, int amount);
        bool transfer(std::string src_id, std::string dest_id, int amount);

        void read_config();
        void write_config();

        // Getters/ Setters
        void add_connection();
        int get_num_connections();
        void set_total_connections(int num);
        int get_total_connections();
        void set_identifier(int val);
        std::string get_identifier();
        void init_priority();
        double get_curr_priority();

        // Network Handling
        void close_connection(std::string client);
        void await_connections();
        void send_connections();

        std::pair<std::string, std::string> get_accounts(std::string message);
        std::string get_account(std::string message);
        int get_ammount(std::string message);
        // Handle received message
        void handle_transaction(std::string message);

        // ISIS protocol
        void isis_reciever(std::string message_);
        void isis_sender(std::string message_);

        // Helpers
        // Parse input for command
        std::string get_cmd(std::string input);

        void communicate(std::string client);
        void stdin_listen();

        bool validate_input(std::string input);

        void print_balances();

        void connect_all(std::unordered_map<std::string, std::string> M);

        static std::mutex lock;
};

extern Node node;

