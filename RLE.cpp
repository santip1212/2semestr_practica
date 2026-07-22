#include "stb_image_write.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <fstream>

std::vector<unsigned char> compress(const std::vector<unsigned char>& channel) {
    std::vector<unsigned char> compressed;
    int size = channel.size();
    for(int i = 0; i < size; ){
        int count = 1;
        unsigned char value = channel[i];
        while(i + count < size && channel[i + count] == value && count < 255){
            count++;
        }
        compressed.push_back((unsigned char)count);
        compressed.push_back(value);
        i += count;
    }
    return compressed;
}

std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressed) {
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

void saveCompressed(const std::string& filename,
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