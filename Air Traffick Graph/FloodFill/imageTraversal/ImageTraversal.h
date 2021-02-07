
#pragma once

#include <iterator>
#include "../../PNG/HSLAPixel.h"
#include "../../PNG/PNG.h"
#include "../Point.h"
#include "../ColorPicker.h"
#include <vector>
#include <cmath>

class ImageTraversal {
    public:
    class Iterator : std::iterator<std::forward_iterator_tag, Point>{
        public:
            Iterator(Point start, Point end, PNG& given, ColorPicker c, double intervals_, std::pair<double, double> sc, std::pair<double, double> ec);

            Iterator & operator++();
            Point operator*();
            bool operator!=(const Iterator &other);

            void GC_coords();
            void Bresenhams(const Point& ep);

        private:
            void check_end(const Point& start, Point& dest, Point ep);

            Point current_point;
            Point start_point;
            Point end_point;
            std::pair<double, double> start_coords;
            std::pair<double, double> end_coords;
            PNG& image;
            ColorPicker color;
            double intervals;
            double sigma_01;
            double sigma_02;
            double alpha_naught;
            double lambda_naught;
            double current_interval;
            const double pi = 3.1415926535;
    };

    ImageTraversal(PNG& png, Point start, Point end, ColorPicker c, double intervals, std::pair<double, double> sc, std::pair<double, double> ec);

    Iterator begin();
    Iterator end();


private:
    Point start_;
    std::pair<double, double> start_coords_;
    Point end_;
    std::pair<double, double> end_coords_;
    PNG& image_;
    ColorPicker color_;
    double intervals_;
};
