#include "huffman.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

// Функция для сжатия данных канала с помощью кодов Хаффмана
std::string compressChannelData(const std::vector<unsigned char>& channelData, 
                                std::map<char, std::string>& huffmanCodes) {
    std::string compressed = "";
    std::string dataStr(reinterpret_cast<const char*>(channelData.data()), channelData.size());
    for (char pixel : dataStr) {
        compressed += huffmanCodes[pixel];
    }
    return compressed;
}

// Функция для сохранения сжатых данных с разделением на каналы
void saveCompressedChannels(const std::string& filename, 
                            const std::vector<unsigned char>& red,
                            const std::vector<unsigned char>& green,
                            const std::vector<unsigned char>& blue,
                            int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось создать файл: " << filename << std::endl;
        return;
    }

    // Сохраняем метаданные
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));

    // Обрабатываем каждый канал отдельно
    std::vector<std::vector<unsigned char>> channels = {red, green, blue};
    std::string channelNames[] = {"Red", "Green", "Blue"};

    for (int c = 0; c < 3; ++c) {
        // Строим дерево Хаффмана для текущего канала
        std::map<char, int> freq;
        std::string channelStr(reinterpret_cast<const char*>(channels[c].data()), channels[c].size());
        for (char pixel : channelStr) {
            freq[pixel]++;
        }

        std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
        for (auto pair : freq) {
            pq.push(new Node(pair.first, pair.second));
        }

        while (pq.size() > 1) {
            Node* left = pq.top(); pq.pop();
            Node* right = pq.top(); pq.pop();
            Node* parent = new Node('\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }

        Node* root = pq.top();
        std::map<char, std::string> huffmanCodes;
        generateCodes(root, "", huffmanCodes);

        // Сжимаем данные канала
        std::string compressedData = compressChannelData(channels[c], huffmanCodes);

        // Сохраняем количество уникальных символов для этого канала
        size_t codeCount = huffmanCodes.size();
        file.write(reinterpret_cast<const char*>(&codeCount), sizeof(codeCount));

        // Сохраняем таблицу кодов для этого канала
        for (const auto& pair : huffmanCodes) {
            file.write(&pair.first, sizeof(char));
            size_t codeLength = pair.second.length();
            file.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
            file.write(pair.second.c_str(), codeLength);
        }

        // Сохраняем размер сжатых данных и сами данные
        size_t compressedSize = compressedData.length();
        file.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        file.write(compressedData.c_str(), compressedSize);
    }

    file.close();
}

// Функция для сжатия изображения
void compressImage(const std::string& inputFile, const std::string& outputFile) {
    int width, height, channels;
    unsigned char* data = stbi_load(inputFile.c_str(), &width, &height, &channels, 3);

    if (data == nullptr) {
        std::cout << "error: " << inputFile << std::endl;
        return;
    }

    // Разделяем на каналы
    std::vector<unsigned char> redChannel(width * height);
    std::vector<unsigned char> greenChannel(width * height);
    std::vector<unsigned char> blueChannel(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            redChannel[y * width + x] = data[idx];
            greenChannel[y * width + x] = data[idx + 1];
            blueChannel[y * width + x] = data[idx + 2];
        }
    }

    stbi_image_free(data);

    // Сохраняем сжатые данные
    saveCompressedChannels(outputFile, redChannel, greenChannel, blueChannel, width, height);
}

// Функция для распаковки изображения
void decompressImage(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << inputFile << std::endl;
        return;
    }

    // Читаем метаданные
    int width, height;
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));

    std::vector<unsigned char> redChannel(width * height);
    std::vector<unsigned char> greenChannel(width * height);
    std::vector<unsigned char> blueChannel(width * height);

    std::vector<std::vector<unsigned char>*> channels = {&redChannel, &greenChannel, &blueChannel};

    // Восстанавливаем каждый канал
    for (int c = 0; c < 3; ++c) {
        // Читаем таблицу кодов
        size_t codeCount;
        file.read(reinterpret_cast<char*>(&codeCount), sizeof(codeCount));

        std::map<std::string, unsigned char> reverseCodes;

        for (size_t i = 0; i < codeCount; ++i) {
            char symbol;
            file.read(&symbol, sizeof(char));
            
            size_t codeLength;
            file.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
            
            std::string code(codeLength, ' ');
            file.read(&code[0], codeLength);
            
            reverseCodes[code] = static_cast<unsigned char>(symbol);
        }

        // Читаем сжатые данные
        size_t compressedSize;
        file.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
        
        std::string compressedData(compressedSize, ' ');
        file.read(&compressedData[0], compressedSize);

        // Декомпрессия
        std::vector<unsigned char> decompressedData;
        std::string currentCode = "";
        
        for (char bit : compressedData) {
            currentCode += bit;
            auto it = reverseCodes.find(currentCode);
            if (it != reverseCodes.end()) {
                decompressedData.push_back(it->second);
                currentCode = "";
            }
        }

        // Копируем восстановленные данные в канал
        std::copy(decompressedData.begin(), decompressedData.end(), channels[c]->begin());
    }

    file.close();

    // Собираем изображение из каналов
    std::vector<unsigned char> imageData(width * height * 3);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x);
            imageData[idx * 3] = redChannel[idx];
            imageData[idx * 3 + 1] = greenChannel[idx];
            imageData[idx * 3 + 2] = blueChannel[idx];
        }
    }

    // Сохраняем изображение
    stbi_write_png(outputFile.c_str(), width, height, 3, imageData.data(), width * 3);
    std::cout << outputFile << std::endl;
}
