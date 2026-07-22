#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <vector>
#include <cstdint>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief Сохраняет RGB данные в JPEG и возвращает как вектор байт
 * @param data RGB данные (3 байта на пиксель)
 * @param width Ширина изображения
 * @param height Высота изображения
 * @param quality Качество JPEG (1-100)
 * @return Вектор байт с JPEG данными
 */
std::vector<uint8_t> saveJPEGToMemory(
    const std::vector<unsigned char>& data,
    int width,
    int height,
    int quality = 80
);

/**
 * @brief Загружает изображение из памяти в RGB данные
 * @param imageData Данные изображения (JPEG, BMP, PNG и т.д.)
 * @param width Ширина изображения (возвращается)
 * @param height Высота изображения (возвращается)
 * @param channels Количество каналов (возвращается)
 * @return RGB данные (3 байта на пиксель)
 */
std::vector<unsigned char> loadImageFromMemory(
    const std::vector<uint8_t>& imageData,
    int& width,
    int& height,
    int& channels
);

#endif // IMAGE_UTILS_H