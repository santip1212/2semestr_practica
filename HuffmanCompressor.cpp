#include "HuffmanCompressor.h"
#include "ImageUtils.h"
#include <map>
#include <queue>
#include <string>
#include <cstring>

struct HuffmanNode {
    unsigned char ch;
    int freq;
    HuffmanNode *left, *right;
    
    HuffmanNode(unsigned char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r) : ch(0), freq(f), left(l), right(r) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

static void generateCodes(HuffmanNode* root, std::string code, std::map<unsigned char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = code;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

static void deleteTree(HuffmanNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

static std::vector<unsigned char> compressChannelHuffman(const std::vector<unsigned char>& channel) {
    if (channel.empty()) return std::vector<unsigned char>();
    
    std::map<unsigned char, int> freq;
    for (unsigned char c : channel) {
        freq[c]++;
    }
    
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
    for (auto& p : freq) {
        pq.push(new HuffmanNode(p.first, p.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        pq.push(new HuffmanNode(left->freq + right->freq, left, right));
    }
    
    HuffmanNode* root = pq.top();
    
    std::map<unsigned char, std::string> codes;
    generateCodes(root, "", codes);
    
    std::string bitString;
    for (unsigned char c : channel) {
        bitString += codes[c];
    }
    
    std::vector<unsigned char> result;
    
    result.push_back(static_cast<unsigned char>(codes.size()));
    for (auto& p : codes) {
        result.push_back(p.first);
        result.push_back(static_cast<unsigned char>(p.second.size()));
        for (char bit : p.second) {
            result.push_back(bit == '1' ? 1 : 0);
        }
    }
    
    uint32_t bitLen = static_cast<uint32_t>(bitString.length());
    result.insert(result.end(), (unsigned char*)&bitLen, (unsigned char*)&bitLen + 4);
    
    unsigned char byte = 0;
    int bitPos = 0;
    for (char bit : bitString) {
        if (bit == '1') {
            byte |= (1 << (7 - bitPos));
        }
        bitPos++;
        if (bitPos == 8) {
            result.push_back(byte);
            byte = 0;
            bitPos = 0;
        }
    }
    if (bitPos > 0) {
        result.push_back(byte);
    }
    
    deleteTree(root);
    
    return result;
}

static std::vector<unsigned char> decompressChannelHuffman(const std::vector<unsigned char>& compressed) {
    if (compressed.empty()) return std::vector<unsigned char>();
    
    size_t pos = 0;
    
    unsigned char codeCount = compressed[pos++];
    std::map<std::string, unsigned char> reverseCodes;
    
    for (int i = 0; i < codeCount; ++i) {
        unsigned char symbol = compressed[pos++];
        unsigned char codeLen = compressed[pos++];
        std::string code;
        for (int j = 0; j < codeLen; ++j) {
            code += (compressed[pos++] ? '1' : '0');
        }
        reverseCodes[code] = symbol;
    }
    
    uint32_t bitLen;
    memcpy(&bitLen, &compressed[pos], 4);
    pos += 4;
    
    std::vector<unsigned char> result;
    std::string currentCode;
    int bitPos = 0;
    unsigned char byte = 0;
    
    for (size_t i = 0; i < bitLen; ++i) {
        if (bitPos == 0) {
            byte = compressed[pos++];
        }
        char bit = (byte & (1 << (7 - bitPos))) ? '1' : '0';
        bitPos = (bitPos + 1) % 8;
        
        currentCode += bit;
        auto it = reverseCodes.find(currentCode);
        if (it != reverseCodes.end()) {
            result.push_back(it->second);
            currentCode.clear();
        }
    }
    
    return result;
}

std::vector<uint8_t> HuffmanCompressor::compress(
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
    
    auto cr = compressChannelHuffman(r);
    auto cg = compressChannelHuffman(g);
    auto cb = compressChannelHuffman(b);
    
    auto dr = decompressChannelHuffman(cr);
    auto dg = decompressChannelHuffman(cg);
    auto db = decompressChannelHuffman(cb);
    
    std::vector<unsigned char> rgb(pixelCount * 3);
    for (int i = 0; i < pixelCount; ++i) {
        rgb[i * 3] = dr[i];
        rgb[i * 3 + 1] = dg[i];
        rgb[i * 3 + 2] = db[i];
    }
    
    return saveJPEGToMemory(rgb, width, height, 80);
}

std::vector<uint8_t> HuffmanCompressor::decompress(
    const std::vector<uint8_t>& compressedData,
    int& width,
    int& height,
    int& channels
) {
    std::vector<unsigned char> result = loadJPEGFromMemory(compressedData, width, height, channels);
    return result;
}