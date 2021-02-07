#include "ImageTraversal.h"

ImageTraversal::ImageTraversal(PNG& png, Point start, Point end, ColorPicker c, double intervals, std::pair<double, double> sc, std::pair<double, double> ec) 
                              : image_(png), color_(c){
    intervals_ = intervals;
    start_ = start;
    end_ = end;
    start_coords_ = sc;
    end_coords_ = ec;
}

ImageTraversal::Iterator ImageTraversal::begin(){
    return Iterator(start_, end_, image_, color_, intervals_, start_coords_, end_coords_);
}

ImageTraversal::Iterator ImageTraversal::end(){
    return Iterator(end_, end_, image_,color_, intervals_, start_coords_, end_coords_);
}
