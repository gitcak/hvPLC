/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                   Images                                   */
/* -------------------------------------------------------------------------- */
struct ImagePool_t {
    /* ----------------------- Add your image define here ----------------------- */
    struct AppLauncher_t {
        // image size: 70 x 70, array length: 4900
        // Add app icon arrays here as needed, e.g.:
        // uint16_t icon_app_my_app[4900];
    };
    AppLauncher_t AppLauncher;
};
