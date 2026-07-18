#ifndef DCT_H
#define DCT_H

#include <vector>
#include <string>

// Структура для хранения сжатых данных
struct CompressedDCTData {
    std::vector<unsigned char> red;
    std::vector<unsigned char> green;
    std::vector<unsigned char> blue;
    int width;
    int height;
    int quality; // качество сжатия (1-100)
};

//Основные функции DCT
void applyDCT(const std::vector<unsigned char>& channel, 
              std::vector<float>& output, 
              int width, int height);

void applyInverseDCT(const std::vector<float>& input, 
                     std::vector<unsigned char>& output, 
                     int width, int height);

// Квантование и обратное квантование
void quantize(const std::vector<float>& dctCoeffs, 
              std::vector<int>& quantized, 
              int quality, 
              int width, 
              int height);

void dequantize(const std::vector<int>& quantized, 
                std::vector<float>& dctCoeffs, 
                int quality, 
                int width, 
                int height);

// Сжатие всего изображения через DCT + RLE
void compressDCT(const std::string& filename,
                 const std::vector<unsigned char>& r,
                 const std::vector<unsigned char>& g,
                 const std::vector<unsigned char>& b,
                 int width, 
                 int height,
                 int quality = 80);

// Распаковка DCT + RLE
void decompressDCT(const std::string& filename,
                   std::vector<unsigned char>& r,
                   std::vector<unsigned char>& g,
                   std::vector<unsigned char>& b,
                   int& width, 
                   int& height);

// Сохранение изображения
void saveDCTCompressed(const std::string& filename,
                       const std::vector<unsigned char>& r,
                       const std::vector<unsigned char>& g,
                       const std::vector<unsigned char>& b,
                       int width, 
                       int height,
                       int quality);

void loadDCTCompressed(const std::string& filename,
                       std::vector<unsigned char>& r,
                       std::vector<unsigned char>& g,
                       std::vector<unsigned char>& b,
                       int& width, 
                       int& height,
                       int& quality);

#endif // DCT_H