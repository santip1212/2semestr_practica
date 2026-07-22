#ifndef RLECOMPRESSOR_H
#define RLECOMPRESSOR_H

#include "ICompressor.h"
#include <vector>
#include <string>

class RLECompressor : public ICompressor {
public:
    std::vector<uint8_t> compress(
        const std::vector<uint8_t>& imageData,
        int width,
        int height,
        int channels
    ) override;
    
    std::vector<uint8_t> decompress(
        const std::vector<uint8_t>& compressedData,
        int& width,
        int& height,
        int& channels
    ) override;
    
    std::string getName() const override {
        return "RLE";
    }
    
private:
    std::vector<unsigned char> compressChannel(const std::vector<unsigned char>& channel);
    std::vector<unsigned char> decompressChannel(const std::vector<unsigned char>& compressed);
};

#endif // RLECOMPRESSOR_H