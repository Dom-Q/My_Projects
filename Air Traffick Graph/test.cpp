#include "Graph.h"

int main(){
    std::cout << "Program is running\n";
    PNG map(1425, 625);
    map.readFromFile("World_Map.PNG");
    Graph G("Data/Airports.txt", "Data/Routes.txt", "Data/Airlines.txt", map);

    // Random Testing
    std::string arr[2] = {""};
    int i = 0;
    for(i = 0; i < 2; i++){
        arr[i] = (G.get_airport_by_ID(G.get_rand_airportID())).get_name();
        std::cout << arr[i] << "\n";
        Airport spec = G.get_airport_by_name(arr[i]);
        std::cout << "Lat/Long - (" << spec.get_coords().first << "," << spec.get_coords().second << ")\n";
        double x = map.get_x_pixel(spec.get_coords().second, spec.get_coords().first);
        double y = map.get_y_pixel(spec.get_coords().first, spec.get_coords().second);
        G.plot_point(x, y, true);
        int x_coord = (int)round(x);
        int y_coord = (int)round(y);
        std::cout << "Centered at - (" << x_coord << "," << y_coord << ")\n";
        std::cout << "------------------------\n";
        // G.draw_routes((G.get_airport_by_name(arr[i])).get_OpenFlightID());
    }
    G.draw_specific(G.get_airport_by_name(arr[0]), G.get_airport_by_name(arr[1]));

   /*
    // Selective Testing

    // Need to test southern hemisphere

    std::string arr[55] = {"Chicago O'Hare International Airport", 
                          "Domodedovo International Airport", 
                          "Leicester Airport", 
                          "Los Angeles International Airport",
                          "Juan Simons Vela Airport",
                          "Cabo Frio Airport",
                          "Copacabana Airport",
                          "Bunia Airport",
                          "Balmaceda Airport",
                          "Carriel Sur Airport",
                          "Tazadit Airport",
                          "Belgaum Airport",
                          "Assiut International Airport",
                          "Xinzhou Wutaishan Airport",
                          "Gwadar International Airport",
                          "Mashhad International Airport",
                          "Shaibah Airport",
                          "Krainiy Airport",
                          "Aguenar – Hadj Bey Akhamok Airport",
                          "Damazin Airport",
                          "Da Nang International Airport",
                          "Portland International Airport",
                          "Summer Beaver Airport",
                          "Badu Island Airport",
                          "Kruger Mpumalanga International Airport",
                          "Kasama Airport",
                          "Arandis Airport",
                          "Khwai River Lodge Airport",
                          "Juba International Airport",
                          "Labuan Airport",
                          "Cà Mau Airport",
                          "Madurai Airport",
                          "Nzagi Airport",
                          "Lusaka City Airport",
                          "Tôlanaro Airport",
                          "Brisbane Archerfield Airport",
                          "Port Pirie Airport",
                          "Kerang Airport",
                          "Smithton Airport",
                          "Glacier Country Heliport",
                          "Docker River Airport",
                          "Bulolo Airport",
                          "Departi Parbo Airport",
                          "Namrole Airport",
                          "Sugimanuru Airport",
                          "Lutselk'e Airport",
                          "Beaver Creek Airport",
                          "Miramichi Airport",
                          "St. Anthony Airport",
                          "Ospika Airport",
                          "Natashquan Airport",
                          "General Edward Lawrence Logan International Airport"};
    int i = 0;
    for(i = 0; i < 52; i++){
        std::cout << arr[i] << "\n";
        Airport spec = G.get_airport_by_name(arr[i]);
        std::cout << "Lat/Long - (" << spec.get_coords().first << "," << spec.get_coords().second << ")\n";
        double x = map.get_x_pixel(spec.get_coords().second, spec.get_coords().first);
        double y = map.get_y_pixel(spec.get_coords().first, spec.get_coords().second);
        G.plot_point(x, y, true);
        int x_coord = (int)round(x);
        int y_coord = (int)round(y);
        std::cout << "Centered at - (" << x_coord << "," << y_coord << ")\n";
        std::cout << "------------------------\n";
    }

    */

    /*

    std::string test_ID = G.get_rand_airportID();
    if(G.get_adjacent_by_ID(test_ID).size() < 8){
        std::cout << "Finding new airport...\n";
        test_ID = G.get_rand_airportID();
    }
    G.draw_routes(G.get_rand_airportID());

    */

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
    PNG drawn = G.get_image();
    drawn.writeToFile("World_Routes.PNG");

    return 0;
}
