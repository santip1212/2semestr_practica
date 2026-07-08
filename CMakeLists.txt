cmake_minimum_required(VERSION 3.10)

project(RLECompressor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RLECompressor RLE.cpp)

target_include_directories(RLECompressor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Настройки компиляции
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(RLECompressor PRIVATE
        -Wall
        -Wextra
        -Wno-missing-field-initializers
        -Wno-unused-parameter
        -O2
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(RLECompressor PRIVATE
        /W3
        /O2
    )
endif()

# Копируем тестовое изображение в папку сборки (если есть)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/leto.bmp)
    add_custom_command(TARGET RLECompressor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/leto.bmp
            ${CMAKE_CURRENT_BINARY_DIR}/leto.bmp
        COMMENT "Copying test image"
    )
endif()