#include "LZWCompressor.h"
#include "ImageUtils.h"
#include <unordered_map>
#include <string>
#include <cstring>

std::vector<unsigned char> LZWCompressor::compressChannelLZW(const std::vector<unsigned char>& channel) {
    std::vector<unsigned char> compressed;
    
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<unsigned char>(i))] = i;
    }
    
    int dictSize = 256;
    std::string current;
    std::vector<int> outputCodes;
    
    for (unsigned char c : channel) {
        std::string next = current + static_cast<char>(c);
        if (dictionary.find(next) != dictionary.end()) {
            current = next;
        } else {
            outputCodes.push_back(dictionary[current]);
            dictionary[next] = dictSize++;
            current = std::string(1, static_cast<char>(c));
        }
    }
    
    if (!current.empty()) {
        outputCodes.push_back(dictionary[current]);
    }
    
    for (int code : outputCodes) {
        compressed.push_back((code >> 8) & 0xFF);
        compressed.push_back(code & 0xFF);
    }
    
    return compressed;
}

std::vector<unsigned char> LZWCompressor::decompressChannelLZW(const std::vector<unsigned char>& compressed) {
    std::vector<unsigned char> decompressed;
    if (compressed.size() < 2) return decompressed;
    
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<unsigned char>(i));
    }
    
    int dictSize = 256;
    std::vector<int> codes;
    for (size_t i = 0; i < compressed.size(); i += 2) {
        if (i + 1 < compressed.size()) {
            int code = (compressed[i] << 8) | compressed[i + 1];
            codes.push_back(code);
        }
    }
    
    if (codes.empty()) return decompressed;
    
    std::string current = dictionary[codes[0]];
    for (unsigned char c : current) {
        decompressed.push_back(c);
    }
    
    for (size_t i = 1; i < codes.size(); ++i) {
        int code = codes[i];
        std::string entry;
        
        if (dictionary.find(code) != dictionary.end()) {
            entry = dictionary[code];
        } else if (code == dictSize) {
            entry = current + current[0];
        } else {
            break;
        }
        
        for (unsigned char c : entry) {
            decompressed.push_back(c);
        }
        
        dictionary[dictSize++] = current + entry[0];
        current = entry;
    }
    
    return decompressed;
}

std::vector<uint8_t> LZWCompressor::compress(
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
    
    auto cr = compressChannelLZW(r);
    auto cg = compressChannelLZW(g);
    auto cb = compressChannelLZW(b);
    
    auto dr = decompressChannelLZW(cr);
    auto dg = decompressChannelLZW(cg);
    auto db = decompressChannelLZW(cb);
    
    std::vector<unsigned char> rgb(pixelCount * 3);
    for (int i = 0; i < pixelCount; ++i) {
        rgb[i * 3] = dr[i];
        rgb[i * 3 + 1] = dg[i];
        rgb[i * 3 + 2] = db[i];
    }
    
    return saveJPEGToMemory(rgb, width, height, 80);
}

std::vector<uint8_t> LZWCompressor::decompress(
    const std::vector<uint8_t>& compressedData,
    int& width,
    int& height,
    int& channels
) {
    std::vector<unsigned char> result = loadJPEGFromMemory(compressedData, width, height, channels);
    return result;
}