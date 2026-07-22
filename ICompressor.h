#ifndef ICOMPRESSOR_H
#define ICOMPRESSOR_H

#include <vector>
#include <string>
#include <cstdint>

/**
 * @brief Базовый интерфейс для всех алгоритмов сжатия
 */
class ICompressor {
public:
    virtual ~ICompressor() = default;
    
    /**
     * @brief Сжатие изображения
     * @param imageData Исходные данные изображения (RGB или RGBA)
     * @param width Ширина изображения
     * @param height Высота изображения
     * @param channels Количество каналов (3 для RGB, 4 для RGBA)
     * @return Сжатые данные в формате JPEG
     */
    virtual std::vector<uint8_t> compress(
        const std::vector<uint8_t>& imageData,
        int width,
        int height,
        int channels
    ) = 0;
    
    /**
     * @brief Распаковка изображения из JPEG
     * @param compressedData Сжатые данные (JPEG)
     * @param width Ширина изображения (возвращается)
     * @param height Высота изображения (возвращается)
     * @param channels Количество каналов (возвращается)
     * @return Распакованные данные (RGB)
     */
    virtual std::vector<uint8_t> decompress(
        const std::vector<uint8_t>& compressedData,
        int& width,
        int& height,
        int& channels
    ) = 0;
    
    /**
     * @brief Получение имени алгоритма
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Получение расширения файла
     */
    virtual std::string getExtension() const {
        return ".jpg";
    }
};

#endif // ICOMPRESSOR_H