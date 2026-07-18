#include "dct.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstring>

// Вспомогательная функция для RLE сжатия
static std::vector<unsigned char> compressRLE(const std::vector<int>& data) {
    std::vector<unsigned char> compressed;
    int size = data.size();
    
    for (int i = 0; i < size; ) {
        int count = 1;
        int value = data[i];
        while (i + count < size && data[i + count] == value && count < 255) {
            count++;
        }
        // Сохраняем значение как 2 байта (т.к. оно может быть больше 255)
        compressed.push_back((value >> 8) & 0xFF);
        compressed.push_back(value & 0xFF);
        compressed.push_back((unsigned char)count);
        i += count;
    }
    return compressed;
}

// Вспомогательная функция для RLE распаковки
static std::vector<int> decompressRLE(const std::vector<unsigned char>& compressed) {
    std::vector<int> decompressed;
    
    for (size_t i = 0; i < compressed.size(); i += 3) {
        if (i + 2 < compressed.size()) {
            int value = (compressed[i] << 8) | compressed[i + 1];
            int count = compressed[i + 2];
            for (int j = 0; j < count; j++) {
                decompressed.push_back(value);
            }
        }
    }
    return decompressed;
}

// DCT коэффициенты (8x8 блок)
static const float DCT_COEFF[8][8] = {
    {0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536},
    {0.4904, 0.4157, 0.2778, 0.0975, -0.0975, -0.2778, -0.4157, -0.4904},
    {0.4619, 0.1913, -0.1913, -0.4619, -0.4619, -0.1913, 0.1913, 0.4619},
    {0.4157, -0.0975, -0.4904, -0.2778, 0.2778, 0.4904, 0.0975, -0.4157},
    {0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536, 0.3536},
    {0.2778, -0.4904, 0.0975, 0.4157, -0.4157, -0.0975, 0.4904, -0.2778},
    {0.1913, -0.4619, 0.4619, -0.1913, -0.1913, 0.4619, -0.4619, 0.1913},
    {0.0975, -0.2778, 0.4157, -0.4904, 0.4904, -0.4157, 0.2778, -0.0975}
};

// Применение DCT к блоку 8x8
static void dct8x8(const float block[8][8], float output[8][8]) {
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            float sum = 0.0f;
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    sum += block[x][y] * DCT_COEFF[u][x] * DCT_COEFF[v][y];
                }
            }
            // Масштабирование
            float cu = (u == 0) ? 1.0f / sqrt(2.0f) : 1.0f;
            float cv = (v == 0) ? 1.0f / sqrt(2.0f) : 1.0f;
            output[u][v] = sum * cu * cv / 4.0f;
        }
    }
}

// Обратное DCT
static void inverseDCT8x8(const float input[8][8], float output[8][8]) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            float sum = 0.0f;
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    float cu = (u == 0) ? 1.0f / sqrt(2.0f) : 1.0f;
                    float cv = (v == 0) ? 1.0f / sqrt(2.0f) : 1.0f;
                    sum += cu * cv * input[u][v] * DCT_COEFF[u][x] * DCT_COEFF[v][y];
                }
            }
            output[x][y] = sum / 4.0f;
        }
    }
}

// Таблица квантования для яркости (стандартная JPEG)
static const int QUANT_TABLE[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

// Применение DCT ко всему каналу
void applyDCT(const std::vector<unsigned char>& channel, 
              std::vector<float>& output, 
              int width, int height) {
    // Разбиваем на блоки 8x8
    int blocksX = (width + 7) / 8;
    int blocksY = (height + 7) / 8;
    output.resize(blocksX * blocksY * 64);
    
    float block[8][8];
    float dctBlock[8][8];
    
    for (int by = 0; by < blocksY; by++) {
        for (int bx = 0; bx < blocksX; bx++) {
            // Заполняем блок (с дополнением нулями если необходимо)
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    int px = bx * 8 + x;
                    int py = by * 8 + y;
                    if (px < width && py < height) {
                        block[y][x] = (float)channel[py * width + px] - 128.0f;
                    } else {
                        block[y][x] = 0.0f;
                    }
                }
            }
            
            // Применяем DCT
            dct8x8(block, dctBlock);
            
            // Сохраняем результат
            int idx = (by * blocksX + bx) * 64;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    output[idx + i * 8 + j] = dctBlock[i][j];
                }
            }
        }
    }
}

// Обратное DCT
void applyInverseDCT(const std::vector<float>& input, 
                     std::vector<unsigned char>& output, 
                     int width, int height) {
    int blocksX = (width + 7) / 8;
    int blocksY = (height + 7) / 8;
    output.resize(width * height);
    
    float block[8][8];
    float idctBlock[8][8];
    
    for (int by = 0; by < blocksY; by++) {
        for (int bx = 0; bx < blocksX; bx++) {
            // Извлекаем коэффициенты
            int idx = (by * blocksX + bx) * 64;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    block[i][j] = input[idx + i * 8 + j];
                }
            }
            
            // Применяем обратное DCT
            inverseDCT8x8(block, idctBlock);
            
            // Сохраняем результат
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    int px = bx * 8 + x;
                    int py = by * 8 + y;
                    if (px < width && py < height) {
                        float val = idctBlock[y][x] + 128.0f;
                        if (val < 0) val = 0;
                        if (val > 255) val = 255;
                        output[py * width + px] = (unsigned char)val;
                    }
                }
            }
        }
    }
}

// функция квантования
void quantize(const std::vector<float>& dctCoeffs, 
              std::vector<int>& quantized, 
              int quality, 
              int width, 
              int height) {
    int blocksX = (width + 7) / 8;
    int blocksY = (height + 7) / 8;
    quantized.resize(dctCoeffs.size());
    
    // ПРАВИЛЬНАЯ формула масштабирования
    float scale = 100.0f / quality - 0.99f;
    if (scale < 0.01f) scale = 0.01f;
    if (scale > 20.0f) scale = 20.0f; 
    
    // Защита от слишком малых значений
    if (scale < 0.1f) scale = 0.1f;
    if (scale > 100.0f) scale = 100.0f;
    
    std::cout << "Quality: " << quality << ", Scale: " << scale << std::endl;
    
    for (int by = 0; by < blocksY; by++) {
        for (int bx = 0; bx < blocksX; bx++) {
            int idx = (by * blocksX + bx) * 64;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    float quantVal = QUANT_TABLE[i][j] * scale;
                    if (quantVal < 1) quantVal = 1;
                    float val = dctCoeffs[idx + i * 8 + j] / quantVal;
                    quantized[idx + i * 8 + j] = (int)round(val);
                }
            }
        }
    }
}

// функция обратного квантования
void dequantize(const std::vector<int>& quantized, 
                std::vector<float>& dctCoeffs, 
                int quality, 
                int width, 
                int height) {
    int blocksX = (width + 7) / 8;
    int blocksY = (height + 7) / 8;
    dctCoeffs.resize(quantized.size());
    
    // Та же формула для согласованности
    float scale;
    if (quality < 50) {
        scale = 5000.0f / quality;
    } else {
        scale = 200.0f - quality * 2.0f;
    }
    scale /= 100.0f;
    
    if (scale < 0.1f) scale = 0.1f;
    if (scale > 100.0f) scale = 100.0f;
    
    for (int by = 0; by < blocksY; by++) {
        for (int bx = 0; bx < blocksX; bx++) {
            int idx = (by * blocksX + bx) * 64;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    float quantVal = QUANT_TABLE[i][j] * scale;
                    if (quantVal < 1) quantVal = 1;
                    dctCoeffs[idx + i * 8 + j] = quantized[idx + i * 8 + j] * quantVal;
                }
            }
        }
    }
}

// Сжатие всего изображения через DCT + RLE
void compressDCT(const std::string& filename,
                 const std::vector<unsigned char>& r,
                 const std::vector<unsigned char>& g,
                 const std::vector<unsigned char>& b,
                 int width, 
                 int height,
                 int quality) {
    std::vector<float> dctR, dctG, dctB;
    std::vector<int> quantR, quantG, quantB;
    
    // Применяем DCT к каждому каналу
    applyDCT(r, dctR, width, height);
    applyDCT(g, dctG, width, height);
    applyDCT(b, dctB, width, height);
    
    // Квантуем
    quantize(dctR, quantR, quality, width, height);
    quantize(dctG, quantG, quality, width, height);
    quantize(dctB, quantB, quality, width, height);
    
    // Сжимаем через RLE
    auto cr = compressRLE(quantR);
    auto cg = compressRLE(quantG);
    auto cb = compressRLE(quantB);
    
    // Сохраняем в файл
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error: failed to create file " << filename << std::endl;
        return;
    }
    
    // Сигнатура DCT
    unsigned int sig = 0x4454434D; 
    out.write((char*)&sig, 4);
    out.write((char*)&width, 4);
    out.write((char*)&height, 4);
    out.write((char*)&quality, 4);
    
    // Размеры сжатых данных
    int lens[3] = {(int)cr.size(), (int)cg.size(), (int)cb.size()};
    out.write((char*)lens, 12);
    
    // Данные
    out.write((char*)cr.data(), cr.size());
    out.write((char*)cg.data(), cg.size());
    out.write((char*)cb.data(), cb.size());
    
    out.close();
    std::cout << "DCT Compression complete. Quality: " << quality << "%" << std::endl;
}

// Распаковка DCT + RLE
void decompressDCT(const std::string& filename,
                   std::vector<unsigned char>& r,
                   std::vector<unsigned char>& g,
                   std::vector<unsigned char>& b,
                   int& width, 
                   int& height) {
    int quality;
    loadDCTCompressed(filename, r, g, b, width, height, quality);
}

// Сохранение сжатого файла DCT
void saveDCTCompressed(const std::string& filename,
                       const std::vector<unsigned char>& r,
                       const std::vector<unsigned char>& g,
                       const std::vector<unsigned char>& b,
                       int width, 
                       int height,
                       int quality) {
    compressDCT(filename, r, g, b, width, height, quality);
}

// Загрузка сжатого файла DCT
void loadDCTCompressed(const std::string& filename,
                       std::vector<unsigned char>& r,
                       std::vector<unsigned char>& g,
                       std::vector<unsigned char>& b,
                       int& width, 
                       int& height,
                       int& quality) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error: failed to open file " << filename << std::endl;
        return;
    }
    
    unsigned int sig;
    in.read((char*)&sig, 4);
    
    if (sig != 0x4454434D) {
        std::cerr << "Error: invalid file format (not DCT)" << std::endl;
        return;
    }
    
    in.read((char*)&width, 4);
    in.read((char*)&height, 4);
    in.read((char*)&quality, 4);
    
    int lens[3];
    in.read((char*)lens, 12);
    
    std::vector<unsigned char> cr(lens[0]), cg(lens[1]), cb(lens[2]);
    in.read((char*)cr.data(), lens[0]);
    in.read((char*)cg.data(), lens[1]);
    in.read((char*)cb.data(), lens[2]);
    
    // Распаковываем RLE
    auto quantR = decompressRLE(cr);
    auto quantG = decompressRLE(cg);
    auto quantB = decompressRLE(cb);
    
    // Обратное квантование
    std::vector<float> dctR, dctG, dctB;
    dequantize(quantR, dctR, quality, width, height);
    dequantize(quantG, dctG, quality, width, height);
    dequantize(quantB, dctB, quality, width, height);
    
    // Обратное DCT
    applyInverseDCT(dctR, r, width, height);
    applyInverseDCT(dctG, g, width, height);
    applyInverseDCT(dctB, b, width, height);
    
    std::cout << "DCT Unpacking complete. Quality: " << quality << "%" << std::endl;
}