#ifndef FRACTALCOMPRESSORADAPTER_H
#define FRACTALCOMPRESSORADAPTER_H

#include "ICompressor.h"
#include "fractal_compressor.h"
#include <vector>
#include <string>

class FractalCompressorAdapter : public ICompressor {
public:
    FractalCompressorAdapter(int rangeBlockSize = 8, int domainBlockSize = 16, int searchStep = 4);
    
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
        return "Fractal";
    }
    
private:
    int rangeBlockSize;
    int domainBlockSize;
    int searchStep;
    FractalCompressor compressor;
};

#endif // FRACTALCOMPRESSORADAPTER_H