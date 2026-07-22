#include "stb_image.h"
#include "stb_image_write.h"
#include "dct.h"
#include <iostream>
#include <vector>
#include <fstream>

int main_l() {
    int width, height, channels;
    unsigned char* data = stbi_load("leto.bmp", &width, &height, &channels, 3);
    
    if (data == nullptr) {
        std::cout << "error leto.bmp" << std::endl;
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
    
    // Параметры качества (1-100)
    int quality = 90;
    
    // Сжатие с DCT + RLE
    std::string compressedFile = "leto_dct_compressed.dct";
    saveDCTCompressed(compressedFile, redChannel, greenChannel, blueChannel, width, height, quality);
    
    // Информация о размерах
    size_t originalSize = width * height * 3;
    std::ifstream compressedFileStream(compressedFile, std::ios::binary | std::ios::ate);
    size_t compressedSize = compressedFileStream.tellg();
    compressedFileStream.close();
    
    if (compressedSize > 0) {
        double ratio = (double)compressedSize / originalSize * 100;
        double compression = (double)originalSize / compressedSize;
        std::cout << "Compression ratio: " << ratio << "%" << std::endl;
        std::cout << "Compression ratio: "<< compression << "x" << std::endl;
    }
    
    // Распаковка
    std::vector<unsigned char> r_decomp, g_decomp, b_decomp;
    int newWidth, newHeight;
    int loadedQuality;
    
    loadDCTCompressed(compressedFile, r_decomp, g_decomp, b_decomp, newWidth, newHeight, loadedQuality);
    
    if (!r_decomp.empty() && !g_decomp.empty() && !b_decomp.empty()) {
        // Сохраняем распакованное изображение
        std::vector<unsigned char> rgb;
        rgb.reserve(newWidth * newHeight * 3);
        
        for (int i = 0; i < newWidth * newHeight; ++i) {
            rgb.push_back(r_decomp[i]);
            rgb.push_back(g_decomp[i]);
            rgb.push_back(b_decomp[i]);
        }
        
        std::string outputFile = "leto_dct_decompressed.bmp";
        stbi_write_bmp(outputFile.c_str(), newWidth, newHeight, 3, rgb.data());
        std::cout << "Size: " << newWidth << "x" << newHeight << std::endl;
        std::cout << "Quality: " << loadedQuality << "%" << std::endl;
    } else {
        std::cout << "Error during extraction!" << std::endl;
        return 1;
    }
    
    return 0;
}