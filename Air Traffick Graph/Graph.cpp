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

    // Filter the relevant data for airport objects
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
    // This allows us to index based on what data we want (titles we want will be found in map)
    std::string headers[14] = {"openflightID", "name", "city", "country", "abbreviation", "ICAO", "latitude", "longitude",
                               "altitude", "timezone", "DST", "timezone-database", "type", "source"};

    // Fills Airport Data
    // Maps Aiport IDs to their respective airport object and all adjacent routes
    if (airport_data.is_open()) {
        std::string curr_airport;
        // Parse File
        while (!airport_data.eof()) {
            int i = 0;
            int start_index = 0;
            int reads = 0;
            int delim_index = 0;
            bool is_string = false;
            std::getline(airport_data, curr_airport);
            // Parse Line as string
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
            // Convert lat/long to double type
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
            if(lat_val < - 55){
                continue;
            }
            // Create Airport Object
            // Store in Graph
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

    // Headers for Route Data
    std::string route_headers[9] = {"airline_code", "airline_ID", "source_code", "source_ID", "destination_code",
                                    "destination_ID", "codeshare", "stops","equipment"};
    std::ifstream route_data(route_file);

    var_names.clear();
    // Fill Edge List
    // Routes (Edges) hold 2 airports (source and destination), airline that flies it
    if (route_data.is_open()) {
        // Route data
        // Filter relevant route data
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

    // Fill Airline Data
    // Maps airline ID to the name of the airline
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

// Draws a point at a specific coordinate
void Graph::plot_point(double x, double y, bool is_source){
    int x_coord = (int)round(x);
    int y_coord = (int)round(y);
    HSLAPixel dot(240, .8, .7);
    if(is_source){
        dot.h = 285;
    }
    bool visisted[9][9] = {false};
    int total = 81;
    int right_edge = world_map_.width();
    int bottom_edge = world_map_.height();
    int horiz_diff = 0;
    int vert_diff = 0;
    // Boundary Checking
    x_coord -= 4;
    y_coord -= 4;
    // Left check
    if(x_coord < 0){
        horiz_diff = 0 - x_coord;
        total -= 9*horiz_diff;
        x_coord = 0;
    }
    // Above Check
    if(y_coord < 0){
        vert_diff = 0 - y_coord;
        total -= 9*vert_diff;
        y_coord = 0;
    }
    // Right Check
    if(x_coord >= right_edge){
        horiz_diff = x_coord - (right_edge - 1);
        total -= 9*horiz_diff;
        x_coord = right_edge - 1;
    }
    // Bottom Check
    if(y_coord >= bottom_edge){
        vert_diff = y_coord - bottom_edge - 1;
        total -= 9*vert_diff;
        y_coord = bottom_edge - 1;
    }

    int x_start = x_coord;
    int y_start = y_coord;

    // Spiral traversal algorithm
    int counter = 1;
    HSLAPixel& starter = world_map_.getPixel(x_coord, y_coord);
    starter = dot;
    visisted[0][0] = true;
    int dir = 0;
    while(counter < total){
        switch(dir){
            case 0:
                while(x_coord < right_edge && x_coord - x_start < 8 && !visisted[y_coord - y_start][x_coord - x_start + 1]){
                    x_coord++;
                    visisted[y_coord - y_start][x_coord - x_start] = true;
                    if(x_coord - x_start <= 1 || x_coord - x_start >= 7 || y_coord - y_start <= 1 || y_coord - y_start >= 7){
                        HSLAPixel& current = world_map_.getPixel(x_coord, y_coord);
                        current = dot;
                    }
                    counter++;
                }
                dir = (dir + 1) % 4;
                break;
            case 1:
                while(y_coord < bottom_edge && y_coord - y_start < 8 && !visisted[y_coord - y_start + 1][x_coord - x_start]){
                    y_coord++;
                    visisted[y_coord - y_start][x_coord - x_start] = true;
                    if(x_coord - x_start <= 1 || x_coord - x_start >= 7 || y_coord - y_start <= 1 || y_coord - y_start >= 7){
                        HSLAPixel& current = world_map_.getPixel(x_coord, y_coord);
                        current = dot;
                    }
                    counter++;
                }
                dir = (dir + 1) % 4;
                break;
            case 2:
                while(x_coord >= 0 && x_coord - x_start >= 0 && !visisted[y_coord - y_start][x_coord - x_start - 1]){
                    x_coord--;
                    visisted[y_coord - y_start][x_coord - x_start] = true;
                    if(x_coord - x_start <= 1 || x_coord - x_start >= 7 || y_coord - y_start <= 1 || y_coord - y_start >= 7){
                        HSLAPixel& current = world_map_.getPixel(x_coord, y_coord);
                        current = dot;
                    }
                    counter++;
                }
                dir = (dir + 1) % 4;
                break;
            case 3:
                while(y_coord >= 0 && y_coord - y_start >= 0 && !visisted[y_coord - y_start - 1][x_coord - x_start]){
                    y_coord--;
                    visisted[y_coord - y_start][x_coord - x_start] = true;
                    if(x_coord - x_start <= 1 || x_coord - x_start >= 7 || y_coord - y_start <= 1 || y_coord - y_start >= 7){
                        HSLAPixel& current = world_map_.getPixel(x_coord, y_coord);
                        current = dot;
                    }
                    counter++;
                }
                dir = (dir + 1) % 4;
                break;
            default:
                break;
        }
    }
    return;
}

void Graph::draw_routes(std::string airport_ID){
    // Acquire adjacent airports
    const Airport& source = get_airport_by_ID(airport_ID);

    std::cout << "Routes from " << source.get_name() << ", located in " << source.get_location().first << ", " << source.get_location().second << "\n";

    std::vector<Route> adjacent = get_adjacent_by_ID(airport_ID);
    std::vector<Airport> destinations;

    size_t i = 0;
    for(i = 0; i < adjacent.size(); i++){
        destinations.push_back(adjacent[i].get_destination());
    }
    adjacent.clear();

    int num_adjacent = (int)destinations.size();

    std::cout << "There are " << num_adjacent << " adjacent airports.\n";

    if(!num_adjacent){
        return;
    }
    ColorPicker curr_color(num_adjacent);

    // Create floodfill and mark source airport
    size_t length = destinations.size();
    i = 0;
    FloodFilledImage flood_fill(world_map_);

    // Convert and plot source
    double source_x = world_map_.get_x_pixel(source.get_coords().second, source.get_coords().first);
    double source_y = world_map_.get_y_pixel(source.get_coords().first, source.get_coords().second);
    plot_point(source_x, source_y, true);
    unsigned source_x_coord = (int)round(source_x);
    unsigned source_y_coord = (int)round(source_y);
    Point source_point(source_x_coord, source_y_coord);

    // Loop through all routes
    for(i = 0; i < length; i++){
        double dest_x = world_map_.get_x_pixel(destinations[i].get_coords().second, destinations[i].get_coords().first);
        double dest_y = world_map_.get_y_pixel(destinations[i].get_coords().first, destinations[i].get_coords().second);
        plot_point(dest_x, dest_y, false);
        unsigned dest_x_coord = (int)round(dest_x);
        unsigned dest_y_coord = (int)round(dest_y);

        Point curr_dest(dest_x_coord, dest_y_coord);
        std::pair<double, double> sc = source.get_coords();
        std::pair<double, double> ec = destinations[i].get_coords();
        ImageTraversal* curr_path = new ImageTraversal(world_map_, source_point, curr_dest, curr_color, 200, sc, ec);
        flood_fill.addFloodFill(curr_path);
    }
    animation_ = flood_fill.animate(10);
    std::cout << "Writing...\n";
    animation_.write("Map.gif");
}

void Graph::draw_specific(const Airport& s, const Airport& d){
    // Convert and plot source
    double source_x = world_map_.get_x_pixel(s.get_coords().second, s.get_coords().first);
    double source_y = world_map_.get_y_pixel(s.get_coords().first, s.get_coords().second);
    plot_point(source_x, source_y, true);
    unsigned source_x_coord = (int)round(source_x);
    unsigned source_y_coord = (int)round(source_y);
    Point s_point(source_x_coord, source_y_coord);

    // Convert and plot destination
    double dest_x = world_map_.get_x_pixel(d.get_coords().second, d.get_coords().first);
    double dest_y = world_map_.get_y_pixel(d.get_coords().first, d.get_coords().second);
    plot_point(dest_x, dest_y, false);
    unsigned dest_x_coord = (int)round(dest_x);
    unsigned dest_y_coord = (int)round(dest_y);
    Point d_point(dest_x_coord, dest_y_coord);

    ColorPicker c(1);

    // Iterate/Draw route
    ImageTraversal* curr_route = new ImageTraversal(world_map_, s_point, d_point, c, 150, s.get_coords(), d.get_coords());
    ImageTraversal::Iterator I = curr_route->begin();
    while(I != curr_route->end()){
        ++I;
    }
    delete curr_route;
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

int Graph::num_routes(){
    return (int)routes_.size();
}

int Graph::num_airports(){
    return (int)airports_.size();
}

int Graph::num_airlines(){
    return (int)airlines_.size();
}

std::string Graph::find_airline(std::string name){
    std::unordered_map<std::string, std::string>::iterator I = airlines_.begin();
    while(I != airlines_.end()){
        if((*I).second == name){
            return (*I).first;
        }
        I++;
    }
    return "";
}

std::string Graph::find_airport(std::string name){
    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>::iterator I = airports_.begin();
    while(I != airports_.end()){
        if((*I).second.first.get_name().find(name) != std::string::npos){
            return (*I).first;
        }
        I++;
    }
    return "";
}

std::vector<std::string> Graph::find_by_city(std::string city){
    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>::iterator I = airports_.begin();
    std::vector<std::string> ids;
    while(I != airports_.end()){
        if((*I).second.first.get_location().first == city){
            ids.push_back((*I).second.first.get_OpenFlightID());
        }
        I++;
    }
    return ids;
}

const Airport& Graph::get_airport_by_name(std::string n){
    std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>::iterator I = airports_.begin();
    while(I != airports_.end()){
        if((*I).second.first.get_name() == n){
            return (*I).second.first;
        }
        ++I;
    }
    std::cout << "No Airport found with that name.\n";
    return {};
}

Point Graph::map_to_pixel(const Airport& port){
    double x = world_map_.get_x_pixel(port.get_coords().second, port.get_coords().first);
    double y = world_map_.get_y_pixel(port.get_coords().first, port.get_coords().second);
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

PNG Graph::get_image(){
    return world_map_;
}