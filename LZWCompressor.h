#ifndef LZWCOMPRESSOR_H
#define LZWCOMPRESSOR_H

#include "ICompressor.h"
#include <vector>
#include <string>

class LZWCompressor : public ICompressor {
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
        return "LZW";
    }
    
private:
    std::vector<unsigned char> compressChannelLZW(const std::vector<unsigned char>& channel);
    std::vector<unsigned char> decompressChannelLZW(const std::vector<unsigned char>& compressed);
};

#endif // LZWCOMPRESSOR_H