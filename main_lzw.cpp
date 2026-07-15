#include "stb_image_write.h"
#include "stb_image.h"
#include "lzw.h"
#include <iostream>
#include <vector>
#include <fstream>

int main() {
    int width, height, channels;
    unsigned char* data = stbi_load("leto.bmp", &width, &height, &channels, 3);
    
    if (data == nullptr) {
        std::cout << "error" << std::endl;
        return 1;
    }
    
    std::vector<unsigned char> redChannel(width * height);
    std::vector<unsigned char> greenChannel(width * height);
    std::vector<unsigned char> blueChannel(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            redChannel[y * width + x] = data[idx];
            greenChannel[y * width + x] = data[idx + 1];
            blueChannel[y * width + x] = data[idx + 2];
        }
    }
    
    stbi_image_free(data);
    
    // Сжатие и сохранение в LZW формат
    saveCompressedLZW("leto_compressed.lzw", redChannel, greenChannel, blueChannel, width, height);
    
    return 0;
}