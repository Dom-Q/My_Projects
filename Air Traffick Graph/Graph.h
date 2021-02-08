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


class Graph{
    private:
        // Graph Stucture
        // ---------------
        // Hash-Map mapping airport OpenFlightIDs to a pair containing the Airport object and all of its edges (Routes)
        // Edges are Routes, stored in a vector
        // Airline names stored in a map so they can be indexed by OpenFlightID (Routes and Airports only store the ID)
        std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>> airports_;
        std::unordered_map<std::string, std::string> airlines_;
        std::vector<Route> routes_;

        // Graph also stores the PNG image and the gif animation
        PNG world_map_;
        Animation animation_;

    public:
        // Constructor
        Graph(std::string airport_file, std::string route_file, std::string airline_file, PNG map);

        // Read from graph
        const Airport& get_airport_by_ID(std::string ID);
        std::string get_airline_by_ID(std::string id);
        std::vector<Route> get_adjacent_by_ID(std::string id);

        int num_routes();
        int num_airports();
        int num_airlines();

        // Find in Graph by name
        std::string find_airline(std::string name);
        std::string find_airport(std::string name);
        std::vector<std::string> find_by_city(std::string city);

        // Draw routes
        void draw_routes(std::string airport_ID);
        void draw_specific(const Airport& s, const Airport& d);

        Point map_to_pixel(const Airport& port);
        void plot_point(double x, double y, bool is_source);

        PNG get_image();
        // TESTING
        std::unordered_map<std::string, std::pair<Airport, std::vector<Route>>>& get_airports();
        std::unordered_map<std::string, std::string>& get_airlines();
        std::vector<Route>& get_routes();
        std::string get_rand_airportID();
        const Airport& get_airport_by_name(std::string n);
};