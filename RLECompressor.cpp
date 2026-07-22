#include "RLECompressor.h"
#include "ImageUtils.h"
#include <vector>
#include <cstring>
#include <iostream>

std::vector<unsigned char> RLECompressor::compressChannel(const std::vector<unsigned char>& channel) {
    std::vector<unsigned char> compressed;
    int size = static_cast<int>(channel.size());
    for (int i = 0; i < size; ) {
        int count = 1;
        unsigned char value = channel[i];
        while (i + count < size && channel[i + count] == value && count < 255) {
            count++;
        }
        compressed.push_back(static_cast<unsigned char>(count));
        compressed.push_back(value);
        i += count;
    }
    return compressed;
}

std::vector<unsigned char> RLECompressor::decompressChannel(const std::vector<unsigned char>& compressed) {
    std::vector<unsigned char> decompressed;
    for (size_t i = 0; i < compressed.size(); i += 2) {
        if (i + 1 < compressed.size()) {
            int count = static_cast<int>(compressed[i]);
            unsigned char value = compressed[i + 1];
            for (int j = 0; j < count; j++) {
                decompressed.push_back(value);
            }
        }
    }
    return decompressed;
}

std::vector<uint8_t> RLECompressor::compress(
    const std::vector<uint8_t>& imageData,
    int width,
    int height,
    int channels
) {
    int pixelCount = width * height;
    std::vector<unsigned char> r(pixelCount), g(pixelCount), b(pixelCount);
    
    for (int i = 0; i < pixelCount; ++i) {
        r[i] = imageData[i * channels];
        g[i] = imageData[i * channels + 1];
        b[i] = imageData[i * channels + 2];
    }
    
    auto cr = compressChannel(r);
    auto cg = compressChannel(g);
    auto cb = compressChannel(b);
    
    auto dr = decompressChannel(cr);
    auto dg = decompressChannel(cg);
    auto db = decompressChannel(cb);
    
    std::vector<unsigned char> rgb(pixelCount * 3);
    for (int i = 0; i < pixelCount; ++i) {
        rgb[i * 3] = dr[i];
        rgb[i * 3 + 1] = dg[i];
        rgb[i * 3 + 2] = db[i];
    }
    
    return saveJPEGToMemory(rgb, width, height, 80);
}

std::vector<uint8_t> RLECompressor::decompress(
    const std::vector<uint8_t>& compressedData,
    int& width,
    int& height,
    int& channels
) {
    return loadImageFromMemory(compressedData, width, height, channels);
}