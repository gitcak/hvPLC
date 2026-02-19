/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "rgb565_converter.h"
#include <png.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static inline uint16_t rgb888ToRgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

ImageConversionError convertPNGToR5G6B5(const char* filePath, uint16_t* output, size_t* outputLength, int* width,
                                        int* height)
{
    FILE* fp = fopen(filePath, "rb");
    if (!fp) {
        return ImageConversionError::FileOpenFailed;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        return ImageConversionError::PngCreateReadFailed;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(fp);
        return ImageConversionError::PngCreateInfoFailed;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        return ImageConversionError::PngReadError;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    *width               = png_get_image_width(png, info);
    *height              = png_get_image_height(png, info);
    png_byte color_type  = png_get_color_type(png, info);
    png_byte bit_depth   = png_get_bit_depth(png, info);

    // Convert to 8-bit RGBA
    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    // Read rows
    std::vector<png_bytep> row_pointers(*height);
    size_t row_bytes = png_get_rowbytes(png, info);
    for (int y = 0; y < *height; y++) {
        row_pointers[y] = (png_byte*)malloc(row_bytes);
    }
    png_read_image(png, row_pointers.data());

    // Convert to RGB565
    size_t pixel_count = (*width) * (*height);
    for (int y = 0; y < *height; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < *width; x++) {
            png_bytep px = &(row[x * 4]);
            output[y * (*width) + x] = rgb888ToRgb565(px[0], px[1], px[2]);
        }
    }

    *outputLength = pixel_count;

    // Cleanup
    for (int y = 0; y < *height; y++) {
        free(row_pointers[y]);
    }
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);

    return ImageConversionError::Success;
}
