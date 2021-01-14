#include <cmath>
#include <iterator>
#include <iostream>

#include "../../PNG/HSLAPixel.h"
#include "../../PNG/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

ImageTraversal::Iterator::Iterator(Point start, Point end, PNG& given, ColorPicker c, double intervals_) : image(given), color(c) {
    start_point = start;
    end_point = end;
    current_point = start;
    intervals = intervals_;
    sigma_01 = 0;
    sigma_02 = 0;
    alpha_naught = 0;
    lambda_naught = 0;
    current_interval = 0;

    GC_coords(start, end);
}

void ImageTraversal::Iterator::GC_coords(Point source, Point destination){
    // Great Circle Route math for initial arc
    double phi_1 = (image.y_to_lat(source.y)) * pi / 180;
    double lambda_1 = (image.x_to_long(source.x)) * pi / 180;;
    double phi_2 = (image.y_to_lat(destination.y)) * pi / 180;
    double lambda_2 = (image.x_to_long(destination.x)) * pi / 180;

    double lambda_12 = lambda_2 - lambda_1;
    if(lambda_12 > pi){
        lambda_12 -= 2*pi;
    }
    if(lambda_12 < -pi){
        lambda_12 += 2*pi;
    }

    double vert = cos(phi_2)*sin(lambda_12);
    double horiz = (cos(phi_1) * sin(phi_2)) - (sin(phi_1) * cos(phi_2) * cos(lambda_12));
    double alpha_1 = atan2(vert, horiz); // in radians

    double vert2 = sin(alpha_1)*cos(phi_1);
    double horiz2 = cos(alpha_1)*cos(alpha_1) + sin(alpha_1)*sin(alpha_1)*sin(phi_1)*sin(phi_1);
    horiz2 = sqrt(horiz2);
    alpha_naught = atan2(vert2, horiz2); // in radians

    double vert3 = sqrt((vert*vert) + (horiz*horiz));
    double horiz3 = sin(phi_1)*sin(phi_2) + cos(phi_1)*cos(phi_2)*cos(lambda_2 - lambda_1);
    double sigma_12 = atan2(vert3, horiz3); // in radians

    // Waypoint math
    vert3 = tan(phi_1);
    horiz3 = cos(alpha_1);
    sigma_01 = atan2(vert3, horiz3);
    sigma_02 = sigma_01 + sigma_12;

    std::cout << "Sigma_01 = " << sigma_01 << "\n";

    vert3 = sin(alpha_naught)*sin(sigma_01);
    horiz3 = cos(sigma_01);
    double lambda_01 = atan2(vert3, horiz3);
    lambda_naught = lambda_1 - lambda_01;

    std::cout << "Lambda_naught = " << lambda_naught << "\n";

}


bool is_valid(unsigned curr_x, unsigned curr_y, unsigned width, unsigned height) {
    if(curr_x < width){
        if(curr_y < height){
            return true;
        }
        return false;
    }
    return false;
}

void check_end(const Point& start, Point& dest, Point ep){
    // To the right or on same long
    if(dest.x >= start.x){
        if((ep.x >= start.x) && (ep.x <= dest.x)){
            // y-cases
            if((dest.y <= start.y)){
                if((ep.y <= start.y) && (ep.y >= dest.y)){
                    dest = ep;
                    return;
                }
                return;
            }
            else{
                if((ep.y >= start.y) && (ep.y <= dest.y)){
                    dest = ep;
                    return;
                }
                return;
            }
        }
        return;
    }
    // To the left
    else{
        if((ep.x <= start.x) && (ep.x >= dest.x)){
            // y-cases
            if((dest.y <= start.y)){
                if((ep.y <= start.y) && (ep.y >= dest.y)){
                    dest = ep;
                    return;
                }
                return;
            }
            else{
                if((ep.y >= start.y) && (ep.y <= dest.y)){
                    dest = ep;
                    return;
                }
                return;
            }
        }
        return;
    }
}

void ImageTraversal::Iterator::Bresenhams(const Point& ep){
    Point moving(**this);
    std::cout << "Starting at (" << moving.x << "," << moving.y << ")\n";
    std::cout << "Going to (" << ep.x << "," << ep.y << ")\n---------------\n";
    std::cout << "Final destination at (" << end_point.x << "," << end_point.y << ")\n";
    HSLAPixel& s_next_pixel = image.getPixel(moving.x, moving.y);
    HSLAPixel& s_above_pixel = image.getPixel(moving.x, moving.y-1);
    HSLAPixel& s_above_2 = image.getPixel(moving.x, moving.y-2);
    HSLAPixel& s_below_pixel = image.getPixel(moving.x, moving.y+1);
    HSLAPixel& s_below_2 = image.getPixel(moving.x, moving.y+2);
    s_next_pixel = color.getColor();
    s_above_pixel = color.getColor();
    s_below_pixel = color.getColor();
    s_above_2 = color.getColor();
    s_below_2 = color.getColor();
    int xdelta = abs((int)(ep.x - moving.x));
    int ydelta = abs((int)(ep.y - moving.y));
    int xstep;
    int ystep;
    int decision;
    // Determine Directionn of line
    if(ep.x > moving.x){
        xstep = 1;
    }
    else if(ep.x < moving.x){
        xstep = -1;
    }
    else{
        xstep = 0;
    }
    if(ep.y > moving.y){
        ystep = 1;
    }
    else if(ep.y < moving.y){
        ystep = -1;
    }
    else{
        ystep = 0;
    }
    std::cout << "Pixel at (" << moving.x << "," << moving.y << ")\n";
    // Move using Bresenhem's alg
    if(xdelta > ydelta){
        decision = 2*ydelta - xdelta;
        while(moving.x != ep.x){
            // If decision var > 0, smaller step (in this case y) moves
            if(decision >= 0){
                moving.y += ystep;
                decision -= 2*xdelta;
            }
            // Bigger step always moves (in this case x) and decision += 2 x smaller delta
            decision += 2*ydelta;
            moving.x += xstep;
            // Draw pixels
            HSLAPixel& next_pixel = image.getPixel(moving.x, moving.y);
            HSLAPixel& above_pixel = image.getPixel(moving.x, moving.y-1);
            HSLAPixel& above_2 = image.getPixel(moving.x, moving.y-2);
            HSLAPixel& below_pixel = image.getPixel(moving.x, moving.y+1);
            HSLAPixel& below_2 = image.getPixel(moving.x, moving.y+2);
            next_pixel = color.getColor();
            above_pixel = color.getColor();
            below_pixel = color.getColor();
            above_2 = color.getColor();
            below_2 = color.getColor();
        }
    }
    else{
        decision = 2*xdelta - ydelta;
        while(moving.y != ep.y){
            if(decision >= 0){
                moving.x += xstep;
                decision -= 2*ydelta;
            }
            decision += 2*xdelta;
            moving.y += ystep;
            HSLAPixel& next_pixel = image.getPixel(moving.x, moving.y);
            HSLAPixel& above_pixel = image.getPixel(moving.x, moving.y-1);
            HSLAPixel& above_2 = image.getPixel(moving.x, moving.y-2);
            HSLAPixel& below_pixel = image.getPixel(moving.x, moving.y+1);
            HSLAPixel& below_2 = image.getPixel(moving.x, moving.y+2);
            next_pixel = color.getColor();
            above_pixel = color.getColor();
            below_pixel = color.getColor();
            above_2 = color.getColor();
            below_2 = color.getColor();
        }
    }
    return;
}

ImageTraversal::Iterator& ImageTraversal::Iterator::operator++() {
    if(current_point == end_point || current_interval == intervals){
        return *this;
    }
    std::pair<double, double> new_coords;
    
    current_interval++;

    // Great Circle Calculation for next point
    double sigma = sigma_01 + (current_interval / intervals) * (sigma_01 + sigma_02);
    double vert = cos(alpha_naught)*sin(sigma);
    double horiz = (cos(sigma)*cos(sigma)) + (sin(alpha_naught)*sin(alpha_naught)*sin(sigma)*sin(sigma));
    horiz = sqrt(horiz);
    double phi_new = atan2(vert, horiz);

    vert = sin(alpha_naught)*sin(sigma);
    horiz = cos(sigma);
    double lambda_new = atan2(vert, horiz) + lambda_naught;

    std::cout << "Sigma = " << sigma*180 / pi << "\n";
    std::cout << "Lambda_new = " << lambda_new*180 / pi << "\n";
    std::cout << "phi_new = " << phi_new*180 / pi << "\n";

    new_coords.first = phi_new*180 / pi;
    new_coords.second = lambda_new*180 / pi;

    double next_x = image.get_x_pixel(new_coords.second);
    double next_y = image.get_y_pixel(new_coords.first);
    
    std::cout << "Next_x = " << next_x << "\n";

    // Border check
    if(next_x < 0){
        next_x += image.width();
        if((*this).current_point.x < next_x){
            (*this).current_point.x = image.width() - 1;
        }
    }
    if(next_x >= image.width()){
        next_x -= image.width();
        if((*this).current_point.x > next_x){
            (*this).current_point.x = 0;
        }
    }
    if(next_y < 0){
        next_y += image.height();
        if((*this).current_point.y < next_y){
            (*this).current_point.y = image.height() - 1;
        }
    }
    if(next_y >= image.height()){
        next_y -= image.height() + 1;
        if(this->current_point.y > next_y){
            (*this).current_point.y  = 0;
        }
    }
    next_y = (unsigned)next_y;
    next_x = (unsigned)next_x;
    Point new_start(next_x, next_y);

    // Endpoint Check
    check_end(**this, new_start, end_point);

    // Draw Line
    Bresenhams(new_start);
    
    (*this).current_point = new_start;
    return *this;
}

// Dereference Current Point
Point ImageTraversal::Iterator::operator*() {
  return current_point;
}

// Compare current point held by each iterator
bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
    if(image == other.image){
        if(current_point == other.current_point){
            return false;
        }
    }
    return true;
}
