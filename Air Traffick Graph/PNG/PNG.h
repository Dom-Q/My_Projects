#pragma once

#include <string>
using std::string;

#include "HSLAPixel.h"

class PNG {
    public:

    // Creates an empty PNG image.
    PNG();

    // Creates a PNG image of the specified dimensions.
    PNG(unsigned int width, unsigned int height);

    // Copy constructor
    PNG(PNG const & other);

    // Destructor
    ~PNG();
    PNG const & operator= (PNG const & other);
    bool operator== (PNG const & other) const;
    bool operator!= (PNG const & other) const;


    // Return width of the image
    unsigned int width() const;

    // Return height of the image.
    unsigned int height() const;

    // Resizes the image to the given coordinates. Attempts to preserve
    // existing pixel data in the image when doing so, but will crop if
    // necessary.*/
    void resize(unsigned int newWidth, unsigned int newHeight);

    // Map latitude/longitude values to pixels on given image
    double get_x_pixel(double longitude, double lat);
    double get_y_pixel(double latitude, double lon);

    //---------------------------------//
    /* @author CS 225: Data Structures */

    //Reads in a PNG image from a file.
    bool readFromFile(string const & fileName);

    /**
         * Writes a PNG image to a file.
         * @param fileName Name of the file to be written.
         * @return true, if the image was successfully written.
         */
    bool writeToFile(string const & fileName);

    /**
         * Pixel access operator. Gets a reference to the pixel at the given
         * coordinates in the image. (0,0) is the upper left corner.
         * This reference allows the image to be changed.
         * @param x X-coordinate for the pixel reference to be grabbed from.
         * @param y Y-coordinate for the pixel reference to be grabbed from.
         * @return A reference to the pixel at the given coordinates.
         */
    HSLAPixel & getPixel(unsigned int x, unsigned int y);

    /**
         * Pixel access operator. Gets a const reference to the pixel at the
         * given coordinates in the image. (0,0) is the upper left corner.
         * This const reference DOES NOT allow the image to be changed.
         * @param x X-coordinate for the const pixel reference to be grabbed from.
         * @param y Y-coordinate for the const pixel reference to be grabbed from.
         * @return A const reference to the pixel at the given coordinates.
         */
    const HSLAPixel & getPixel(unsigned int x, unsigned int y) const;

    private:
    unsigned int width_;            /*< Width of the image */
    unsigned int height_;           /*< Height of the image */
    HSLAPixel *imageData_;          /*< Array of pixels */
    double x_scale;
    double y_scale;

    void _copy(PNG const & other);

    /**
     * Common function for powering the following signature stubs.
     * HSLAPixel & getPixel(unsigned int x, unsigned int y);
     * const HSLAPixel & getPixel(unsigned int x, unsigned int y) const;
     */
    HSLAPixel & _getPixelHelper(unsigned int x, unsigned int y) const;
};

std::ostream & operator<<(std::ostream & out, PNG const & pixel);
std::stringstream & operator<<(std::stringstream & out, PNG const & pixel);

