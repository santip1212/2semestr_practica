#include "lzw.h"
#include "stb_image_write.h"
#include <fstream>
#include <unordered_map>
#include <string>

std::vector<unsigned char> compressLZW(const std::vector<unsigned char>& channel) {
    std::vector<unsigned char> compressed;
    
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, (unsigned char)i)] = i;
    }
    
    int dictSize = 256;
    std::string current;
    std::vector<int> outputCodes;
    
    for (unsigned char c : channel) {
        std::string next = current + (char)c;
        if (dictionary.find(next) != dictionary.end()) {
            current = next;
        } else {
            outputCodes.push_back(dictionary[current]);
            dictionary[next] = dictSize++;
            current = std::string(1, (char)c);
        }
    }
    
    if (!current.empty()) {
        outputCodes.push_back(dictionary[current]);
    }
    
    for (int code : outputCodes) {
        compressed.push_back((code >> 8) & 0xFF);
        compressed.push_back(code & 0xFF);
    }
    
    return compressed;
}

std::vector<unsigned char> decompressLZW(const std::vector<unsigned char>& compressed) {
    std::vector<unsigned char> decompressed;
    
    if (compressed.size() < 2) return decompressed;
    
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, (unsigned char)i);
    }
    
    int dictSize = 256;
    std::vector<int> codes;
    for (size_t i = 0; i < compressed.size(); i += 2) {
        if (i + 1 < compressed.size()) {
            int code = (compressed[i] << 8) | compressed[i + 1];
            codes.push_back(code);
        }
    }
    
    if (codes.empty()) return decompressed;
    
    std::string current = dictionary[codes[0]];
    for (unsigned char c : current) {
        decompressed.push_back(c);
    }
    
    for (size_t i = 1; i < codes.size(); ++i) {
        int code = codes[i];
        std::string entry;
        
        if (dictionary.find(code) != dictionary.end()) {
            entry = dictionary[code];
        } else if (code == dictSize) {
            entry = current + current[0];
        } else {
            break;
        }
        
        for (unsigned char c : entry) {
            decompressed.push_back(c);
        }
        
        dictionary[dictSize++] = current + entry[0];
        current = entry;
    }
    
    return decompressed;
}

void saveCompressedLZW(const std::string& filename,
                       const std::vector<unsigned char>& r,
                       const std::vector<unsigned char>& g,
                       const std::vector<unsigned char>& b,
                       int width, int height) {
    
    auto cr = compressLZW(r);
    auto cg = compressLZW(g);
    auto cb = compressLZW(b);
    
    std::ofstream out(filename, std::ios::binary);
    
    unsigned short sig = 0x4C5A;
    out.write((char*)&sig, 2);
    out.write((char*)&width, 4);
    out.write((char*)&height, 4);
    
    int lens[3] = {(int)cr.size(), (int)cg.size(), (int)cb.size()};
    out.write((char*)lens, 12);
    
    out.write((char*)cr.data(), cr.size());
    out.write((char*)cg.data(), cg.size());
    out.write((char*)cb.data(), cb.size());
}

void loadAndDecompressLZW(const std::string& filename, 
                          std::vector<unsigned char>& r,
                          std::vector<unsigned char>& g,
                          std::vector<unsigned char>& b,
                          int& width, int& height) {
    
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        return;
    }
    
    unsigned short sig;
    in.read((char*)&sig, 2);
    
    if (sig != 0x4C5A) {
        return;
    }
    
    in.read((char*)&width, 4);
    in.read((char*)&height, 4);
    
    int lens[3];
    in.read((char*)lens, 12);
    
    std::vector<unsigned char> cr(lens[0]), cg(lens[1]), cb(lens[2]);
    in.read((char*)cr.data(), lens[0]);
    in.read((char*)cg.data(), lens[1]);
    in.read((char*)cb.data(), lens[2]);
    
    r = decompressLZW(cr);
    g = decompressLZW(cg);
    b = decompressLZW(cb);
}