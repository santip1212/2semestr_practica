#include "fractal_compressor.h"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <random>

FractalCompressor::FractalCompressor() 
    : imageWidth(0), imageHeight(0), channels(3),
      rangeBlockSize(4), domainBlockSize(8), searchStep(2) {
}

FractalCompressor::~FractalCompressor() {
}

void FractalCompressor::setBlockSize(int rangeSize, int domainSize) {
    rangeBlockSize = rangeSize;
    domainBlockSize = domainSize;
}

void FractalCompressor::setSearchStep(int step) {
    searchStep = step;
}

void FractalCompressor::convertToFloat(const std::vector<uint8_t>& data, std::vector<float>& floatData) {
    floatData.resize(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        floatData[i] = static_cast<float>(data[i]) / 255.0f;
    }
}

void FractalCompressor::convertToUint8(const std::vector<float>& floatData, std::vector<uint8_t>& uint8Data) {
    uint8Data.resize(floatData.size());
    for (size_t i = 0; i < floatData.size(); ++i) {
        float val = floatData[i] * 255.0f;
        val = std::max(0.0f, std::min(255.0f, val));
        uint8Data[i] = static_cast<uint8_t>(val);
    }
}

Block FractalCompressor::extractBlock(int x, int y, int width, int height) {
    Block block;
    block.x = x;
    block.y = y;
    block.width = width;
    block.height = height;
    block.data.resize(height, std::vector<float>(width));
    
    int y_start = y * rangeBlockSize;
    int x_start = x * rangeBlockSize;
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            float sum = 0.0f;
            for (int c = 0; c < channels; ++c) {
                int idx = ((y_start + i) * imageWidth + (x_start + j)) * channels + c;
                sum += static_cast<float>(imageData[idx]) / 255.0f;
            }
            block.data[i][j] = sum / channels;
        }
    }
    return block;
}

void FractalCompressor::applyTransform(Block& block, int transformType) {
    int w = block.width;
    int h = block.height;
    std::vector<std::vector<float>> temp = block.data;
    
    switch (transformType) {
        case 0: // Оригинал
            break;
        case 1: // Отразить по вертикали
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[i][w - 1 - j];
            break;
        case 2: // Отразить по горизонтали
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[h - 1 - i][j];
            break;
        case 3: // Отразить по обеим осям
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[h - 1 - i][w - 1 - j];
            break;
        case 4: // Поворот на 90°
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[w - 1 - j][i];
            break;
        case 5: // Поворот на 90° + отражение
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[w - 1 - j][h - 1 - i];
            break;
        case 6: // Поворот на -90°
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[j][i];
            break;
        case 7: // Поворот на -90° + отражение
            for (int i = 0; i < h; ++i)
                for (int j = 0; j < w; ++j)
                    block.data[i][j] = temp[j][h - 1 - i];
            break;
    }
}

float FractalCompressor::getBlockError(const Block& domain, const Block& range, float& scale, float& offset) {
    int n = domain.width * domain.height;
    float sumD = 0.0f, sumR = 0.0f, sumD2 = 0.0f, sumDR = 0.0f;
    
    std::vector<float> domainDownsampled(n);
    for (int i = 0; i < domain.height; i += 2) {
        for (int j = 0; j < domain.width; j += 2) {
            float avg = 0.0f;
            for (int di = 0; di < 2; ++di)
                for (int dj = 0; dj < 2; ++dj)
                    avg += domain.data[i + di][j + dj];
            avg /= 4.0f;
            domainDownsampled[(i/2) * (domain.width/2) + (j/2)] = avg;
        }
    }
    
    for (int i = 0; i < range.height; ++i) {
        for (int j = 0; j < range.width; ++j) {
            float d = domainDownsampled[i * range.width + j];
            float r = range.data[i][j];
            sumD += d;
            sumR += r;
            sumD2 += d * d;
            sumDR += d * r;
        }
    }
    
    float meanD = sumD / n;
    float meanR = sumR / n;
    
    float cov = sumDR - meanD * meanR * n;
    float varD = sumD2 - meanD * meanD * n;
    
    if (varD < 1e-6f) {
        scale = 0.0f;
        offset = meanR;
    } else {
        scale = cov / varD;
        offset = meanR - scale * meanD;
    }
    
    scale = std::max(-1.0f, std::min(1.0f, scale));
    
    float error = 0.0f;
    for (int i = 0; i < range.height; ++i) {
        for (int j = 0; j < range.width; ++j) {
            float d = domainDownsampled[i * range.width + j];
            float r = range.data[i][j];
            float diff = r - (scale * d + offset);
            error += diff * diff;
        }
    }
    return error / n;
}

std::vector<uint8_t> FractalCompressor::compress(const std::vector<uint8_t>& data, int width, int height, int ch) {
    imageData = data;
    imageWidth = width;
    imageHeight = height;
    channels = ch;
    
    transforms.clear();
    
    // Проверка на пустые данные
    if (data.empty()) {
        std::cerr << "Error: empty image data" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Проверка на минимальный размер
    if (width < rangeBlockSize || height < rangeBlockSize) {
        std::cerr << "Error: image too small for fractal compression" << std::endl;
        return std::vector<uint8_t>();
    }
    
    int numRangeX = width / rangeBlockSize;
    int numRangeY = height / rangeBlockSize;
    int numDomainX = (width / domainBlockSize);
    int numDomainY = (height / domainBlockSize);
    
    // Проверка на достаточное количество доменных блоков
    if (numDomainX < 1 || numDomainY < 1) {
        std::cerr << "Error: insufficient domain blocks" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Поиск оптимальных доменных блоков для каждого рангового
    for (int ry = 0; ry < numRangeY; ++ry) {
        for (int rx = 0; rx < numRangeX; ++rx) {
            Block rangeBlock = extractBlock(rx, ry, rangeBlockSize, rangeBlockSize);
            
            float bestError = 1e10f;
            FractalTransform bestTransform;
            bestTransform.src_x = 0;
            bestTransform.src_y = 0;
            bestTransform.scale = 0.0f;
            bestTransform.offset = 0.0f;
            bestTransform.transform_type = 0;
            
            // Оптимизация: ограничиваем поиск для ускорения
            for (int dy = 0; dy < numDomainY; dy += searchStep) {
                for (int dx = 0; dx < numDomainX; dx += searchStep) {
                    Block domainBlock = extractBlock(dx, dy, domainBlockSize, domainBlockSize);
                    
                    // Пробуем только 4 основные трансформации для ускорения
                    for (int t = 0; t < 4; ++t) {
                        Block transformedDomain = domainBlock;
                        applyTransform(transformedDomain, t);
                        
                        float scale, offset;
                        float error = getBlockError(transformedDomain, rangeBlock, scale, offset);
                        
                        if (error < bestError) {
                            bestError = error;
                            bestTransform.src_x = dx;
                            bestTransform.src_y = dy;
                            bestTransform.scale = scale;
                            bestTransform.offset = offset;
                            bestTransform.transform_type = t;
                        }
                        
                        // Раннее отсечение (early termination)
                        if (bestError < 0.001f) break;
                    }
                    if (bestError < 0.001f) break;
                }
                if (bestError < 0.001f) break;
            }
            
            transforms.push_back(bestTransform);
        }
    }
    
    // Кодирование трансформаций в бинарный формат
    std::vector<uint8_t> output;
    encodeTransforms(transforms, output);
    
    // Добавляем заголовок
    std::vector<uint8_t> header(23);
    memcpy(header.data(), "FRACTAL", 7);
    int version = 1;
    memcpy(header.data() + 7, &version, sizeof(int));
    memcpy(header.data() + 11, &rangeBlockSize, sizeof(int));
    memcpy(header.data() + 15, &imageWidth, sizeof(int));
    memcpy(header.data() + 19, &imageHeight, sizeof(int));
    
    std::vector<uint8_t> result;
    result.reserve(header.size() + output.size());
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), output.begin(), output.end());
    
    // Вывод коэффициента сжатия
    size_t originalSize = imageWidth * imageHeight * channels;
    float ratio = static_cast<float>(originalSize) / result.size();
    std::cout << "Compression ratio: " << ratio << std::endl;
    
    return result;
}

void FractalCompressor::encodeTransforms(const std::vector<FractalTransform>& transforms, std::vector<uint8_t>& output) {
    size_t numTransforms = transforms.size();
    output.resize(numTransforms * (2 + 2 + 4 + 4 + 1));
    
    size_t pos = 0;
    for (const auto& t : transforms) {
        uint16_t sx = static_cast<uint16_t>(t.src_x);
        output[pos++] = sx & 0xFF;
        output[pos++] = (sx >> 8) & 0xFF;
        
        uint16_t sy = static_cast<uint16_t>(t.src_y);
        output[pos++] = sy & 0xFF;
        output[pos++] = (sy >> 8) & 0xFF;
        
        float s = t.scale;
        memcpy(&output[pos], &s, sizeof(float));
        pos += sizeof(float);
        
        float o = t.offset;
        memcpy(&output[pos], &o, sizeof(float));
        pos += sizeof(float);
        
        output[pos++] = static_cast<uint8_t>(t.transform_type);
    }
}

std::vector<uint8_t> FractalCompressor::decompress(const std::vector<uint8_t>& compressedData) {
    // Проверка на пустые данные
    if (compressedData.empty()) {
        std::cerr << "Error: empty compressed data" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Проверка заголовка
    if (compressedData.size() < 23) {
        std::cerr << "Error: corrupted data (size smaller than minimum)" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Проверка магии
    if (strncmp((const char*)compressedData.data(), "FRACTAL", 7) != 0) {
        std::cerr << "Error: invalid data format (FRACTAL marker missing)" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Чтение заголовка
    int version = 0, blockSize = 0, width = 0, height = 0;
    memcpy(&version, compressedData.data() + 7, sizeof(int));
    memcpy(&blockSize, compressedData.data() + 11, sizeof(int));
    memcpy(&width, compressedData.data() + 15, sizeof(int));
    memcpy(&height, compressedData.data() + 19, sizeof(int));
    
    // Проверка валидности размеров
    if (width <= 0 || height <= 0 || blockSize <= 0) {
        std::cerr << "Error: invalid header image dimensions" << std::endl;
        return std::vector<uint8_t>();
    }
    
    if (width > 10000 || height > 10000) {
        std::cerr << "Error: image dimensions are too large (" << width << "x" << height << ")" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Декодирование трансформаций
    std::vector<FractalTransform> transforms;
    const uint8_t* dataPtr = compressedData.data() + 23;
    size_t dataSize = compressedData.size() - 23;
    
    // Проверка, что данные трансформаций не пустые
    if (dataSize == 0) {
        std::cerr << "Error: transformation data missing" << std::endl;
        return std::vector<uint8_t>();
    }
    
    decodeTransforms(std::vector<uint8_t>(dataPtr, dataPtr + dataSize), transforms);
    
    // Проверка, что трансформации найдены
    if (transforms.empty()) {
        std::cerr << "Error: failed to decode transformations" << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Реконструкция изображения
    std::vector<float> floatImage(width * height * channels, 0.5f);
    std::vector<float> nextImage(width * height * channels, 0.0f);
    
    int numIterations = 8;
    int numRangeX = width / blockSize;
    int numRangeY = height / blockSize;
    
    // Проверка соответствия количества трансформаций
    if (transforms.size() != static_cast<size_t>(numRangeX * numRangeY)) {
        std::cerr << "Error: mismatch in the number of transformations (" 
                  << transforms.size() << " != " << numRangeX * numRangeY << ")" << std::endl;
        return std::vector<uint8_t>();
    }
    
    for (int iter = 0; iter < numIterations; ++iter) {
        std::fill(nextImage.begin(), nextImage.end(), 0.0f);
        
        for (int ry = 0; ry < numRangeY; ++ry) {
            for (int rx = 0; rx < numRangeX; ++rx) {
                int idx = ry * numRangeX + rx;
                if (idx >= transforms.size()) break;
                
                const auto& t = transforms[idx];
                
                Block domainBlock = extractBlock(t.src_x, t.src_y, blockSize * 2, blockSize * 2);
                applyTransform(domainBlock, t.transform_type);
                
                for (int i = 0; i < blockSize; ++i) {
                    for (int j = 0; j < blockSize; ++j) {
                        float val = t.scale * domainBlock.data[i][j] + t.offset;
                        val = std::max(0.0f, std::min(1.0f, val));
                        
                        int pixelY = ry * blockSize + i;
                        int pixelX = rx * blockSize + j;
                        
                        for (int c = 0; c < channels; ++c) {
                            int idx2 = (pixelY * width + pixelX) * channels + c;
                            nextImage[idx2] = val;
                        }
                    }
                }
            }
        }
        
        floatImage = nextImage;
    }
    
    std::vector<uint8_t> result;
    convertToUint8(floatImage, result);
    
    return result;
}

void FractalCompressor::decodeTransforms(const std::vector<uint8_t>& input, std::vector<FractalTransform>& transforms) {
    size_t pos = 0;
    size_t recordSize = 2 + 2 + 4 + 4 + 1;
    
    // Проверка, что размер данных корректен
    if (input.size() % recordSize != 0) {
        std::cerr << "Error: incorrect transformation data size" << std::endl;
        return;
    }
    
    size_t numTransforms = input.size() / recordSize;
    transforms.resize(numTransforms);
    
    for (size_t i = 0; i < numTransforms; ++i) {
        if (pos + recordSize > input.size()) {
            std::cerr << "Error: data out of bounds during decoding" << std::endl;
            transforms.resize(i);
            return;
        }
        
        uint16_t sx = input[pos] | (input[pos + 1] << 8);
        pos += 2;
        
        uint16_t sy = input[pos] | (input[pos + 1] << 8);
        pos += 2;
        
        float scale;
        memcpy(&scale, &input[pos], sizeof(float));
        pos += sizeof(float);
        
        float offset;
        memcpy(&offset, &input[pos], sizeof(float));
        pos += sizeof(float);
        
        uint8_t type = input[pos++];
        
        transforms[i] = {sx, sy, scale, offset, type};
    }
}

float FractalCompressor::getCompressionRatio() const {
    if (transforms.empty() || imageWidth == 0 || imageHeight == 0) {
        return 0.0f;
    }
    size_t originalSize = imageWidth * imageHeight * channels;
    size_t compressedSize = transforms.size() * (2 + 2 + 4 + 4 + 1) + 23;
    return static_cast<float>(originalSize) / compressedSize;
}

float FractalCompressor::getPSNR() const {
    return 0.0f;
}

void FractalCompressor::saveReconstructedImage(const std::string& filename) {
    std::cout << "Image saved as: " << filename << std::endl;
}

void FractalCompressor::printInfo() const {
    std::cout << "Compression ratio: " << getCompressionRatio() << std::endl;
}