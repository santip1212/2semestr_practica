#ifndef LZW_H
#define LZW_H
#include <vector>
#include <string>

std::vector<unsigned char> compressLZW(const std::vector<unsigned char>& channel);
std::vector<unsigned char> decompressLZW(const std::vector<unsigned char>& compressed);

void saveCompressedLZW(const std::string& filename,
                       const std::vector<unsigned char>& r,
                       const std::vector<unsigned char>& g,
                       const std::vector<unsigned char>& b,
                       int width, int height);

void loadAndDecompressLZW(const std::string& filename, 
                          std::vector<unsigned char>& r,
                          std::vector<unsigned char>& g,
                          std::vector<unsigned char>& b,
                          int& width, int& height);

#endif // LZW_H