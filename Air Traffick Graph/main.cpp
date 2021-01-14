#include "Graph.h"

int main(){
    std::cout << "Program is running\n";
    PNG map(1425, 625);
    map.readFromFile("World_Map.PNG");
    Graph G("Data/Airports.txt", "Data/Routes.txt", "Data/Airlines.txt", map);
    //G.draw_routes(G.get_rand_airportID());

    /*

    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>& airports = G.get_airports();
    //std::unordered_map<std::string, std::string>& airlines = G.get_airlines();
    //std::vector<Route>& routes = G.get_routes();

    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>::iterator airports_iterator = airports.begin();
    //std::unordered_map<std::string, std::string>::iterator airlines_iterator = airlines.begin();

    int max_routes = 0;
    while(airports_iterator != airports.end()){
        if((int)((*airports_iterator).second.second.size()) > 100){
            max_routes++;
        }
        // std::cout << "ID = " << (*airports_iterator).first << " Name = " << (*airports_iterator).second.second.size() << "\n";
        airports_iterator++;
    }
    std::cout << "Most airports = " << max_routes << "\n";

    */

    Airport test = G.get_airport_by_ID(G.get_rand_airportID());
    Point pixel(map.get_x_pixel(-150), map.get_y_pixel(60)); //G.map_to_pixel(test);
    std::vector<Route> adj = G.get_adjacent_by_ID(test.get_OpenFlightID());
    Airport test_dest = adj[0].get_destination();
    Point ep(map.get_x_pixel(90), map.get_y_pixel(50)); //G.map_to_pixel(test_dest);
    int num_adj = (int)adj.size();
    ColorPicker color(num_adj);
    ImageTraversal curve(map, pixel, ep, color, 50);
    ImageTraversal::Iterator pen = curve.begin();
    int i = 0;
    for(i = 0; i < 12; i++){
        ++pen;
    }

    /*

    std::string s_state = test.get_location().first;
    std::string s_country = test.get_location().second;
    std::string d_state = test.get_location().first;
    std::string d_country = test_dest.get_location().second;
    std::cout << "Start x = " << pixel.x << " End x = " << ep.x << "\n";
    std::cout << "Stary y = " << pixel.y << " End y = " << ep.y << "\n";
    std::cout << "From " << s_state << "," << s_country << " To " << d_state << "," << d_country << "\n------------------\n";
    ++pen;
    ++pen;
    ++pen;
    ++pen;
    */
    map = curve.get_image();
    map.writeToFile("World_Routes.PNG");

    return 0;
}
