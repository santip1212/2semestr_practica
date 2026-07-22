#include "fractal_compressor.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

int main_k() {
    std::string inputFile = "dad.bmp";
    
    std::string outputFile = "compressed.fractal";
    
    // Загрузка изображения
    int width, height, channels;
    unsigned char* image = stbi_load(inputFile.c_str(), &width, &height, &channels, 0);
    
    if (!image) {
        std::cerr << "Error: failed to load image " << inputFile << std::endl;
        return 1;
    }
    
    // Преобразование в вектор
    size_t dataSize = static_cast<size_t>(width) * height * channels;
    std::vector<uint8_t> imageData(image, image + dataSize);
    
    // Создание компрессора
    FractalCompressor compressor;
    
    // Настройка параметров
    compressor.setBlockSize(8, 16);   // 4x4 и 8x8 блоки
    compressor.setSearchStep(4);     // шаг поиска 2
    
    // Сжатие
    std::vector<uint8_t> compressedData = compressor.compress(imageData, width, height, channels);
    
    if (compressedData.empty()) {
        std::cerr << "Error: compression failed" << std::endl;
        stbi_image_free(image);
        return 1;
    }
    
    // Сохранение сжатых данных
    FILE* file = fopen(outputFile.c_str(), "wb");
    if (file) {
        fwrite(compressedData.data(), 1, compressedData.size(), file);
        fclose(file);
    }
    
    // Декомпрессия
    std::vector<uint8_t> decompressedData = compressor.decompress(compressedData);
    
    if (decompressedData.empty()) {
        std::cerr << "Error: decompression failed" << std::endl;
        stbi_image_free(image);
        return 1;
    }
    
    // Сохранение декомпрессированного изображения
    std::string outFile = "reconstructed_" + inputFile;
    size_t dotPos = outFile.find_last_of(".");
    if (dotPos != std::string::npos) {
        outFile = outFile.substr(0, dotPos) + ".png";
    } else {
        outFile = outFile + ".png";
    }
    
    stbi_write_png(outFile.c_str(), width, height, channels, decompressedData.data(), width * channels);
    std::cout << compressor.getCompressionRatio() << std::endl;
    
    // Освобождение памяти
    stbi_image_free(image);
    return 0;
}