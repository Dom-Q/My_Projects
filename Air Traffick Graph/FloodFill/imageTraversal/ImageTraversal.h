/**
 * @file ImageTraversal.h
 */
#pragma once

#include <iterator>
#include "../../PNG/HSLAPixel.h"
#include "../../PNG/PNG.h"
#include "../Point.h"
#include "../ColorPicker.h"
#include <vector>
#include <cmath>

using namespace cs225;

class ImageTraversal {
    public:
    class Iterator : std::iterator<std::forward_iterator_tag, Point>{
        public:
            Iterator(Point start, Point end, PNG& given, ColorPicker c, double intervals_);

            Iterator & operator++();
            Point operator*();
            bool operator!=(const Iterator &other);

            void GC_coords(Point source, Point destination);
            void Bresenhams(const Point& ep);

        private:
            Point current_point;
            Point start_point;
            Point end_point;
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

    ImageTraversal(PNG png, const Point & start, const Point& end, ColorPicker c, double intervals);

    Iterator begin();
    Iterator end();

    //Test
    PNG get_image();

private:
    Point start_;
    Point end_;
    PNG image_;
    ColorPicker color_;
    double intervals_;
};
