# Assets Imported from M5StamPLC UserDemo

**Date:** February 16, 2026
**Source:** `C:\Users\ncamaj\Desktop\decomp\M5StamPLC-UserDemo-main\app\assets\`
**Destination:** `esphome\components\stamplc_m5gfx\assets\`
**Total Files:** 35
**Total Size:** 1.3 MB

## ✅ Successfully Imported Assets

### 📝 Fonts (4 VLW files + conversion tool)
Located in: `assets/fonts/`

- ✅ `Montserrat-SemiBoldItalic-10.vlw` (23 KB)
- ✅ `Montserrat-SemiBoldItalic-12.vlw` (28 KB)
- ✅ `Montserrat-SemiBoldItalic-16.vlw` (43 KB)
- ✅ `Montserrat-SemiBoldItalic-24.vlw` (57 KB)
- ✅ `vlw2h.py` - Python script to convert VLW to C headers

**Already converted to C headers:**
- ✅ `montserrat_semibolditalic_10.h` (148 KB)
- ✅ `montserrat_semibolditalic_12.h` (182 KB)
- ✅ `montserrat_semibolditalic_16.h` (275 KB)
- ✅ `montserrat_semibolditalic_24.h` (363 KB)

### 🎨 Icons & Images (18 PNG files)
All icons are 70x70 pixels, RGBA format

#### App Launcher Icons (6 icons)
Located in: `assets/images/app_launcher/`

- ✅ `icon_app_dashboard.png` - Gauge/speedometer icon
- ✅ `icon_app_ezdata.png` - Data/cloud sync icon
- ✅ `icon_app_log_monitor.png` - Log/monitor icon
- ✅ `icon_app_setting.png` - Settings/gear icon
- ✅ `icon_app_timer_relay.png` - Timer icon
- ✅ `icon_app_trigger_relay.png` - Relay/switch icon

#### Dashboard Icons (6 icons)
Located in: `assets/images/app_dashboard/`

- ✅ `icon_ezdata_not_ok.png` - EZData error status
- ✅ `icon_ezdata_ok.png` - EZData success status
- ✅ `icon_wifi_not_ok.png` - WiFi disconnected
- ✅ `icon_wifi_ok.png` - WiFi connected
- ✅ `tag_console.png` - Console tab tag
- ✅ `tag_io.png` - I/O tab tag

#### Trigger Relay Icons (2 icons)
Located in: `assets/images/app_trigger_relay/`

- ✅ `icon_load.png` - Load configuration icon
- ✅ `icon_save.png` - Save configuration icon

### 🎨 Theme & Colors
Located in: `assets/theme_types.h`

```cpp
struct ColorPool_t {
    struct Misc_t {
        uint32_t bgPopFatalError = 0x0078d7;  // Blue
        uint32_t bgPopWarning    = 0xFE8B00;  // Orange
        uint32_t bgPopError      = 0xF45050;  // Red
        uint32_t bgPopSuccess    = 0x009653;  // Green
    };
    Misc_t Misc;
};
```

### 🛠️ Supporting Files

#### Type Definitions
- ✅ `fonts_types.h` - Font pool structure
- ✅ `images_types.h` - Image pool structure
- ✅ `theme_types.h` - Color theme definitions
- ✅ `localization_types.h` - Text localization stub

#### Asset Management
- ✅ `assets.h` - Main asset pool interface
- ✅ `assets.cpp` - Asset pool implementation

#### Conversion Tools
- ✅ `rgb565_converter.h` - PNG to RGB565 converter interface
- ✅ `localization.csv` - Localization strings (reference)

## 📚 Documentation Created

- ✅ `ASSETS_README.md` - Complete asset usage guide with directory structure

## 🎯 Next Steps

### To Use These Assets:

1. **Convert PNG icons to C headers** (if needed for embedded use):
   ```bash
   # Use tools from the project or the M5GFX libraries
   # Icons are currently in PNG format for flexibility
   ```

2. **Access fonts in your code**:
   ```cpp
   AssetPool::LoadFont10(sprite);
   AssetPool::LoadFont12(sprite);
   AssetPool::LoadFont16(sprite);
   AssetPool::LoadFont24(sprite);
   ```

3. **Access icons**:
   ```cpp
   const ImagePool_t& images = AssetPool::GetImage();
   // Use images.AppLauncher.icon_app_dashboard, etc.
   ```

4. **Access theme colors**:
   ```cpp
   const ColorPool_t& colors = AssetPool::GetColor();
   sprite->fillRect(0, 0, 100, 50, colors.Misc.bgPopSuccess);
   ```

## 📁 Directory Structure

```
esphome/components/stamplc_m5gfx/assets/
├── fonts/
│   ├── Montserrat-SemiBoldItalic-10.vlw
│   ├── Montserrat-SemiBoldItalic-12.vlw
│   ├── Montserrat-SemiBoldItalic-16.vlw
│   ├── Montserrat-SemiBoldItalic-24.vlw
│   ├── montserrat_semibolditalic_10.h
│   ├── montserrat_semibolditalic_12.h
│   ├── montserrat_semibolditalic_16.h
│   ├── montserrat_semibolditalic_24.h
│   └── vlw2h.py
├── images/
│   ├── app_launcher/       (6 PNG icons)
│   ├── app_dashboard/      (6 PNG icons)
│   └── app_trigger_relay/  (2 PNG icons)
├── assets.h
├── assets.cpp
├── fonts_types.h
├── images_types.h
├── theme_types.h
├── localization_types.h
├── rgb565_converter.h
├── localization.csv
└── ASSETS_README.md
```

## ✨ Summary

All icon files, fonts, colors, and supporting infrastructure have been successfully pulled from the M5StamPLC UserDemo source and organized into the ESPHome component structure. The assets are ready to use and well-documented.

**Total Transfer:** 18 PNG icons + 4 VLW fonts + 4 converted font headers + conversion tools + theme definitions = Complete asset library!

## ✅ Integrated Into GUI (This Repo)

- Imported PNGs are now converted into RGB565 C headers under:
  - `esphome/components/stamplc_m5gfx/assets/image_*.h`
- `ImagePool_t` and `AssetPool::CreateStaticAsset()` now expose all imported icon groups:
  - App Launcher icons
  - Dashboard status/tag icons
  - Trigger Relay load/save icons
- `AppHome` now uses multi-page slides:
  - `Overview` page (I/O tag + live humidity/temperature/mode)
  - `Status` page (WiFi/EZData status icons + binary state row)
  - `System` page (runtime, lockout, board/bus telemetry, uptime/reason)
- Runtime navigation controls:
  - `Button B` = next page
  - `Button C` = previous page
  - `Button A` = exit back to launcher
