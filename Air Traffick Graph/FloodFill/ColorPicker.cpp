#include "ColorPicker.h"

using namespace cs225;

ColorPicker::ColorPicker(int routes){
    routes_ = routes;
}

HSLAPixel ColorPicker::getColor(){
    const int limit = 916;
    double fraction = (double)routes_ / limit;

    double sat = .4*fraction;
    sat += .6;
    double hue;
    if(fraction > .6875){
        hue = (fraction - .6875)/.3125;
        hue *= 25;
        hue += 345;
    }
    else{
        hue = (fraction) / .6875;
        hue *= 55;
        hue += 80;
    }

    return HSLAPixel(hue, sat, .75);
}