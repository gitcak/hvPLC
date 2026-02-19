# Mooncake/M5GFX Integration Walkthrough

This document outlines the changes made to integrate the Mooncake/M5GFX GUI framework into the ESPHome project and provides steps for verification.

## 1. Overview

The goal was to replace the existing diagnostic fallback screen with a modern, extensible GUI using the Mooncake framework and M5GFX library. The new system features an App Launcher and a Home App, with a robust asset management system optimized for the ESP32-S3's memory constraints.

## 2. Changes Made

### Component Structure
- **`components/stamplc_m5gfx`**: Created a new custom component to house the GUI logic.
- **`stamplc_m5gfx/libs`**: Integrated `Mooncake` and `M5GFX` (and dependencies like `SmoothUIToolKit`) as local libraries.
- **`stamplc_m5gfx/reference_upstream`**: Included upstream App/HA logic, adapted for ESPHome.

### Key Implementations
- **Asset Management (`AssetPool`)**:
  - Implemented a memory-efficient `AssetPool` that uses pointers to `PROGMEM` data (C arrays) for fonts and icons, avoiding RAM duplication.
  - Converted font files (`.ttf` -> `.c/.h`) and icons (`.png` -> `.c/.h`) to C arrays.
  - providing a central `AssetPool` class to serve these assets to the UI.

- **Application Logic**:
  - **`AppLauncher`**: Ported the launcher to allow selecting applications.
  - **`AppHome`**: Ported the home screen app.
  - **`MooncakeRuntime`**: Created `mooncake_runtime.cpp` to initialize the framework, register apps, and manage the lifecycle.

- **Hardware Abstraction Layer (HAL)**:
  - **`EsphomeBridgeHal`**: Implemented a bridge between Mooncake's HAL interface and ESPHome's hardware APIs (display, buttons, time).
  - Wired up ESPHome binary sensors (buttons) to the Mooncake input system.

- **Build System**:
  - **`gen_upstream_wrappers.ps1`**: Created a script to generate wrapper `.cpp` files for upstream library sources, enabling them to be compiled by ESPHome's build system.
  - **`CMakeLists.txt` / `idf_component.yml`**: Configured build flags and included directories.

## 3. Verification Plan

### Automated Verification
- **Compilation**: The firmware captures all necessary dependencies and compiles without errors. (Check `output_final_success.txt` for results).
- **Linker Check**: Verify no multiple definition errors occur (resolved by cleaning up duplicate source files).

### Manual Verification (On Device)

1.  **Flash Firmware**:
    - Use `esphome run` or the ESPHome dashboard to flash the compiled firmware to the device.

2.  **Boot Process**:
    - Observe the boot log. Look for `[I][MooncakeRuntime:...]` messages indicating successful initialization.
    - Check for `AssetPool` injection success messages.

3.  **UI Navigation**:
    - **Launcher**: efficient verification that the "App Launcher" screen appears after boot (or after a splash screen).
    - **Navigation**: Press the Next/Back/Select buttons (mapped to ESPHome binary sensors) to navigate the launcher menu.
    - **Launch App**: Select "Home" and press Select. Verify the Home app launches.
    - **Quit App**: Use the Back/Home button pattern to return to the launcher (if implemented).

4.  **Visual Inspection**:
    - **Fonts**: text should be rendered clearly using the custom Montserrat fonts.
    - **Icons**: Icons for apps should be visible and correct.
    - **Colors**: The theme colors (e.g., primary color) should be applied correctly.

## 4. Troubleshooting

- **Missing Assets**: If icons/fonts are effectively invisible or garbled, check the `AssetPool::InjectStaticAsset` call in `mooncake_runtime.cpp`.
- **Input Not Working**: Verify the GPIO mappings in `esphome/stamplc_humidifier.yaml` match the button IDs used in `EsphomeBridgeHal`.
- **Crash/Reboot**: Check the serial monitor for stack traces. Common causes are null pointer dereferences in `AssetPool` or memory exhaustion (though PROGMEM usage minimizes this).
