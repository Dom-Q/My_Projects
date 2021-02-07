/**
 * @file FloodFilledImage.h
 * Definition for a class to do flood fill on an image
 */
#pragma once

#include "../PNG/PNG.h"
#include <iostream>

#include "ColorPicker.h"
#include "imageTraversal/ImageTraversal.h"

#include "Point.h"
#include "Animation.h"

class FloodFilledImage {
public:
  FloodFilledImage(PNG& png);
  void addFloodFill(ImageTraversal* traversal);
  Animation animate(unsigned frameInterval);
  PNG get_image();

private:
    PNG& image;
    std::vector<ImageTraversal*> traversal;
};
