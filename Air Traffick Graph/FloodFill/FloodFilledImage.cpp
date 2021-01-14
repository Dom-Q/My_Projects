#include "FloodFilledImage.h"

using namespace cs225;

/**
 * Constructs a new instance of a FloodFilledImage with a image `png`.
 * 
 * @param png The starting image of a FloodFilledImage
 */
FloodFilledImage::FloodFilledImage(const PNG & png) {
    /** @todo [Part 2] */
    image = PNG(png);
}

/**
 * Adds a FloodFill operation to the FloodFillImage.  This function must store the operation,
 * which will be used by `animate`.
 * 
 * @param traversal ImageTraversal used for this FloodFill operation.
 * @param colorPicker ColorPicker used for this FloodFill operation.
 */
void FloodFilledImage::addFloodFill(ImageTraversal & traversal, ColorPicker & colorPicker) {
    /** @todo [Part 2] */
    this->colors.push_back(&colorPicker);
    this->traversal.push_back(&traversal);
}

/**
 * Creates an Animation of frames from the FloodFill operations added to this object.
 * 
 * Each FloodFill operation added by `addFloodFill` is executed based on the order
 * the operation was added.  This is done by:
 * 1. Visiting pixels within the image based on the order provided by the ImageTraversal iterator and
 * 2. Updating each pixel to a new color based on the ColorPicker
 * 
 * While applying the FloodFill to the image, an Animation is created by saving the image
 * after every `frameInterval` pixels are filled.  To ensure a smooth Animation, the first
 * frame is always the starting image and the final frame is always the finished image.
 * 
 * (For example, if `frameInterval` is `4` the frames are:
 *   - The initial frame
 *   - Then after the 4th pixel has been filled
 *   - Then after the 8th pixel has been filled
 *   - ...
 *   - The final frame, after all pixels have been filed)
 */ 
Animation FloodFilledImage::animate(unsigned frameInterval) {
    Animation animation;
    bool added = false;
    /** @todo [Part 2] */ 
    while(!(colors.empty()) || !(traversal.empty())){
        ImageTraversal* traversal_type = traversal.front();
        traversal.pop_front();
        ImageTraversal::Iterator iterator = traversal_type->begin(); 
        ImageTraversal::Iterator end_iterator = traversal_type->end();
        ColorPicker* color = colors.front();
        colors.pop_front();
        if(!added){
            animation.addFrame(image);
            added = true;
        }
        unsigned i = 0;
        while(iterator != end_iterator){

            Point current_point = *iterator;
            HSLAPixel new_color_pixel = color->getColor();
            HSLAPixel& original_pixel = image.getPixel(current_point.x, current_point.y);
            original_pixel = new_color_pixel;
            ++iterator;
            i++;
            if((i % frameInterval) == 0){
                animation.addFrame(image);
            }
        }
        if (!((i % frameInterval) == 1)){
            animation.addFrame(image);
        }
    }
    this->image = animation.getFrame(animation.frameCount() - 1);

    return animation;
}

PNG FloodFilledImage::get_image(){
    return this->image;
}