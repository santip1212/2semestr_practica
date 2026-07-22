cmake_minimum_required(VERSION 3.10)
project(ImageCompressor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt настройки
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

# Находим Qt
find_package(Qt6 COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Создаем исполняемый файл
add_executable(image_compressor
    main.cpp
    
    # Главное приложение
    ImageCompressorApp.cpp
    ImageCompressorApp.h
    ResultWindow.cpp
    ResultWindow.h
    
    # Утилиты
    ImageUtils.cpp
    ImageUtils.h
    SaveBMP.cpp         
    SaveBMP.h           
    
    # Адаптеры для алгоритмов
    ICompressor.h
    RLECompressor.cpp
    RLECompressor.h
    HuffmanCompressor.cpp
    HuffmanCompressor.h
    LZWCompressor.cpp
    LZWCompressor.h
    DCTCompressor.cpp
    DCTCompressor.h
    FractalCompressorAdapter.cpp
    FractalCompressorAdapter.h
    
    # Существующие алгоритмы
    dct.cpp
    dct.h
    fractal_compressor.cpp
    fractal_compressor.h
    huffman.cpp
    huffman.h
    lzw.cpp
    lzw.h
    RLE.cpp
    
    stb_image_wrapper.cpp
    stb_image_wrapper.h
)

target_include_directories(image_compressor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(image_compressor
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Настройки для компиляторов
if(MSVC)
    target_compile_options(image_compressor PRIVATE /W4 /wd4244 /wd4267 /wd4996)
else()
    target_compile_options(image_compressor PRIVATE -Wall -Wextra -Wno-sign-compare)
endif()

# Включаем математическую библиотеку
if(UNIX)
    target_link_libraries(image_compressor m)
endif()

# Устанавливаем выходную директорию
set_target_properties(image_compressor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Создаем папку для выходных файлов
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "========================================")
message(STATUS "Сборка Image Compressor")
message(STATUS "Выходная директория: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "Алгоритмы: RLE, Huffman, LZW, DCT, Fractal")
message(STATUS "========================================")