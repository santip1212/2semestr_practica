#ifndef DCTCOMPRESSOR_H
#define DCTCOMPRESSOR_H

#include "ICompressor.h"
#include <vector>
#include <string>

class DCTCompressor : public ICompressor {
public:
    DCTCompressor(int quality = 80);
    
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
        return "DCT";
    }
    
    void setQuality(int q) { quality = q; }
    
private:
    int quality;
};

#endif // DCTCOMPRESSOR_H