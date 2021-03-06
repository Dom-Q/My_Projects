#include "Graph.h"
Graph::Graph(std::string airport_file, std::string route_file, std::string airline_file, PNG map){
    world_map_ = map;
    srand(time(NULL));
    //***************// 
    // Data Handling //
    //***************//
    airports_ = std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>();
    std::ifstream airport_data(airport_file);
    // Map for data we want (maps title of data to data read in from file)
    std::unordered_map<std::string, std::string> var_names;

    std::pair<std::string, std::string> name_pair("name", "");
    std::pair<std::string, std::string> lat_pair("latitude", "");
    std::pair<std::string, std::string> long_pair("longitude", "");
    std::pair<std::string, std::string> city_pair("city", "");
    std::pair<std::string, std::string> country_pair("country", "");
    std::pair<std::string, std::string> ID_pair("openflightID", "");
    std::pair<std::string, std::string> abbrev_pair("abbreviation", "");
    var_names.insert(name_pair);
    var_names.insert(lat_pair);
    var_names.insert(long_pair);
    var_names.insert(city_pair);
    var_names.insert(country_pair);
    var_names.insert(ID_pair);
    var_names.insert(abbrev_pair);

    // List of each section of data in the database (in order)
    std::string headers[14] = {"openflightID", "name", "city", "country", "abbreviation", "ICAO", "latitude", "longitude",
                               "altitude", "timezone", "DST", "timezone-database", "type", "source"};

    if (airport_data.is_open()) {
        std::string curr_airport;
        while (!airport_data.eof()) {
            int i = 0;
            int start_index = 0;
            int reads = 0;
            int delim_index = 0;
            bool is_string = false;
            std::getline(airport_data, curr_airport);
            while(reads < 7){
                if(curr_airport[start_index] == '\"'){
                    is_string = true;
                    delim_index = curr_airport.find('\"', start_index + 1);
                    while (curr_airport[delim_index + 1] != ',') {
                        delim_index = curr_airport.find('\"', delim_index + 1);
                    }
                } 
                else{
                    delim_index = curr_airport.find(",", start_index);
                }
                if(var_names.count(headers[i])){
                    reads++;
                    std::string value = "";
                    if (is_string) {
                        start_index++;
                    }
                    for (int j = start_index; j < delim_index; j++) {
                        value += curr_airport[j];
                    }
                    var_names[headers[i]] = value;
                }
                start_index = delim_index + 1;
                if(is_string){
                    start_index++;
                }
                i++;
                is_string = false;
            }
            double lat_val;
            double long_val;
            if (var_names["latitude"][0] == '-'){
                lat_val = std::stod(var_names["latitude"].substr(1), NULL) * -1;
            }
            else{
                lat_val = std::stod(var_names["latitude"], NULL);
            }
            if(var_names["longtitude"][0] == '-'){
                long_val = std::stod(var_names["longitude"].substr(1), NULL) * -1;
            }
            else{
                long_val = std::stod(var_names["longitude"], NULL);
            }
            Airport new_airport(var_names["openflightID"], var_names["name"],
                                var_names["city"], var_names["country"],
                                lat_val, long_val, var_names["abbreviation"]);
            std::vector<Route> adjacent_routes = {};
            std::pair<Airport, std::vector<Route>> new_pair(new_airport, adjacent_routes);
            std::pair<std::string, std::pair<Airport, std::vector<Route>>> completed(new_airport.get_OpenFlightID(), new_pair);
            airports_.insert(completed);
        }
        airport_data.close();
    }

    std::string route_headers[9] = {"airline_code", "airline_ID", "source_code", "source_ID", "destination_code",
                                    "destination_ID", "codeshare", "stops","equipment"};
    std::ifstream route_data(route_file);

    var_names.clear();
    if (route_data.is_open()) {
        // Route data
        std::pair<std::string, std::string> source_pair("source_ID", "");
        std::pair<std::string, std::string> dest_pair("destination_ID", "");
        std::pair<std::string, std::string> ac_pair("airline_ID", "");
        std::pair<std::string, std::string> stops_pair("stops", "");
        var_names.insert(source_pair);
        var_names.insert(dest_pair);
        var_names.insert(ac_pair);
        var_names.insert(stops_pair);
        std::string curr_route;
        while (!route_data.eof()) {
            int reads = 0;
            int i = 0;
            int start_index = 0;
            int delim_index = 0;
            std::getline(route_data, curr_route);
            while (reads < 4) {
                if (curr_route[start_index] == ',') {
                    start_index++;
                    i++;
                    continue;
                }
                delim_index = curr_route.find(",", start_index);
                if (delim_index == -1) {
                    delim_index = (int)curr_route.size() - 1;
                }
                if (var_names.count(route_headers[i])) {
                    reads++;
                    std::string value = "";
                    int j;
                    j = start_index;
                    for (j = start_index; j < delim_index; j++) {
                        value += curr_route[j];
                    }
                    var_names[route_headers[i]] = value;
                }
                i++;
                start_index = delim_index + 1;
            }
            if (var_names["stops"] != "0") {
                continue;
            }
            const Airport& curr_source = get_airport_by_ID(var_names["source_ID"]);
            const Airport& curr_dest = get_airport_by_ID(var_names["destination_ID"]);
            int num_stops = std::stoi(var_names["stops"], NULL);
            Route new_route(curr_source, curr_dest, var_names["airline_ID"], num_stops);
            routes_.push_back(new_route);
            airports_[curr_source.get_OpenFlightID()].second.push_back(new_route);
        }
        route_data.close();
    }

    std::ifstream airline_data(airline_file);
    if(airline_data.is_open()){
        std::string curr_elem;
        while(!airline_data.eof()){
            int count = 0;
            int delim_index = 0;
            int start_index = 0;
            bool is_string = false;
            std::string curr_ID;
            std::string curr_name;
            bool active = true;
            std::getline(airline_data, curr_elem);
            while(delim_index < (int)curr_elem.size() - 2){
                if(curr_elem[start_index] == '\"'){
                    is_string = true;
                    start_index++;
                    delim_index = curr_elem.find('\"', start_index);
                    //std::cout << "Delim idx = " << delim_index << "\n";
                    while ((curr_elem[delim_index + 1] != ',') && ((delim_index + 2) != (int)curr_elem.size())) {
                        delim_index = curr_elem.find('\"', delim_index + 1);
                    }
                } 
                else{
                    delim_index = curr_elem.find(",", start_index);
                }
                //std::cout << "Delim idx = " << delim_index << "\n";
                if(count == 2 && delim_index != ((int)curr_elem.size() - 2)){
                    start_index = delim_index + 1;
                    if(is_string){
                        start_index++;
                    }
                    is_string = false;
                    continue;
                }
                count++;
                if(count == 1){
                    while(start_index != delim_index){
                        curr_ID += curr_elem[start_index];
                        start_index++;
                    }
                }
                else if(count == 2){
                    while(start_index != delim_index){
                        curr_name += curr_elem[start_index];
                        start_index++;
                    }
                }
                else{
                    if(curr_elem[start_index] == 'N'){
                        active = false;
                    }
                }
                start_index = delim_index + 1;
                if (is_string) {
                    start_index++;
                }
                is_string = false;
            }
            if(!active){
                continue;
            }
            std::pair<std::string, std::string> new_airline(curr_ID, curr_name);
            airlines_.insert(new_airline);
        }
        airport_data.close();
    }
}

void Graph::plot_point(double x, double y){
    unsigned x_coord = (int)round(x);
    unsigned y_coord = (int)round(y);
    x_coord += y_coord;

}

void Graph::draw_routes(std::string airport_ID){
    // Acquire adjacent airports
    const Airport& source = get_airport_by_ID(airport_ID);
    std::vector<Route> adjacent = get_adjacent_by_ID(airport_ID);
    std::vector<Airport> destinations;
    size_t i = 0;
    for(i = 0; i < adjacent.size(); i++){
        destinations.push_back(adjacent[i].get_destination());
    }
    adjacent.clear();

    int num_adjacent = (int)destinations.size();
    if(!num_adjacent){
        return;
    }
    ColorPicker curr_color(num_adjacent);

    // Create floodfill and mark source airport
    size_t length = destinations.size();
    i = 0;
    FloodFilledImage flood_fill(world_map_);
    double source_x = world_map_.get_x_pixel(source.get_coords().second);
    double source_y = world_map_.get_y_pixel(source.get_coords().first);

    plot_point(source_x, source_y);

    unsigned source_x_coord = (int)round(source_x);
    unsigned source_y_coord = (int)round(source_y);
    Point source_point(source_x_coord, source_y_coord);
    for(i = 0; i < length; i++){
        double dest_x = world_map_.get_x_pixel(destinations[i].get_coords().second);
        double dest_y = world_map_.get_y_pixel(destinations[i].get_coords().first);
        plot_point(dest_x, dest_y);
        unsigned dest_x_coord = (int)round(dest_x);
        unsigned dest_y_coord = (int)round(dest_y);

        Point curr_dest(dest_x_coord, dest_y_coord);
        ImageTraversal curr_path(world_map_, source_point, curr_dest, curr_color, 10);
        flood_fill.addFloodFill(curr_path, curr_color);
    }
    //animation_ = flood_fill.animate(100);
    //world_map_ = flood_fill.get_image();
}


const Airport& Graph::get_airport_by_ID(std::string ID) {
    return airports_[ID].first;
}

std::string Graph::get_airline_by_ID(std::string id){
    return airlines_[id];
}

std::vector<Route> Graph::get_adjacent_by_ID(std::string id){
    return airports_[id].second;
}

Point Graph::map_to_pixel(const Airport& port){
    double x = world_map_.get_x_pixel(port.get_coords().second);
    double y = world_map_.get_y_pixel(port.get_coords().first);
    unsigned x_coord = (int)round(x);
    unsigned y_coord = (int)round(y);
    return Point(x_coord, y_coord);
}


// TESTING

std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>& Graph::get_airports(){
    return airports_;
}
std::unordered_map<std::string, std::string>& Graph::get_airlines(){
    return airlines_;
}
std::vector<Route>& Graph::get_routes(){
    return routes_;
}
std::string Graph::get_rand_airportID(){
    int limit = rand() % (int)airports_.size();
    int i = 0;
    int adj = 0;
    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>::iterator it;
    int test_count = 0;
    while(!adj){
        it = airports_.begin();
        limit = rand() % (int)airports_.size();
        for(i = 0; i < limit; i++){
            ++it;
        }
        test_count++;
        if(!(test_count % 10)){
            std::cout << "Searching...\n";
        }
        adj = (int)(*it).second.second.size();
    }
    return (*it).first;
}