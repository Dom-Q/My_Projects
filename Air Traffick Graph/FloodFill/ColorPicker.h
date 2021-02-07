#pragma once
#pragma once
#include "../Airport.h"

#include "../PNG/HSLAPixel.h"
#include "Point.h"

class ColorPicker{
    public:
    HSLAPixel getColor();
    ColorPicker(int routes);
    private:
    int routes_;
};