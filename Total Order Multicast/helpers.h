#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
// Read hostname from configuration file
std::string get_hostname(std::ifstream& config_file, std::string identifier);

// Read port number from configuration file
uint16_t get_port(std::ifstream& config_file, std::string identifier);

// Read number of nodes in group from configuration file
int get_num_nodes(std::ifstream& config_file);

// Create process id based on configuration file
int get_procid(std::ifstream& config_file, std::string identifier);

// Return map of all hostnames and their associated port number (as string)
std::unordered_map<std::string, std::string> get_all_hosts(std::ifstream& config_file);
