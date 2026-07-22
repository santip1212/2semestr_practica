#include "ImageUtils.h"
#include "stb_image_wrapper.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>

std::vector<uint8_t> saveJPEGToMemory(
    const std::vector<unsigned char>& data,
    int width,
    int height,
    int quality
) {
    std::vector<uint8_t> result;
    
    // Проверка размера данных
    if (data.size() != static_cast<size_t>(width * height * 3)) {
        std::cerr << "ERROR: data size mismatch! Expected " << width * height * 3 << ", got " << data.size() << std::endl;
        return result;
    }
    
    // Создаем имя временного файла с расширением .jpg
    char tempFilename[512];
    
    #ifdef _WIN32
    char tempPath[MAX_PATH];
    DWORD pathLen = GetTempPathA(MAX_PATH, tempPath);
    if (pathLen > 0 && pathLen < MAX_PATH) {
        static int counter = 0;
        snprintf(tempFilename, sizeof(tempFilename), "%simage_temp_%d_%d.jpg", 
                 tempPath, static_cast<int>(time(nullptr)), counter++);
    } else {
        snprintf(tempFilename, sizeof(tempFilename), "image_temp_%d.jpg", static_cast<int>(time(nullptr)));
    }
    #else
    snprintf(tempFilename, sizeof(tempFilename), "/tmp/image_temp_%d.jpg", static_cast<int>(time(nullptr)));
    #endif
    
    if (stbi_write_jpg(tempFilename, width, height, 3, data.data(), quality)) {
        std::ifstream file(tempFilename, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            result.resize(size);
            file.read(reinterpret_cast<char*>(result.data()), size);
            file.close();
        }
        remove(tempFilename);
    } else {
        std::cerr << "stbi_write_jpg FAILED!" << std::endl;
    }
    
    return result;
}

std::vector<unsigned char> loadImageFromMemory(
    const std::vector<uint8_t>& imageData,
    int& width,
    int& height,
    int& channels
) {
    int w, h, c;
    unsigned char* data = stbi_load_from_memory(
        imageData.data(),
        static_cast<int>(imageData.size()),
        &w, &h, &c, 3
    );
    
    if (!data) {
        width = 0;
        height = 0;
        channels = 0;
        return std::vector<unsigned char>();
    }
    
    width = w;
    height = h;
    channels = 3;
    
    std::vector<unsigned char> result(data, data + w * h * 3);
    free(data);
    
    return result;
}