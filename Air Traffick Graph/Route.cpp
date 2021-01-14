#include "Route.h"

Route::Route(const Airport& source, const Airport& destination, std::string airline_code, int stops) : source_(source), destination_(destination) {
    airline_ID_ = airline_code;
    stops_ = stops;
}

const Airport& Route::get_source(){
    return source_;
}

const Airport& Route::get_destination(){
    return destination_;
}

std::string Route::get_airline_ID(){
    return airline_ID_;
}

int Route::get_stops(){
    return stops_;
}