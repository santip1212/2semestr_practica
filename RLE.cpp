#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include<iostream>
#include<vector>
#include <fstream>
std::vector<unsigned char> compress(const std::vector<unsigned char>& channel){ // rle сжатие одного канала
    std::vector<unsigned char> compressed;
    int size = channel.size();
    for(int i = 0; i < size;){
        int count = 1;
        unsigned char value = channel[i];
        while(i + count < size && channel[i + count] == value && count < 255){
            count++;
        }
        compressed.push_back((unsigned char)count);
        compressed.push_back(value);
        i+=count;
    }
    return compressed;
}
std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressed) { // рапсаковка одного канала
    std::vector<unsigned char> decompressed;
    for (int i = 0; i < (int)compressed.size(); i += 2) {
        int count = compressed[i];
        unsigned char value = compressed[i + 1];
        
        for (int j = 0; j < count; j++) {
            decompressed.push_back(value);
        }
    }
    return decompressed;
}
void saveCompressed(const std::string& filename, // сохрание сжатого файла
                    const std::vector<unsigned char>& r,
                    const std::vector<unsigned char>& g,
                    const std::vector<unsigned char>& b,
                    int width, int height) {
    
    auto cr = compress(r);
    auto cg = compress(g);
    auto cb = compress(b);
    
    std::ofstream out(filename, std::ios::binary);

    unsigned short sig = 0x524C;
    out.write((char*)&sig, 2);
    out.write((char*)&width, 4);
    out.write((char*)&height, 4);  
    
    int lens[3] = {(int)cr.size(), (int)cg.size(), (int)cb.size()};
    out.write((char*)lens, 12);
    
    out.write((char*)cr.data(), cr.size());
    out.write((char*)cg.data(), cg.size());
    out.write((char*)cb.data(), cb.size());
}
void saveBMP(const std::string& filename, // сохранение распакованого bmp
             const std::vector<unsigned char>& r,
             const std::vector<unsigned char>& g,
             const std::vector<unsigned char>& b,
             int width, int height) {
    
    std::vector<unsigned char> rgb;
    rgb.reserve(width * height * 3);
    
    for (int i = 0; i < width * height; ++i) {
        rgb.push_back(r[i]);
        rgb.push_back(g[i]);
        rgb.push_back(b[i]);
    }
    
    stbi_write_bmp(filename.c_str(), width, height, 3, rgb.data());
}
int main(){
    int width, height, channels;
    unsigned char* data = stbi_load("leto.bmp", &width, &height, &channels, 3);

    std::vector<unsigned char> redChannel(width * height);
    std::vector<unsigned char> greenChannel(width * height);
    std::vector<unsigned char> blueChannel(width * height);

    if(data == nullptr){
        std::cout << "error" << std::endl;
        return 1;
    }

    for(int y = 0; y < height; ++y){
        for(int x = 0; x < width; ++x){
            int indx = (y * width + x) * 3;
            redChannel[y * width + x] = data[indx];
            greenChannel[y * width + x] = data[indx + 1];
            blueChannel[y * width + x] = data[indx + 2];
        }
    }
    stbi_image_free(data);
    saveCompressed("leto_compressed.rle", redChannel, greenChannel, blueChannel, width, height);
    std::cout << "poluchily: " << width << "x" << height << std::endl;
    std::cout << "isxod: " << (width * height * 3) << " byit" << std::endl;
}