#include "Graph.h"
#include "helpers.cpp"
#include <climits>

int main(){
    PNG map(1425, 625);
    map.readFromFile("World_Map.PNG");
    Graph G("Data/Airports.txt", "Data/Routes.txt", "Data/Airlines.txt", map);

    int in = 0;
    bool first = true;
    while(in != 1 && in != 2 && in != 3){
        if(first){
            first = false;
        }
        else{
            std::cout << "Invalid input. Enter option 1, 2 or 3\n";
        }

        std::cout <<"===========================================\n"
                    "                   MENU                   \n"
                    "                  ------                  \n"                
                    "          Type 1 for Route info           \n"
                    "          Type 2 for Airport info         \n"
                    "          Type 3 for Airline info         \n"
                    "   -------------------------------------  \n"
                    "     Note: Enter all names with proper    \n"
                    "     capitalization and punctuation when  \n"
                    "     prompted during the program.         \n"
                    "===========================================\n";
        std::cin >> in;
    }
    // Menu option
    switch(in){
        // Route Case
        case 1: {
            int num_routes = G.num_routes();
            std::cout << "\nWhat would you like to know about the world's " << num_routes <<  " flight routes?\n\n";
            std::cout <<"1. Connectivity of specific airports\n"
                        "2. Draw flight paths\n"
                        "3. Flight Distances\n"; // Use iterator to calc 3959*sigma12
            int routes_option = 0;
            std::cin >> routes_option;

            std::string source_name;
            std::string dest_name;
            std::string source_id = "";
            std::string dest_id = "";
            bool first = true;
            // Get source and check existence
            while(source_id == ""){
                if(!first) std::cout << "There was no airport found with that name.\n";
                else first = false;

                std::cout << "Type the name of the airport the flight will leave from\n";
                char junk; scanf("%c", &junk);
                std::getline(cin, source_name);
                source_id = G.find_airport(source_name);
            }
            
            // Get Dest Airport
            first = true;
            while(dest_id == ""){
                if(!first) std::cout << "There was no airport found with that name.\n";
                else first = false;
                
                std::cout << "Type the name of the destination airport.\n";
                // char junk; scanf("%c", &junk);
                std::getline(cin, dest_name);
                dest_id = G.find_airport(dest_name);
            }

            switch(routes_option){
                // Connectivity
                case 1: {
                    // Check source adjacency list
                    std::vector<Route> adj = G.get_adjacent_by_ID(source_id);
                    int length = (int)adj.size();
                    int i = 0;
                    for(i = 0; i < length; i++){
                        if(adj[i].get_destination().get_OpenFlightID() == dest_id){
                            std::cout << "\nThere is a flight from " << source_name << " to " << dest_name << "!\n";
                        }
                        break;
                    }
                    if(i == length){
                        std::cout << "There is no flight from " << source_name << " to " << dest_name << ".\n";
                    }
                    break;
                }
                
                // Route Drawing
                case 2:{
                    G.draw_specific(G.get_airport_by_ID(source_id), G.get_airport_by_ID(dest_id));
                    G.get_image().writeToFile("World_Routes.PNG");
                    // Check source adjacency list
                    std::vector<Route> adj = G.get_adjacent_by_ID(source_id);
                    int length = (int)adj.size();
                    int i = 0;
                    for(i = 0; i < length; i++){
                        if(adj[i].get_destination().get_OpenFlightID() == dest_id){
                            std::cout << "There is an active route from " << source_name << " to " << dest_name << "!\n";
                            break;
                        }
                    }   
                    if(i == length){
                        std::cout << "There is no active route from " << source_name << " to " << dest_name << ".\n";
                    }   
                    break;         
                }

                // Distance Calculation
                case 3:{
                    // Use iterator to calculate sigma_12
                    Point s(map.get_x_pixel(G.get_airport_by_ID(source_id).get_coords().second, G.get_airport_by_ID(source_id).get_coords().first), 
                            map.get_y_pixel(G.get_airport_by_ID(source_id).get_coords().first, G.get_airport_by_ID(source_id).get_coords().second));
                    Point e(map.get_x_pixel(G.get_airport_by_ID(dest_id).get_coords().second, G.get_airport_by_ID(dest_id).get_coords().first), 
                            map.get_y_pixel(G.get_airport_by_ID(dest_id).get_coords().first, G.get_airport_by_ID(dest_id).get_coords().second));
                    ColorPicker c(10);
                    ImageTraversal I(map, s, e, c, 100, G.get_airport_by_ID(source_id).get_coords(), G.get_airport_by_ID(dest_id).get_coords());
                    ImageTraversal::Iterator iter = I.begin();
                    // Calculate total distance
                    int distance = (int)iter.calc_length();
                    std::cout << "The great-circle distance from " << source_name << " to " << dest_name << " is " << distance << " miles.\n";
                    break;
                }

                default: {
                    std::cout << routes_option << " is not a valid option.\n";
                    break;
                }
            }
        }
        break;
        
        // Airport Case
        case 2: {
            int num_airports = G.num_airports();
            std::cout << "\nWhat would you like to know about the world's " << num_airports <<  " airports?\n\n";
            std::cout <<"1. Outgoing routes\n"
                        "2. Locate on map\n"
                        "3. Find Airports\n";
            int airports_option = 0;
            std::cin >> airports_option;

            switch(airports_option){
                // Outgoing routes
                case 1: {
                    // Get airport name
                    std::string ap_name;
                    std::string ap_id = "";
                    bool first = true;
                    while(ap_id == ""){
                        if(!first) std::cout << "There is no airport with that name.\n";
                        else first = false;

                        std::cout << "Enter the name of the airport\n";
                        char junk; scanf("%c", &junk);
                        std::getline(cin, ap_name);
                        ap_id = G.find_airport(ap_name);
                    }
                    // Print adjacent
                    std::cout << "\nAll possible destinations flying out of " << ap_name << ":\n";
                    std::vector<Route> adj = G.get_adjacent_by_ID(ap_id);
                    int k = 0;
                    int length = (int)adj.size();
                    for(k = 0; k < length; k++){
                        std::cout << k + 1 << ". " << adj[k].get_destination().get_name() << "\n"; 
                    }
                    // Draw outgoing routes
                    G.draw_routes(ap_id);
                    G.get_image().writeToFile("World_Routes.PNG");
                    break;
                }


                // Locate Airport 
                case 2: {
                    // Print city/state and draw on map
                    

                }

                // Find Airports in an area
                case 3: {
                    std::cout <<"\n1. Search by city\n"
                                  "2. Search by country\n";
                    int type = 0;
                    std::cin >> type;
                    type--;
                    std::string location;
                    // City
                    if(!type){
                        std::cout << "Enter the name of a city\n";
                        char junk; scanf("%c", &junk);
                        std::getline(cin, location);
                        
                        std::cout << "\n" << location << "\n";

                        std::vector<std::string> ids = G.find_by_city(location);
                        if(ids.empty()){
                            "There are no airports found in a city with that name\n";
                            break;
                        }
                        std::cout << "All airports in " << location << ":\n";
                        int num_ids = (int)ids.size();
                        int i = 0;
                        for(i = 0; i < num_ids; i++){
                            Airport current = G.get_airport_by_ID(ids[i]);
                            int xp = map.get_x_pixel(current.get_coords().second, current.get_coords().first);
                            int yp = map.get_y_pixel(current.get_coords().first, current.get_coords().second);
                            G.plot_point(xp, yp, true);
                            std::cout << i + 1 << ". " << current.get_name() << "\n"; 
                        }
                        G.get_image().writeToFile("World_Routes.PNG");
                    }
                    // Country
                    else{

                    }
                    break;
                }

                default:{
                    std::cout << airports_option << " is not a valid option.\n";
                    break;
                }
            }         

        }
        break;
        
        // Airline Case
        case 3: {
            int num_airlines = G.num_airlines();
            std::cout << "\nWhat would you like to know about the world's " << num_airlines <<  " airlnes?\n\n";
            std::cout <<"1. Find airlines at specific airport\n"
                        "2. Find airline on a specific route\n"
                        "3. Find airlnes in a specific location\n";
            int airline_option = 0;
            std::cin >> airline_option;
            
            switch(airline_option) {
                // Airlines at an airport -- (all and specific??)
                case 1: {

                }

                // Airlines tjat fly specific route
                case 2: {

                }

                // Airline based on location
                case 3: {

                }

                default: {
                    std::cout << airline_option << " is not a valid option.\n";
                    break;
                }
            }

            // Get airline name from user
            std::string airline_ID = "";
            bool first = true;
            while(airline_ID == ""){
                if(!first) std::cout << "There is no active airline with that name.\n";
                else first = false;

                std::string airline_name;
                std::cout << "Type the name of an airline\n";
                char junk; scanf("%c", &junk);
                getline(std::cin, airline_name);
                
                std::string airline_ID = G.find_airline(airline_name);
            }

        }
        break;

        default:
            break;
    }
    return 0;
}