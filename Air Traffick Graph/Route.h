#pragma once
#include "Airport.h"

#include <string>

class Route{
    private:
        const Airport& source_;
        const Airport& destination_;
        std::string airline_ID_;
        int stops_;
    public:
        Route(const Airport& source, const Airport& destination, std::string airline_ID, int stops);
        const Airport& get_source();
        const Airport& get_destination();
        std::string get_airline_ID();
        int get_stops();
};