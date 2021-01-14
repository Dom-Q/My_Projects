#pragma once
#include "Airport.h"
#include "Route.h"
#include "PNG/PNG.h"
#include "FloodFill/FloodFilledImage.h"
#include "FloodFill/imageTraversal/ImageTraversal.h"

#include "stdlib.h"

#include <time.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>


using namespace cs225;

class Graph{
    private:
        std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>> airports_;
        std::unordered_map<std::string, std::string> airlines_;
        std::vector<Route> routes_;
        PNG world_map_;
        Animation animation_;

    public:
        Graph(std::string airport_file, std::string route_file, std::string airline_file, PNG map);

        const Airport& get_airport_by_ID(std::string ID);
        std::string get_airline_by_ID(std::string id);
        std::vector<Route> get_adjacent_by_ID(std::string id);

        void draw_routes(std::string airport_ID);
        Point map_to_pixel(const Airport& port);
        void plot_airport(std::string airport_ID);
        void plot_point(double latitude, double longitude);

        // TESTING
        std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>& get_airports();
        std::unordered_map<std::string, std::string>& get_airlines();
        std::vector<Route>& get_routes();
        std::string get_rand_airportID();
};