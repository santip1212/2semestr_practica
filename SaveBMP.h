#ifndef SAVE_BMP_H
#define SAVE_BMP_H

#include <vector>
#include <string>

void saveBMP(const std::string& filename,
             const std::vector<unsigned char>& r,
             const std::vector<unsigned char>& g,
             const std::vector<unsigned char>& b,
             int width, int height);

#endif // SAVE_BMP_H