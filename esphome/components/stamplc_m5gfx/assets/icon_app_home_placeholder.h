#pragma once
#include <cstdint>

// 70x70 icon, solid color (0x3FB1D3 - Light Blue)
// 70 * 70 = 4900 pixels
// RGB565 format

const uint16_t icon_app_home[4900] = {
    // Fill with 0x3D9A (approx 0x3FB1D3 in RGB565)
    // 4900 times
    #include "icon_app_home_data.h" 
};

// Actually, generating 4900 lines is tedious for a text file. 
// Let's just make it a single color array filled at runtime or a smaller repeated pattern if possible.
// But Mooncake expects a pointer to an array.
// I will generate a small python script to create this header file properly.
