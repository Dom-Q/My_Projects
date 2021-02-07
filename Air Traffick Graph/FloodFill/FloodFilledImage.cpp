#include "FloodFilledImage.h"


FloodFilledImage::FloodFilledImage(PNG& png) : image(png) { }

// Must store pointer so object still exists when animate is called
void FloodFilledImage::addFloodFill(ImageTraversal* traversal) {
    this->traversal.push_back(traversal);
}

// Create animation, list of frams taken at certain intervals
Animation FloodFilledImage::animate(unsigned frameInterval) {
    Animation animation;
    bool added = false;
    int i = 0;
    int length = traversal.size();
    // Loop through all traversals (routes)
    for(i = 0; i < length; i++){

        std::cout << "Traversal number " << i + 1 << "\n";

        ImageTraversal::Iterator iterator = traversal[i]->begin(); 
        ImageTraversal::Iterator end_iterator = traversal[i]->end();
        if(!added){
            animation.addFrame(image);
            added = true;
        }
        unsigned j = 0;
        // Power of the iterator
        // Simply increment until they are equal adding snapshots along the way
        while(iterator != end_iterator){
            ++iterator;
            j++;
            if((j % frameInterval) == 0){

                std::cout << "Frame number " << j << "\n";

                animation.addFrame(image);
            }
            if(j >= 10){
                break;
            }
        }
        // If 1 the most recent frame already got added
        if (!((j % frameInterval) == 1)){
            animation.addFrame(image);
        }

        delete traversal[i];
    }

    return animation;
}

PNG FloodFilledImage::get_image(){
    return this->image;
}