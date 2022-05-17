#include "helpers.h"

std::string get_hostname(std::ifstream& config_file, std::string identifier){
    config_file.clear();
    config_file.seekg(0);
    std::string curr_line;
    std::string out = "";
    while(!config_file.eof()){
        out.clear();
        std::getline(config_file, curr_line);
        if(curr_line.size() < identifier.size()) continue;

        int space_1 = curr_line.find_first_of(' ');
        int space_2 = curr_line.find_first_of(' ', space_1 + 1);

        out = curr_line.substr(0, space_1);

        if(out == identifier) return curr_line.substr(space_1 + 1, space_2 - space_1 - 1);
        out.clear();
    }
    return out;
}

uint16_t get_port(std::ifstream& config_file, std::string identifier){
    config_file.clear();
    config_file.seekg(0);
    uint16_t out = 0;
    std::string curr_line;
    while(!config_file.eof()){
        if(config_file.tellg() == 0){
            std::getline(config_file, curr_line);
        }
        std::getline(config_file, curr_line);

        int space_1 = curr_line.find_first_of(' ');
        int space_2 = curr_line.find_first_of(' ', space_1 + 1);
        
        std::string name = curr_line.substr(0, space_1);
        if(name == identifier){
            out = std::stoi(curr_line.substr(space_2 + 1));
            return out;
        }
    }
    return out;
}

int get_num_nodes(std::ifstream& config_file){
    config_file.clear();
    config_file.seekg(0);

    std::string out;
    std::getline(config_file, out);
    return std::stoi(out);
}

int get_procid(std::ifstream& config_file, std::string identifier){
    config_file.clear();
    config_file.seekg(0);

    int proc = 0;
    std::string curr_line;
    while(!config_file.eof()){
        std::getline(config_file, curr_line);
        int end = curr_line.find_first_of(' ');
        if(curr_line.substr(0, end) == identifier){
            return proc;
        }
        proc++;
    }
    return -1;
}

std::unordered_map<std::string, std::string> get_all_hosts(std::ifstream& config_file){
    config_file.clear();
    config_file.seekg(0);

    std::unordered_map<std::string, std::string> out;
    std::string curr_line;
    std::getline(config_file, curr_line);
    int num_nodes = std::stoi(curr_line);
    for(int i = 0; i < num_nodes; i++){
        curr_line.clear();
        std::getline(config_file, curr_line);
        int space_1 = curr_line.find_first_of(' ');
        int space_2 = curr_line.find(' ', space_1 + 1);

        std::string hostname = curr_line.substr(space_1 + 1, space_2 - space_1 - 1);
        std::string port = curr_line.substr(space_2 + 1);

        // std::cerr << hostname << " " << port << "\n";

        out.insert(std::pair<std::string, std::string>(hostname, port));
    }
    return out;
}
