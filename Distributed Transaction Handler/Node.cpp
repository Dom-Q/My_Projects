#include "Node.h"

// Constructors
Node::Node(){
    accounts.clear();
    PQ.clear();
    old_priors.clear();
    priority_num = 0;
    num_connections = 0;
}

// Getters
int Node::get_num_connections(){
    return num_connections;
}

int Node::get_total_connections(){
    return total_connections;
}

std::string Node::get_identifier(){
    return id;
}

double Node::get_curr_priority(){
    return priority_num;
}

// Setters
void Node::add_connection(){
    num_connections++;
}

void Node::set_total_connections(int num){
    total_connections = num;
}

void Node::set_identifier(int val){
    id = std::to_string(val);
}

void Node::init_priority(){
    priority_num = std::stod(id) / ( (int)id.size()*10 );
}


// void Node::connect_all(std::unordered_map<std::string, std::string> M){
//     std::unordered_map<std::string, std::string>::iterator I = M.begin();
//     while(I != M.end()){
//         if(remote_fds.count(I->first)){
//             I++;
//             continue;
//         }
//         std::cout << "C\n";
//         // Socket and host structs
//         struct sockaddr_in host_addr;
//         struct addrinfo hints;
//         struct addrinfo* addr_res;
//         int new_fd;

//         // Create and fill socket
//         new_fd = socket(AF_INET, SOCK_STREAM, 0);
//         std::cout<< new_fd << "\n";
//         if(new_fd == -1){
//             std::cerr << "Failed to create socket.\n";
//             continue;
//         }
//         memset(&host_addr, '\0', sizeof(host_addr));

//         // Get host data
//         char hostname[256];
//         memset(hostname, '\0', 256);
//         I->first.copy(hostname, I->first.size(), 0);
        
//         // Assign and validate socket address data
//         hints.ai_family = AF_INET;
//         hints.ai_socktype = SOCK_STREAM;
//         hints.ai_protocol = 0;
//         if(getaddrinfo(hostname, I->second.c_str(), &hints, &addr_res)){
//             std::cerr << "Unable to allocate socket for connecting to port " << I->first << "\n";
//             I++;
//             close(new_fd);
//             continue;
//         }
        
//         host_addr = *((sockaddr_in*)addr_res->ai_addr);

//         if(!remote_fds.count(I->first)){
//             std::cerr << "F.\n";
//             if(connect(new_fd, (sockaddr*)&host_addr, sizeof(host_addr)) < 0){
//                 close(new_fd);
//                 std::cerr << "Couldn't connect to host " << I->first << "\n";
                
//                 I++;
//                 continue;
//             }
//             std::cerr << "Ffff.\n";
//             num_connections++;
//             remote_fds[I->first] = new_fd;
//             I++;
//         }
//         else{
//             close(new_fd);
//             I++;
//         }
//     }
// }

void Node::connect_all(std::unordered_map<std::string, std::string> M){
    std::unordered_map<std::string, std::string>::iterator I = M.begin();
    std::unordered_set<std::string> sent;
    while((int)sent.size() < total_connections){
        if(I == M.end()){
            I = M.begin();
        }
        if(sent.count(I->first)){
            I++;
            continue;
        }
        // Socket and host structs
        struct sockaddr_in host_addr;
        struct addrinfo hints;
        struct addrinfo* addr_res;
        int new_fd;

        // Create and fill socket
        new_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(new_fd == -1){
            std::cerr << "Failed to create socket.\n";
            continue;
        }
        memset(&host_addr, '\0', sizeof(host_addr));

        // Get host data
        char hostname[256];
        memset(hostname, '\0', 256);
        I->first.copy(hostname, I->first.size(), 0);
        
        // Assign and validate socket address data
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        if(getaddrinfo(hostname, I->second.c_str(), &hints, &addr_res)){
            std::cerr << "Unable to allocate socket for connecting to port " << I->first << "\n";
            I++;
            close(new_fd);
            continue;
        }
        
        host_addr = *((sockaddr_in*)addr_res->ai_addr);


        node.lock.lock();
        if(remote_fds.count(I->first)){
            close(new_fd);
            sent.insert(I->first);
            I++;
            node.lock.unlock();
        }
        else if(connect(new_fd, (sockaddr*)&host_addr, sizeof(host_addr)) < 0){
            close(new_fd);
            // std::cerr << "Couldn't connect to host " << I->first << "\n";
            
            I++;
            node.lock.unlock();
            continue;
        }
        else{
            num_connections++;
            sent.insert(I->first);
            remote_fds[I->first] = new_fd;
            std::cerr << "Connected to host " << I->first << "\n";
            I++;
            if(num_connections == total_connections){   
                // Socket and host structs
                // Create and fill socket
                new_fd = socket(AF_INET, SOCK_STREAM, 0);
                memset(&host_addr, '\0', sizeof(host_addr));

                // Get host data
                char hostname[256];
                memset(hostname, '\0', 256);
                HOST_NAME_.copy(hostname, HOST_NAME_.size(), 0);
                
                // Assign and validate socket address data
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = 0;
                getaddrinfo(hostname, local_port.c_str() , &hints, &addr_res);
                host_addr = *((sockaddr_in*)addr_res->ai_addr);
                connect(new_fd, (sockaddr*) &host_addr, sizeof(host_addr));
                close(new_fd);
            }
            node.lock.unlock();
        }
    }
}

// Communicate on a socket - listen for and process incoming messages
void Node::communicate(std::string client){
    // Buffer
    char buf[256];
    int fd = remote_fds[client];
    while(true){
        memset(buf, '\0', 256);
        
        // std::cerr << "Awaiting Message...\n";

        ssize_t read_bytes = recv(fd, buf, 256, 0);

        // std::cerr << "Read " << read_bytes << " bytes\n";

        // Connection broken
        if(read_bytes < 0){

            perror(client.c_str());

            close_connection(client);
            return;
        }
        
        
        // Connection still made - process message
        if(num_connections != 0){
            std::string incoming_message(buf);

            // std::cout << incoming_message << ":MESSAGE\n";

            if(incoming_message.size() >= 256){

                std::cerr << "Invalid command  - too large";
                
                continue;
            }

            // std::cerr << "INCOMING: " << incoming_message << "\n";
            
            reMsg[incoming_message]++;
            if(reMsg[incoming_message] == 1){
                //Recieve thread
                // std::cerr << "Processing: " << incoming_message << "\n";
                std::thread run_t(&Node::isis_reciever, this, incoming_message);
                run_t.detach();
            }

            // else
            if(reMsg[incoming_message] == node.get_num_connections()){
                // std::cerr << "Deleting msg\n";
                reMsg.erase(incoming_message);
            }
        }

        // A connection was lost
        else{
            close_connection(client);
            return;
        }
    }
    return;
}

// Wait for standard input and send messages when received
void Node::stdin_listen(){
    // Infinitely wait for input
    // Thread will die when server goes out of scope
    std::string input;

    std::cout << "Transaction Processing Available\n";

    while(true){
        // Check connections
        if(num_connections > 0){
            input.clear();

            std::getline(std::cin, input);

            
            if(validate_input(input)){
                // Create message obj / multicast msg
                std::string id_ = id + "-" + std::to_string(msg_cnt);
                msg_cnt++;
                Message M(input, id_, HOST_NAME_, node.get_curr_priority());
                std::string msg_out = M.msgToString();

                // std::cout << msg_out << "\n";

                // SEND THREAD ??
                std::thread thread_start(&Node::isis_sender, this, msg_out);
                thread_start.join();
            }
            else{
                std::cout << "Input not valid. Try again.\n";
            }
        }

        // No connections. end thread
        else{
            return;
        }
    }
    return;
}

// ISIS protocol
void Node::isis_sender(std::string message_){ 
    b_multicast(message_);
    Message msg(message_);

    sent.insert(std::pair<std::string, std::pair<Message, int>>(msg.get_id(), std::pair<Message, int>(msg,0)));
}

void Node::isis_reciever(std::string message_){
    Message msg = Message(message_);

     // check if needed probaly not but put in in case
    if(msg.getType() == INIT){
        //set priority to higher that all previos msgs

        // b_multicast(message_);//multicast if first time seeing message

        msg.set_priority(priority_num);
        priority_num = std::max(priority_num, msg.getPriority());
        //reply to sender with proposed priority
        msg.set_Type(RESP);
        send_individual(msg.msgToString(), msg.get_HOST_NAME());
        //add to priority queue
        lock.lock();
        if(!PQ.count(msg.getPriority())){
            old_priors.insert(std::pair<std::string, double>(msg.get_id(), msg.getPriority()));
            PQ.insert(std::pair<double, Message>(msg.getPriority(),msg));
            priority_num += 1;
        }
        lock.unlock();

    } 
    else if(msg.getType() == FINAL){
        //add in check to see if final message needs to be multicast

        // b_multicast(message_);
        lock.lock();
        std::string idx = msg.get_id();
        PQ.erase(old_priors[idx]);
        PQ.insert(std::pair<double, Message>(msg.getPriority_f(), msg));
        old_priors.erase(msg.get_id());
        check_for_deliverables();
        lock.unlock();

    } 
    else if(msg.getType() == RESP){
        
        std::string curr_id = msg.get_id();
        
        lock.lock();
        if(sent[curr_id].second == 0) PQ.insert(std::pair<double, Message>(msg.getPriority(), msg));
        sent[curr_id].second++;

        double prev_p = sent[curr_id].first.getPriority_f(); // prvious priority
        if(prev_p < msg.getPriority()){
            sent[curr_id].first.set_priority_f(msg.getPriority());
        }

        if(sent.count(curr_id) && sent[curr_id].second == node.get_num_connections()){
            priority_num = std::max(priority_num + 1, sent[curr_id].first.getPriority_f());

            sent[curr_id].first.set_Type(FINAL);

            std::map<double, Message>::iterator I = PQ.begin();
            while(I != PQ.end()){
                if(I->second.get_id() == curr_id){
                    PQ.erase(I->first);
                    PQ.insert(std::pair<double, Message>(sent[curr_id].first.getPriority_f(), sent[curr_id].first));
                    break;
                }
                I++;
            }
            
            // std::cout << "FINAL MSG: " << sent[curr_id].first.msgToString() << "\n";

            b_multicast(sent[curr_id].first.msgToString());
            check_for_deliverables();
            sent.erase(curr_id);//need to do get rid of all copys of message
        }
        lock.unlock();
    }
}

void Node::handle_transaction(std::string message){
    // Parse command
    std::string cmd = get_cmd(message);
    // Invalid cmd
    if(cmd == "deposit"){
        std::string account = get_account(message);
        int ammount = get_ammount(message);
        accounts[account] += ammount;
    }
    // Deposit
    else if(cmd == "transfer"){
        std::pair<std::string, std::string> users = get_accounts(message);
        int ammount = get_ammount(message);

        if(accounts.count(users.first)){
            if(accounts[users.first] - ammount >= 0){
                accounts[users.second] += ammount;
                accounts[users.first] -= ammount;
            }
            else{
                std::cerr << "Source does not have sufficient funds.\n";
            }
        }
        else{
            std::cerr << "Source does not exist.\n";
        }
    }
    // Transfer
    else{
        std::cerr << "Invalid Command.\n";
    }
    
}

// Send message to specific host
void Node::send_individual(std::string message, std::string hostname){
    char buf[256];
    memset(buf, '\0', 256);
    size_t length = message.size() >= 256 ? 255:message.size();
    message.copy(buf, length, 0);
    int curr_fd = remote_fds[hostname];

    ssize_t bytes_sent = send(curr_fd, buf, 256, 0);

    // std::cerr << "Wrote " << bytes_sent << " bytest to socket " << curr_fd << "\n";

    return;
}

// B - MultiCast to all other nodes
void Node::b_multicast(std::string message){
    // std::cout << "B multicast\n"; 

    std::unordered_map<std::string, int>::iterator I = remote_fds.begin();
    while(I != remote_fds.end()){
        send_individual(message, I->first);
        I++;
    }
    // std::cerr < "LEFT_BM\n";
    return;
}

//look at top of queue and check if any messages can be delivered
void Node::check_for_deliverables(){
    std::map<double, Message>::iterator It = PQ.begin();

    // std::cerr << PQ.size() << " possible deliverables:\n";

    // while(It != PQ.end()){
    //     std::cout << "- " << It->second.msgToString() << "\n";
    //     It++;
    // }

    // It = PQ.begin();

    while(It != PQ.end() && It->second.getType() == FINAL){
        
        // std::cerr << "Handling deliverable: " << It->second.msgToString();

        std::string transaction = It->second.get_data();
        PQ.erase(It);
        handle_transaction(transaction);
        It = PQ.begin();
    }
    print_balances();
    return;
}

void Node::print_balances(){
    std::string out("BALANCES");
    std::map<std::string, int>::iterator I = accounts.begin();
    
    while(I != accounts.end()){
        if(I->second <= 0){
            I = accounts.erase(I);
            continue;
        }
        std::string next_acc;
        next_acc += " ";
        next_acc += I->first;
        next_acc += ":";
        next_acc += std::to_string(I->second);
        out += next_acc;
        I++;
    }

    std::cout << out << "\n";
    return;
}

// Close a connection on a socket
// Decrease connection counter, erase from fd list and close socket
void Node::close_connection(std::string client){
    close(remote_fds[client]);
    num_connections--;
    remote_fds.erase(client);
    return;
}

// Helpers

std::string Node::get_cmd(std::string input){
    int space_1 = input.find_first_of(' ');
    if(space_1 == -1){
        return "";
    }
    // Parse command
    std::string cmd = input.substr(0, space_1);
    size_t cmd_len = cmd.size();
    for(size_t i = 0; i < cmd_len; i++){
        cmd[i] = tolower(cmd[i]);
    }
    return cmd;
}

std::pair<std::string, std::string> Node::get_accounts(std::string message){
    int space_1 = message.find_first_of(' ');
    int space_2 = message.find(' ', space_1 + 1);
    int space_3 = message.find_last_of(' ');
    std::pair<std::string, std::string> accs;
    accs.first = message.substr(space_1+1, space_2 - space_1 - 1);
    
    accs.second = message.substr(space_2+4, space_3 - space_2 - 4);
    return accs;
}

std::string Node::get_account(std::string message){
    int space_1 = message.find_first_of(' ');
    if(space_1 == -1){
        return "";
    } 
    int space_2 = message.find_first_of(' ', space_1 + 1);
    if(space_2 == -1){
        return "";
    } 
    std::string acc = message.substr(space_1+1, space_2 - space_1 - 1);
    return acc;
}
int Node::get_ammount(std::string message){
    int space = message.find_last_of(' ');
    if(space == -1){
        return 0;
    } 
    return std::stoi(message.substr(space + 1));
}

bool Node::validate_input(std::string input){
    if(get_cmd(input) == "deposit"){
        int first = input.find(' ');
        int second = input.find_last_of(' ');
        if(second <= first) return false;
        for(int i = first + 1; i < second; i++){
            if(input[i] == ' ' || input[i] == '/' ||input[i] == '-') return false;
        }
        for(int i = second + 1; i < (int)input.size(); i++){
            if(input[i] < '0' || input[i] > '9') return false;
        }
        return true;
    }
    else if(get_cmd(input) == "transfer"){
        int space_1 = input.find_first_of(' ');
        int space_2 = input.find(' ', space_1 + 1);
        int space_3 = input.find_last_of(' ');
        for(int i = space_1 + 1; i < space_2; i++){
            if(input[i] == ' ' || input[i] == '/' ||input[i] == '-') return false;
        }
        if(input[space_2 + 1] != '-') return false;
        if(input[space_2 + 2] != '>') return false;
        for(int i = space_3 + 1; i < (int)input.size(); i++){
            if(input[i] < '0' || input[i] > '9') return false;
        }
        return true;
    }
    else return false;
    return false;
}