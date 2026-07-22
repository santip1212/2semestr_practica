#ifndef FRACTAL_COMPRESSOR_H
#define FRACTAL_COMPRESSOR_H

#include <vector>
#include <cstdint>
#include <string>

struct RGB {
    uint8_t r, g, b;
};

struct Block {
    int x, y;           // позиция блока
    int width, height;  // размер блока
    std::vector<std::vector<float>> data; // данные яркости
};

struct FractalTransform {
    int src_x, src_y;   // позиция доменного блока
    float scale;        // масштабирование яркости
    float offset;       // сдвиг яркости
    int transform_type; // тип трансформации (0-7)
};

class FractalCompressor {
private:
    int imageWidth, imageHeight;
    int channels;
    std::vector<uint8_t> imageData;
    
    // Параметры сжатия
    int rangeBlockSize;     // размер рангового блока 
    int domainBlockSize;    // размер доменного блока 
    int searchStep;         // шаг поиска для ускорения
    
    // Промежуточные данные
    std::vector<FractalTransform> transforms;
    std::vector<float> reconstructedData;
    
    // Вспомогательные методы
    float getBlockError(const Block& domain, const Block& range, float& scale, float& offset);
    void applyTransform(Block& block, int transformType);
    Block extractBlock(int x, int y, int width, int height);
    float computeRMSE(const std::vector<uint8_t>& original, const std::vector<float>& reconstructed);
    void convertToFloat(const std::vector<uint8_t>& data, std::vector<float>& floatData);
    void convertToUint8(const std::vector<float>& floatData, std::vector<uint8_t>& uint8Data);
    
    // Кодирование/декодирование трансформаций
    void encodeTransforms(const std::vector<FractalTransform>& transforms, std::vector<uint8_t>& output);
    void decodeTransforms(const std::vector<uint8_t>& input, std::vector<FractalTransform>& transforms);
    
public:
    FractalCompressor();
    ~FractalCompressor();
    
    // Основные методы сжатия/распаковки
    std::vector<uint8_t> compress(const std::vector<uint8_t>& imageData, int width, int height, int channels);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressedData);
    
    // Получение параметров
    void setBlockSize(int rangeSize, int domainSize);
    void setSearchStep(int step);
    
    // Статистика
    float getCompressionRatio() const;
    float getPSNR() const;
    
    // Вспомогательные методы для тестирования
    void saveReconstructedImage(const std::string& filename);
    void printInfo() const;
};

#endif // FRACTAL_COMPRESSOR_H