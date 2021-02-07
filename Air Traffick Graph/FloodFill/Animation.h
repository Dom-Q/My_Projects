#pragma once

#include <string>
#include <vector>
#include "../PNG/PNG.h"

using namespace std;

// Class that creates animated images from a sequence of PNG object frames.
class Animation
{
  public:

    void addFrame(const PNG& img);
    void write(const std::string& filename);
    PNG getFrame(unsigned index);
    unsigned frameCount();


  private:
    std::vector<PNG> frames;

    template <typename T>
    string to_string(const T& value);
    string getString(int i, int padToSameLengthAs);
    bool exists(const string& path);
};
