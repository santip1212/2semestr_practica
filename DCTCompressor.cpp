#include "DCTCompressor.h"
#include "dct.h"
#include "ImageUtils.h"
#include <vector>
#include <cstring>
#include <iostream>

DCTCompressor::DCTCompressor(int quality) : quality(quality) {}

std::vector<uint8_t> DCTCompressor::compress(
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
    
    std::vector<float> dctR, dctG, dctB;
    applyDCT(r, dctR, width, height);
    applyDCT(g, dctG, width, height);
    applyDCT(b, dctB, width, height);
    
    std::vector<int> quantR, quantG, quantB;
    quantize(dctR, quantR, quality, width, height);
    quantize(dctG, quantG, quality, width, height);
    quantize(dctB, quantB, quality, width, height);
    
    std::vector<float> dequantR, dequantG, dequantB;
    dequantize(quantR, dequantR, quality, width, height);
    dequantize(quantG, dequantG, quality, width, height);
    dequantize(quantB, dequantB, quality, width, height);
    
    std::vector<unsigned char> rOut, gOut, bOut;
    applyInverseDCT(dequantR, rOut, width, height);
    applyInverseDCT(dequantG, gOut, width, height);
    applyInverseDCT(dequantB, bOut, width, height);
    
    std::vector<unsigned char> rgb(pixelCount * 3);
    for (int i = 0; i < pixelCount; ++i) {
        rgb[i * 3] = rOut[i];
        rgb[i * 3 + 1] = gOut[i];
        rgb[i * 3 + 2] = bOut[i];
    }
    
    return saveJPEGToMemory(rgb, width, height, quality);
}

std::vector<uint8_t> DCTCompressor::decompress(
    const std::vector<uint8_t>& compressedData,
    int& width,
    int& height,
    int& channels
) {
    return loadImageFromMemory(compressedData, width, height, channels);
}