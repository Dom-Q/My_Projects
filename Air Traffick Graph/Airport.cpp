#include "Airport.h"

Airport::Airport() {
    OpenFlightID = "-1";
    name_ = "";
    location_ = std::make_pair("", "");
    coordinates_ = std::make_pair(0, 0);
    abbreviation_ = "";
}

Airport::Airport(std::string ID, std::string name, std::string city,
                 std::string country, double latitude, double longitude,
                 std::string code) {
    OpenFlightID = ID;
    name_ = name;
    location_.first = city;
    location_.second = country;
    coordinates_.first = latitude;
    coordinates_.second = longitude;
    abbreviation_ = code;
}

Airport::Airport(const Airport& other) {
    OpenFlightID = other.OpenFlightID;
    name_ = other.name_;
    location_.first = other.location_.first;
    location_.second = other.location_.second;
    coordinates_.first = other.coordinates_.first;
    coordinates_.second = other.coordinates_.second;
    abbreviation_ = other.abbreviation_;
}

std::pair<double, double> Airport::get_coords() const{ 
    return coordinates_; 
}

std::pair<std::string, std::string> Airport::get_location() const {
    return location_;
}

std::string Airport::get_name() const {
    return name_;
}

std::string Airport::get_abbreviation() const{ 
    return abbreviation_;
}

std::string Airport::get_OpenFlightID() const{
    return OpenFlightID;
}

bool Airport::operator==(const Airport& other) const {
    if (OpenFlightID == other.OpenFlightID) {
        return true;
    }
    return false;
}
