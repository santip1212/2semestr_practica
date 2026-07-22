#include "ImageUtils.h"
#include "stb_image.h"
#include "stb_image_wrapper.h"
#include "stb_image_write.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "stb_image_write.h"

std::vector<uint8_t> saveJPEGToMemory(
    const std::vector<unsigned char>& data,
    int width,
    int height,
    int quality
) {
    std::vector<uint8_t> result;
    
    // Создаем временный файл
    char tempFilename[256];
    #ifdef _WIN32
    // Для Windows используем tmpnam_s
    char* tempName = nullptr;
    if (tmpnam_s(tempFilename, sizeof(tempFilename)) == 0) {
        // OK
    } else {
        // fallback
        strcpy_s(tempFilename, sizeof(tempFilename), "temp_image.jpg");
    }
    #else
    strcpy(tempFilename, "/tmp/image_temp.jpg");
    #endif
    
    // Сохраняем в JPEG через файл
    if (stbi_write_jpg(tempFilename, width, height, 3, data.data(), quality)) {
        // Читаем файл в память
        std::ifstream file(tempFilename, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            result.resize(size);
            file.read(reinterpret_cast<char*>(result.data()), size);
            file.close();
        }
        // Удаляем временный файл
        remove(tempFilename);
    }
    
    return result;
}

std::vector<unsigned char> loadJPEGFromMemory(
    const std::vector<uint8_t>& jpegData,
    int& width,
    int& height,
    int& channels
) {
    int w, h, c;
    unsigned char* data = stbi_load_from_memory(
        jpegData.data(),
        static_cast<int>(jpegData.size()),
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
    free(data);  // stbi_image_free - используем free напрямую
    
    return result;
}