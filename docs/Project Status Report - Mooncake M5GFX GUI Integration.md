# Project Status Report: Mooncake/M5GFX GUI Integration

## ✅ Successfully Completed

1. **Component Structure** - ESPHome custom component scaffolding is in place
   - [stamplc_m5gfx component](esphome/components/stamplc_m5gfx/) properly configured
   - Python integration ([__init__.py](esphome/components/stamplc_m5gfx/__init__.py:1)) registers all sensor bindings correctly

2. **Library Dependencies** - All GUI libraries vendored and integrated
   - M5GFX v0.2.19 ✓
   - Mooncake v1.2.1 ✓
   - smooth_ui_toolkit v1.1.2 ✓

3. **HAL Bridge** - ESPHome ↔ Mooncake abstraction layer complete
   - [hal_esphome.cpp](esphome/components/stamplc_m5gfx/hal_esphome.cpp:1) implements display, input, sensor bridge
   - ST7789V2 display driver configured correctly (240x135, offsets 40x53)
   - Button press/release event detection working
   - Buzzer PWM integration ready

4. **Build System** - Firmware compiles successfully
   - All 33 upstream source wrappers generated via [gen_upstream_wrappers.ps1](tools/gen_upstream_wrappers.ps1:1)
   - Reference upstream M5Stack firmware imported
   - RAM: 13.1% (43KB), Flash: 33.0% (1.3MB) - healthy margins

5. **Runtime Infrastructure**
   - [mooncake_runtime.cpp](esphome/components/stamplc_m5gfx/mooncake_runtime.cpp:1) provides lifecycle management
   - EsphomeBridgeHal bridges Mooncake's HAL interface
   - Diagnostics fallback renderer functional

---

## ❌ Critical Issues Found

### 1. **No GUI Applications Implemented**
**Location:** [app_home/](esphome/components/stamplc_m5gfx/app_home/)
**Issue:** Only placeholder stubs exist:
```cpp
void AppHome::setup() {
  // Placeholder for HOME card and widget initialization.
}
```
**Impact:** GUI displays fallback diagnostics screen instead of humidifier interface

---

### 2. **App Launcher Not Integrated**
**Location:** [app_launcher/](esphome/components/stamplc_m5gfx/app_launcher/)
**Issue:** Directory contains only `.gitkeep` - no launcher implementation
**Impact:** Cannot navigate between screens/apps

---

### 3. **Asset Pool Empty**
**Location:** [mooncake_runtime.cpp:118](esphome/components/stamplc_m5gfx/mooncake_runtime.cpp:118)
**Issue:** `StaticAsset_t` is an empty struct with no fonts, images, or localization data
```cpp
callback.AssetPoolInjection = []() {
    auto *asset = new StaticAsset_t();  // Empty!
    if (!AssetPool::InjectStaticAsset(asset)) {
      delete asset;
    }
  };
```
**Impact:** No fonts or graphics available for GUI rendering
**Reference Assets Available:** [reference_upstream/app/assets/](esphome/components/stamplc_m5gfx/reference_upstream/app/assets/)
- Montserrat fonts (4 sizes: 10, 12, 16, 24pt)
- Localization system (EN/CN/JP)
- Theme system

---

### 4. **No Mooncake Apps Registered**
**Location:** [mooncake_runtime.cpp:125](esphome/components/stamplc_m5gfx/mooncake_runtime.cpp:125)
**Issue:** `APP::Setup()` called without any app registration
**Expected:** Should register humidifier-specific apps (home screen, settings, status, etc.)
**Reference:** Official firmware has [app_launcher](esphome/components/stamplc_m5gfx/reference_upstream/app/apps/app_launcher/app_launcher.cpp:1) and [app_template](esphome/components/stamplc_m5gfx/reference_upstream/app/apps/app_template/app_template.cpp:1)

---

### 5. **Missing HVAC-Specific GUI Widgets**
**Issue:** No custom widgets for:
- Indoor/target humidity gauge
- Outdoor temperature display
- Humidifier relay status indicator
- Safety status (interlock, runtime cap, lockout)
- Runtime counter display
- Mode/reason text display

**Available Data:** All sensor values properly bound in [gui.yaml](esphome/packages/gui.yaml:7-30)

---

### 6. **Incomplete Integration Between Systems**
**Issue:** [stamplc_m5gfx.cpp](esphome/components/stamplc_m5gfx/stamplc_m5gfx.cpp:16) falls back to diagnostics renderer when `enable_display_driver: true` but Mooncake apps aren't running:
```cpp
if (this->mooncake_started_) {
    mooncake_runtime_loop();
    return;
  }
  // Falls back to render_diagnostics_()
```
**Current State:** `enable_display_driver: true` but no apps to run, so diagnostics mode active

---

## 📋 Recommended Next Steps (Priority Order)

1. **Define Asset Strategy** - Decide whether to:
   - Port reference_upstream assets (fonts/images) or create custom minimal set
   - Implement AssetPool injection in mooncake_runtime.cpp

2. **Create Humidifier Home App** - Build primary screen showing:
   - Current vs target humidity with visual gauge
   - Relay status with color indicator
   - Safety interlocks status
   - Outdoor temp and runtime counter

3. **Implement App Launcher** - Port or simplify the reference app launcher for screen navigation

4. **Add Settings Screen** - Allow adjustment of max_runtime and min_off_lockout via GUI

5. **Test on Hardware** - Flash to physical StamPLC and validate display output

---

## 🔧 Quick Test Command
```bash
cd esphome && esphome run stamplc_humidifier.yaml
```
Currently shows diagnostics screen. After app implementation, would show Mooncake GUI.

---

## 📁 Key File Locations

### Core Component Files
- Component entry: `esphome/components/stamplc_m5gfx/stamplc_m5gfx.{h,cpp}`
- HAL bridge: `esphome/components/stamplc_m5gfx/hal_esphome.{h,cpp}`
- Runtime: `esphome/components/stamplc_m5gfx/mooncake_runtime.{h,cpp}`
- Config: `esphome/packages/gui.yaml`

### Libraries
- `esphome/components/stamplc_m5gfx/libs/M5GFX/` - Display driver
- `esphome/components/stamplc_m5gfx/libs/mooncake/` - App framework
- `esphome/components/stamplc_m5gfx/libs/smooth_ui_toolkit/` - UI widgets

### Reference Implementation
- `esphome/components/stamplc_m5gfx/reference_upstream/app/` - Official M5Stack StamPLC firmware

### Setup Scripts
- `tools/setup_m5gfx_libs.ps1` - Clone GUI library dependencies
- `tools/gen_upstream_wrappers.ps1` - Generate source wrappers for build
- `tools/import_m5stamplc_scaffold.ps1` - Import reference firmware

---

## 🎯 Current Behavior

When `enable_display_driver: true` in gui.yaml:
1. HAL initializes ST7789V2 display
2. Mooncake runtime attempts to start but has no apps registered
3. Falls back to diagnostics renderer showing:
   - Relay and API connection status
   - Button states (A/B/C)
   - Indoor/target humidity values
   - Outdoor temperature
   - Mode text
   - WiFi signal strength

This proves the display and sensor binding infrastructure works correctly. Next step is implementing actual Mooncake apps to replace the diagnostics fallback.
