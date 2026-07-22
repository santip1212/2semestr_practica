#include "SaveBMP.h"
#include "stb_image_write.h"
#include <vector>

void saveBMP(const std::string& filename,
             const std::vector<unsigned char>& r,
             const std::vector<unsigned char>& g,
             const std::vector<unsigned char>& b,
             int width, int height) {
    
    std::vector<unsigned char> rgb;
    rgb.reserve(width * height * 3);
    
    for (int i = 0; i < width * height; ++i) {
        rgb.push_back(r[i]);
        rgb.push_back(g[i]); 
        rgb.push_back(b[i]);
    }  
    
    stbi_write_bmp(filename.c_str(), width, height, 3, rgb.data());
}