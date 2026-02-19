/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <smooth_ui_toolkit.h>

/* -------------------------------------------------------------------------- */
/*                               Gamepad button                               */
/* -------------------------------------------------------------------------- */
namespace Gamepad {
enum GamepadButton_t {
    BTN_START = 0,
    BTN_SELECT,
    BTN_UP,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_DOWN,
    BTN_X,
    BTN_Y,
    BTN_A,
    BTN_B,
    BTN_C,
    BTN_LEFT_STICK,
    GAMEPAD_BUTTON_NUM,
};
}  // namespace Gamepad

/* -------------------------------------------------------------------------- */
/*                                 Locale code                                */
/* -------------------------------------------------------------------------- */
enum LocaleCode_t {
    locale_code_en = 0,
    locale_code_cn,
    locale_code_jp,
};

/* -------------------------------------------------------------------------- */
/*                                System config                               */
/* -------------------------------------------------------------------------- */
namespace Config {
// Default config
struct SystemConfig_t {
    uint8_t brightness  = 255;
    bool beepOn         = true;
    LocaleCode_t localeCode = locale_code_en;
};
}  // namespace Config

/* -------------------------------------------------------------------------- */
/*                                    MISC                                    */
/* -------------------------------------------------------------------------- */
typedef std::function<void(const std::string& log, bool pushScreen, bool clear)> OnLogPageRenderCallback_t;

/* -------------------------------------------------------------------------- */
/*                                  NVS keys                                  */
/* -------------------------------------------------------------------------- */
#define NVS_KEY_APP_HISTORY "app_history"
#define NVS_KEY_BOOT_COUNT  "boot_count"
