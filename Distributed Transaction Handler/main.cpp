#include "Node.h"
#include "helpers.h"

// Shared Node object
std::mutex Node::lock;
Node node;

// arg 1 - node id
// arg 2 - path to config file
int main(int argc, char* argv[]){
    if(argc != 3) {
        std::cout << "Incorrect Arguments\n";
        return 1;
    }
    // Get args
    std::string identifier(argv[1]);
    std::string config_path(argv[2]);

    // Open config file
    std::ifstream config_file;
    config_file.open(config_path);
    if(!config_file.is_open()){
        std::cerr << "Failed to open configuration file. Make sure config path is correct.\n";
        return 1;
    }

    // Get node name hostname and port
    node.set_total_connections(get_num_nodes(config_file) - 1);
    std::string str_hostname = get_hostname(config_file, identifier);
    node.HOST_NAME_ = str_hostname;
    uint16_t port_num = get_port(config_file, identifier);
    int proc_id = get_procid(config_file, identifier);
    node.set_identifier(proc_id);

    node.init_priority();

    node.local_port = std::to_string(port_num);

    if(port_num == 0 || str_hostname == ""){
        std::cerr << "Failed to parse config file. Make sure identifier and path are correct.\n";
        return 1;
    }

    // Socket and host structs
    struct sockaddr_in host_addr;
    struct addrinfo hints;
    struct addrinfo* addr_res;
    int host_fd;

    // Create and fill socket
    host_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(host_fd == -1){
        std::cerr << "Failed to create socket.\n";
        return 1;
    }
    memset(&host_addr, '\0', sizeof(host_addr));

    // Get host data
    char hostname[256];
    memset(hostname, '\0', 256);
    str_hostname.copy(hostname, str_hostname.size());
    
    // Assign and validate socket address data
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    if(getaddrinfo(hostname, std::to_string(port_num).c_str(), &hints, &addr_res)){
        std::cerr << "Unable to allocate original socket with given hostname and port.\n";
        return 1;
    }
    
    host_addr = *((sockaddr_in*)addr_res->ai_addr);

    // Verify address info (family, port, addr)
    // std::cout << host_addr.sin_family << "  " << ntohs(host_addr.sin_port) << "  " << inet_ntoa(host_addr.sin_addr) << "\n";

    // Bind socket
    if(-1 == bind(host_fd, (sockaddr*) &host_addr, sizeof(sockaddr_in))){
        perror("Failed to bind address to socket");
        return 1;
    }

    // Listen for connections
    if(-1 == listen(host_fd, node.get_total_connections())){
        std::cerr << "Error listening on this host.\n";
        return 1;
    }

    // std::cout << "Host " << str_hostname << " listening on port " << port_num << "\n\n";
    // std::cout << "Total Nodes = " << node.get_total_connections() << "\n\n";

    // Thread for sending out connection requests to all nodes
    std::unordered_map<std::string, std::string> all_names = get_all_hosts(config_file);
    all_names.erase(str_hostname);

    std::thread connection_thread(&Node::connect_all, &node, all_names);

    // Await all connections
    while(node.get_num_connections() < node.get_total_connections()){
        // Accept connection on new fd
        struct sockaddr_in remote_addr;
        socklen_t remote_size;
        
        int remote_fd = accept(host_fd, (sockaddr*) &remote_addr, &remote_size);
        if(node.get_num_connections() == node.get_total_connections()){
            if(remote_fd != -1) close(remote_fd);
            break;
        }

        if(remote_fd == -1){
            perror("Failed to accept socket");
            continue;
        }
        
        // std::cerr << remote_size << " - rem_size;\n";

        int host_addr = getpeername(remote_fd, (sockaddr*)&remote_addr, &remote_size);
        if(host_addr < 0){
            perror("Name not found");
            continue;
        }
        std::string hname = inet_ntoa(remote_addr.sin_addr);
        char buffer[1025];
        memset(buffer, '\0', 1025);
        int err=getnameinfo((struct sockaddr*)&remote_addr, remote_size, buffer, sizeof(buffer),0,0,0);
        if (err!=0) {
            perror("Failed to convert IP");
            continue;
        }
        hname = std::string(buffer);
        // std::cerr << "NAME = " << hname << "\n";
        if(hname.empty()){
            perror("Failed to accept host\n");
            continue;
        }

        node.lock.lock();
        if(all_names.count(hname) && !node.remote_fds.count(hname)){
            std::cerr << "Accepted connection from " << hname << "\n";
            node.remote_fds.insert(std::pair<std::string, int>(hname,remote_fd));
            // Increment node counter
            node.add_connection();
        }
        node.lock.unlock();
    }

    connection_thread.join();

    std::unordered_map<std::string, int>::iterator I;
    // I = node.remote_fds.begin();
    // while(I != node.remote_fds.end()){
    //     std::cerr << I->first << " - " << I->second << "\n";
    //     I++;
    // }
    // std::cerr << "\n";

    std::cout << "Waiting for all hosts to connect...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    
    // std::cout << "Waited 6 seconds...\n";

    // TO DO
    // Create threads for each receiving socket   
    // Current thread needs 10 connections still made 
    // ALso need to start receiving input from stdin
    // Create threads for receiving messages and wait

    // std::vector<std::thread> comm_threads;

    I = node.remote_fds.begin();
    std::vector<std::thread> threads;
    while(I != node.remote_fds.end()){
        // std::thread curr_thread(&Node::communicate, node, I->first);
        threads.emplace_back(&Node::communicate, &node, I->first);
        I++;
    }

    // Make stdin thread
    std::thread stdin_thread(&Node::stdin_listen, &node);
    
    // Join threads - comm threads should terminate when a connection breaks
    // for(int i = 0; i < node.remote_fds.size(); i++){
    //     comm_threads[i].join();
    // }

    stdin_thread.join();

    for(size_t i = 0; i < threads.size(); i++){
        if(threads[i].joinable()){
            threads[i].join();
        }
    }

    // connection_thread.join();

    return 0;
}