#include <iostream>
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <cassert>
#include <algorithm>
#include <functional>

#include "lodepng/lodepng.h"
#include "PNG.h"
#include "RGB_HSL.h"


// A bunch of conditions because of the map's extreme and irregular distortion
double PNG::get_x_pixel(double longitude, double lat){

    double pix = ((180 + longitude) * x_scale);
    double shift = 69;
    
    if(longitude < -120){
        shift -= 10;
    }
    if(longitude < -50 && lat > 50){
        shift -= 80;
        shift += ((180 + longitude)/180) * 45;
    }
    if(longitude > 15 && lat > 30){
        shift += (((longitude - 10) / 180) * 50) + 5;
    }
    if(lat < 30 && lat > 0 && longitude < 0){
        shift += (12 + ( ((35 - lat) / 35) * 10 ) );
    }
    if(lat < 0 && lat > - 45 && longitude < 0){
        shift += ( ( (45 + lat) / 45) * 23);
    }
    if(lat < -35 && longitude < 0){
        shift -= (4 + ( (((lat*-1) - 35) / 35) * 38) );
    }
    if(longitude < -5 && longitude > -15 && lat > 0 && lat < 30){
        shift -= 10;
    }
    if(longitude > 55 && lat > 0 && lat < 44){
        shift -= 5;
    }
    if(lat > 40 && lat < 50 && longitude < -25){
        shift -= (6 + ( (((longitude*-1) - 80) / 40) * 15));
    }
    if(lat > 0 && longitude > 0 && lat < 10){
        shift += 2;
    }
    if(longitude > 75 && lat > 0 && lat < 10){
        shift -= (5 + (((longitude < 75 ? 0:longitude - 85) / 40) * 14));
    }
    if(lat < 0 && longitude > 0){
        shift += 4;
    }
    if(longitude > 70 && lat < -35){
        shift += (1 + (((( ((lat*-1) - 33) / 15) * 40))));
    }
    if(lat < 0 && longitude > 75 && lat > -30){
        shift -= (1 + (((30 + lat) / 20) * 25));
        if(lat > -5){
            shift += 10;
            if(longitude > 125){
                shift -= (3 + (((longitude - 125) / 25) * 6));
            }
            else{
                shift += (2 + (((125 - longitude) / 25) * 8));
            }
        }
    }
    if(lat > 50 && lat < 60 && longitude < -30){
        shift += 18;
        if(longitude > -75){
            shift += 10;
        }
    }
    if(lat > 40 && longitude < -50 && longitude > -65){
        shift += 5;
    }
    if(lat > 43 && longitude < -65 && longitude > -80){
        shift -= 14;
    }
    pix -= shift;
    return pix;
}

double PNG::get_y_pixel(double latitude, double lon){

    double pix = (90 - latitude) * y_scale;

    if(latitude < 45 && latitude > 0){
        pix += 8;
        pix += ((45 - latitude) / 45) * 20;
        if(latitude < 30 && lon < 0){
            pix += ( ( (30 - latitude) / 30 ) * 21 ) + 3;
        }
    }
    if(latitude < 0 && latitude > -15 && lon < 0){
        pix -= ( ((14 + latitude ) / 16) * 18);
    }
    if(latitude < 0){
        pix += 68;
        if(latitude < -20 && lon < 0){
            pix += (4 + ((((latitude + 20)*-1) / 30) * 32));
        }
    }
    if(latitude > 60){
        pix -= 7;
    }
    if(latitude > 0 && latitude < 26 && lon > 0){
        pix += ( 7 +  (((25 - latitude) / 25) * 10) );
    }
    if(latitude > 0 && lon > 0 && latitude < 10){
        pix += 4;
    }
    if(latitude < 0 && latitude > -15 && lon > 0){
        pix -= 6;
    }
    if(lon > 0 && lon < 50 && latitude < -15){
        pix += ( (((latitude*-1)-15)/15) *10 );
    }
    if(latitude < -15 && lon > 70){
        pix += (5 + ((((latitude > -25 ? 0:(latitude*-1 - 25)) / 25) * 20) ));
        if(latitude < -30){
            pix += (5 + (( ((latitude*-1) - 30) / 15) * 6) );
        }
    }
    if(latitude < 0 && latitude > -5 && lon > 75){
        pix -= 10;
    }
    return pix;
}

void PNG::_copy(PNG const & other) {
    // Clear self
    delete[] imageData_;

    // Copy `other` to self
    width_ = other.width_;
    height_ = other.height_;
    x_scale = other.x_scale;
    y_scale = other.y_scale;
    imageData_ = new HSLAPixel[width_ * height_];
    for (unsigned i = 0; i < width_ * height_; i++) {
        imageData_[i] = other.imageData_[i];
    }
}

PNG::PNG() {
    x_scale = 0;
    y_scale = 0;
    width_ = 0;
    height_ = 0;
    imageData_ = NULL;
}

PNG::PNG(unsigned int width, unsigned int height) {
    width_ = width;
    height_ = height;
    x_scale = width / (double)360;
    y_scale = height / (double)180;
    imageData_ = new HSLAPixel[width * height];
}

PNG::PNG(PNG const & other) {
    imageData_ = NULL;
    _copy(other);
}

PNG::~PNG() {
    delete[] imageData_;
}

PNG const & PNG::operator=(PNG const & other) {
    if (this != &other) { _copy(other); }
    return *this;
}

bool PNG::operator== (PNG const & other) const {
    if (width_ != other.width_) { return false; }
    if (height_ != other.height_) { return false; }

    hslaColor hslaSpace1;
    hslaColor hslaSpace2;
    rgbaColor rgbaSpace1;
    rgbaColor rgbaSpace2;
    for (unsigned i = 0; i < width_ * height_; i++) {
        HSLAPixel & p1 = imageData_[i];
        HSLAPixel & p2 = other.imageData_[i];
        hslaSpace1 = {p1.h, p1.s, p1.l, p1.a};
        hslaSpace2 = {p2.h, p2.s, p2.l, p2.a};
        rgbaSpace1 = hsl2rgb(hslaSpace1);
        rgbaSpace2 = hsl2rgb(hslaSpace2);
        if (rgbaSpace1.r != rgbaSpace2.r
            || rgbaSpace1.g != rgbaSpace2.g
            || rgbaSpace1.b != rgbaSpace2.b
            || rgbaSpace1.a != rgbaSpace2.a) { return false; }
    }

    return true;
}

bool PNG::operator!= (PNG const & other) const {
    return !(*this == other);
}

HSLAPixel & PNG::_getPixelHelper(unsigned int x, unsigned int y) const {
    if (width_ == 0 || height_ == 0) {
        cerr << "ERROR: Call to cs225::PNG::getPixel() made on an image with no pixels." << endl;
        assert(width_ > 0);
        assert(height_ > 0);
    }

    if (x >= width_) {
        cerr << "WARNING: Call to cs225::PNG::getPixel(" << x << "," << y << ") tries to access x=" << x
            << ", which is outside of the image (image width: " << width_ << ")." << endl;
        cerr << "       : Truncating x to " << (width_ - 1) << endl;
        x = width_ - 1;
    }

    if (y >= height_) {
        cerr << "WARNING: Call to cs225::PNG::getPixel(" << x << "," << y << ") tries to access y=" << y
            << ", which is outside of the image (image height: " << height_ << ")." << endl;
        cerr << "       : Truncating y to " << (height_ - 1) << endl;
        y = height_ - 1;
    }

    unsigned index = x + (y * width_);
    return imageData_[index];
}

HSLAPixel & PNG::getPixel(unsigned int x, unsigned int y) { return _getPixelHelper(x,y); }

const HSLAPixel & PNG::getPixel(unsigned int x, unsigned int y) const { return _getPixelHelper(x,y); }

bool PNG::readFromFile(string const & fileName) {
    vector<unsigned char> byteData;
    unsigned error = lodepng::decode(byteData, width_, height_, fileName);

    if (error) {
        cerr << "PNG decoder error " << error << ": " << lodepng_error_text(error) << endl;
        return false;
    }

    delete[] imageData_;
    imageData_ = new HSLAPixel[width_ * height_];

    for (unsigned i = 0; i < byteData.size(); i += 4) {
        rgbaColor rgb;
        rgb.r = byteData[i];
        rgb.g = byteData[i + 1];
        rgb.b = byteData[i + 2];
        rgb.a = byteData[i + 3];

        hslaColor hsl = rgb2hsl(rgb);
        HSLAPixel & pixel = imageData_[i/4];
        pixel.h = hsl.h;
        pixel.s = hsl.s;
        pixel.l = hsl.l;
        pixel.a = hsl.a;
    }

    return true;
}

bool PNG::writeToFile(string const & fileName) {
    unsigned char *byteData = new unsigned char[width_ * height_ * 4];

    for (unsigned i = 0; i < width_ * height_; i++) {
        hslaColor hsl;
        hsl.h = imageData_[i].h;
        hsl.s = imageData_[i].s;
        hsl.l = imageData_[i].l;
        hsl.a = imageData_[i].a;

        rgbaColor rgb = hsl2rgb(hsl);

        byteData[(i * 4)]     = rgb.r;
        byteData[(i * 4) + 1] = rgb.g;
        byteData[(i * 4) + 2] = rgb.b;
        byteData[(i * 4) + 3] = rgb.a;
    }

    unsigned error = lodepng::encode(fileName, byteData, width_, height_);
    if (error) {
        cerr << "PNG encoding error " << error << ": " << lodepng_error_text(error) << endl;
    }

    delete[] byteData;
    return (error == 0);
}

unsigned int PNG::width() const {
    return width_;
}

unsigned int PNG::height() const {
    return height_;
}

void PNG::resize(unsigned int newWidth, unsigned int newHeight) {
    HSLAPixel* newImageData = new HSLAPixel[newWidth * newHeight];

    for (unsigned x = 0; x < newWidth; x++) {
        for (unsigned y = 0; y < newHeight; y++) {
            if (x < width_ && y < height_) {
                HSLAPixel & oldPixel = this->getPixel(x, y);
                HSLAPixel & newPixel = newImageData[ (x + (y * newWidth)) ];
                newPixel = oldPixel;
            }
        }
    }

    delete[] imageData_;

    width_ = newWidth;
    height_ = newHeight;
    imageData_ = newImageData;
}

std::ostream & operator << ( std::ostream& os, PNG const& png ) {
    std::hash<double> hashFunction;
    std::size_t hash = 0;

    for (unsigned x = 0; x < png.width(); x++) {
        for (unsigned y = 0; y < png.height(); y++) {
            const HSLAPixel &pixel = png.getPixel(x, y);
            hash ^= hashFunction(pixel.h);
            hash ^= hashFunction(pixel.s);
            hash ^= hashFunction(pixel.l);
            hash ^= hashFunction(pixel.a);
        }
    }

    os << "PNG(w=" << png.width() << ", h=" << png.height() << ", hash=" << std::hex << hash << std::dec << ")";
    return os;
}
