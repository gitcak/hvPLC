# StamPLC M5GFX Assets

This directory contains assets ported from the M5Stack StamPLC User Demo.

## Directory Structure

```
assets/
├── fonts/                      # Font files and conversion tools
│   ├── Montserrat-SemiBoldItalic-10.vlw
│   ├── Montserrat-SemiBoldItalic-12.vlw
│   ├── Montserrat-SemiBoldItalic-16.vlw
│   ├── Montserrat-SemiBoldItalic-24.vlw
│   ├── vlw2h.py               # VLW to C header converter
│   ├── montserrat_semibolditalic_10.h  # Converted font (C header)
│   ├── montserrat_semibolditalic_12.h
│   ├── montserrat_semibolditalic_16.h
│   └── montserrat_semibolditalic_24.h
├── images/
│   ├── app_launcher/          # App launcher screen icons (70x70 PNG)
│   │   ├── icon_app_dashboard.png
│   │   ├── icon_app_ezdata.png
│   │   ├── icon_app_log_monitor.png
│   │   ├── icon_app_setting.png
│   │   ├── icon_app_timer_relay.png
│   │   └── icon_app_trigger_relay.png
│   ├── app_dashboard/         # Dashboard screen icons
│   │   ├── icon_ezdata_not_ok.png
│   │   ├── icon_ezdata_ok.png
│   │   ├── icon_wifi_not_ok.png
│   │   ├── icon_wifi_ok.png
│   │   ├── tag_console.png
│   │   └── tag_io.png
│   └── app_trigger_relay/     # Trigger relay icons
│       ├── icon_load.png
│       └── icon_save.png
├── assets.h                   # Main asset pool interface
├── assets.cpp                 # Asset pool implementation
├── fonts_types.h              # Font pool structure definitions
├── images_types.h             # Image pool structure definitions
├── theme_types.h              # Color theme definitions
├── localization_types.h       # Text localization stub
└── rgb565_converter.h         # PNG to RGB565 converter (for native builds)
```

## Asset Types

### Fonts
- **Montserrat SemiBold Italic**: Available in sizes 10, 12, 16, and 24pt
- Format: VLW (Variable Length Width) font files
- Converted to C headers using `vlw2h.py`

### Icons
All icons are 70x70 PNG images with RGBA support.

**App Launcher Icons:**
- Dashboard
- EZData
- Log Monitor
- Settings
- Timer Relay
- Trigger Relay

**Dashboard Icons:**
- WiFi status (OK/Not OK)
- EZData status (OK/Not OK)
- Console tag
- I/O tag

**Trigger Relay Icons:**
- Load
- Save

### Colors (theme_types.h)
```cpp
bgPopFatalError = 0x0078d7  // Blue
bgPopWarning    = 0xFE8B00  // Orange
bgPopError      = 0xF45050  // Red
bgPopSuccess    = 0x009653  // Green
```

## Usage

### Fonts
Use the `AssetPool` class to load fonts:
```cpp
AssetPool::LoadFont10(sprite);  // 10pt
AssetPool::LoadFont12(sprite);  // 12pt
AssetPool::LoadFont16(sprite);  // 16pt
AssetPool::LoadFont24(sprite);  // 24pt
```

### Images
Access images through the asset pool:
```cpp
const ImagePool_t& images = AssetPool::GetImage();
sprite->pushImage(0, 0, 70, 70, images.AppLauncher.icon_app_dashboard);
```

### Colors
Access theme colors:
```cpp
const ColorPool_t& colors = AssetPool::GetColor();
sprite->fillRect(0, 0, 100, 50, colors.Misc.bgPopSuccess);
```

## Converting Assets

### Fonts (VLW to C Header)
```bash
python fonts/vlw2h.py fonts/Montserrat-SemiBoldItalic-10.vlw
```

### Images (PNG to RGB565)
For native builds, use the RGB565 converter to convert PNG images to C arrays.

## Source
Assets ported from: `M5StamPLC-UserDemo-main/app/assets/`
- Repository: M5Stack StamPLC User Demo
- License: MIT
- Copyright: 2025 M5Stack Technology CO LTD

## Current Integration Status

- PNG icons are now converted to RGB565 headers in this folder:
  - `image_app_launcher_*.h`
  - `image_app_dashboard_*.h`
  - `image_app_trigger_relay_*.h`
- `ImagePool_t` is fully mapped for launcher/dashboard/trigger-relay icon sets.
- `AppHome` GUI now implements 3 slide pages with button navigation:
  - `Overview`
  - `Status`
  - `System`
- Control mapping in app runtime:
  - `BTN_B`: next slide
  - `BTN_C`: previous slide
  - `BTN_A`: return to launcher
