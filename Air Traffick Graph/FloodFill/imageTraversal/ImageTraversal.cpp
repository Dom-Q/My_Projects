#include "ImageTraversal.h"

ImageTraversal::ImageTraversal(PNG png, const Point & start, const Point& end, ColorPicker c, double intervals) : image_(png), color_(c){
    start_ = start;
    end_ = end;
    intervals_ = intervals;
}

ImageTraversal::Iterator ImageTraversal::begin(){
    return Iterator(start_, end_, image_, color_, intervals_);
}

ImageTraversal::Iterator ImageTraversal::end(){
    return Iterator(end_, end_, image_,color_, intervals_);
}

PNG ImageTraversal::get_image(){
    return image_;
}