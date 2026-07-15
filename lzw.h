#ifndef LZW_H
#define LZW_H
#include <vector>
#include <string>

// LZW сжатие одного канала
std::vector<unsigned char> compressLZW(const std::vector<unsigned char>& channel);

// LZW распаковка одного канала
std::vector<unsigned char> decompressLZW(const std::vector<unsigned char>& compressed);

// Сохранение сжатого файла
void saveCompressedLZW(const std::string& filename,
                       const std::vector<unsigned char>& r,
                       const std::vector<unsigned char>& g,
                       const std::vector<unsigned char>& b,
                       int width, int height);

// Загрузка и распаковка LZW файла
void loadAndDecompressLZW(const std::string& filename, 
                          std::vector<unsigned char>& r,
                          std::vector<unsigned char>& g,
                          std::vector<unsigned char>& b,
                          int& width, int& height);

// Сохранение BMP файла
void saveBMP(const std::string& filename,
             const std::vector<unsigned char>& r,
             const std::vector<unsigned char>& g,
             const std::vector<unsigned char>& b,
             int width, int height);
#endif // LZW_H