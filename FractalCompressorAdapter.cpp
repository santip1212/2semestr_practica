#include "FractalCompressorAdapter.h"
#include "ImageUtils.h"
#include <vector>
#include <cstring>

FractalCompressorAdapter::FractalCompressorAdapter(
    int rangeSize, int domainSize, int step
) : rangeBlockSize(rangeSize), domainBlockSize(domainSize), searchStep(step) {
    compressor.setBlockSize(rangeBlockSize, domainBlockSize);
    compressor.setSearchStep(searchStep);
}

std::vector<uint8_t> FractalCompressorAdapter::compress(
    const std::vector<uint8_t>& imageData,
    int width,
    int height,
    int channels
) {
    std::vector<uint8_t> compressedData = compressor.compress(imageData, width, height, channels);
    
    if (compressedData.empty()) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> decompressedData = compressor.decompress(compressedData);
    
    if (decompressedData.empty()) {
        return std::vector<uint8_t>();
    }
    
    return saveJPEGToMemory(decompressedData, width, height, 80);
}

std::vector<uint8_t> FractalCompressorAdapter::decompress(
    const std::vector<uint8_t>& compressedData,
    int& width,
    int& height,
    int& channels
) {
    std::vector<unsigned char> result = loadJPEGFromMemory(compressedData, width, height, channels);
    return result;
}