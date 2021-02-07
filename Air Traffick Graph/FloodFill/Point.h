/**
 * @file Point.h
 * every 2D point of the image is represented by a Point class
 * you need to trverse all the points of the image
 */
#pragma once

#include <iostream>
#include <sstream>

struct Point {
  unsigned x;
  unsigned y;
  
  Point() : Point(0, 0) { }

  Point(unsigned x, unsigned y) : x(x), y(y) { }

  bool operator<(const Point &other) const {
    return (x < other.x || y < other.y);
  }

  bool operator==(const Point &other) const {
    return (x == other.x && y == other.y);
  }
  
  bool operator!=(const Point& other) const{
      return !(x == other.x && y == other.y);
  }
};


inline std::ostream & operator<<(std::ostream & out, Point const & point) {
  out << "(" << point.x << ", " << point.y << ")";
  return out;
}

