/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <cstddef>

enum class ImageConversionError {
    Success = 0,
    FileOpenFailed,
    PngCreateReadFailed,
    PngCreateInfoFailed,
    PngReadError,
    InvalidFormat,
};

/**
 * @brief Convert a PNG file to RGB565 (R5G6B5) pixel array
 *
 * @param filePath Path to the PNG file
 * @param output Output buffer for RGB565 pixel data
 * @param outputLength Number of pixels written
 * @param width Image width (output)
 * @param height Image height (output)
 * @return ImageConversionError
 */
ImageConversionError convertPNGToR5G6B5(const char* filePath, uint16_t* output, size_t* outputLength, int* width,
                                        int* height);
