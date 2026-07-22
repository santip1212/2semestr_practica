#ifndef HUFFMANCOMPRESSOR_H
#define HUFFMANCOMPRESSOR_H

#include "ICompressor.h"
#include <vector>
#include <string>

class HuffmanCompressor : public ICompressor {
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
        return "Huffman";
    }
};

#endif // HUFFMANCOMPRESSOR_H