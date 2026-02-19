# Plan: Integrate M5GFX/Mooncake GUI into hvPLC ESPHome Project

## Context

The hvPLC project is an ESPHome-based humidifier controller running on M5Stack StamPLC (ESP32-S3). It currently uses LVGL for its display UI — a functional but basic 4-page text-based interface. We want to replace the LVGL UI with the M5GFX/Mooncake GUI framework (extracted from the M5StamPLC-UserDemo), which provides smooth card-style animations, a launcher with icons, and a more polished visual experience.

**Approach:** ESPHome custom external component. ESPHome continues to handle WiFi, HA API, sensors, relay, safety logic — all existing YAML stays. A new external component initializes M5GFX, runs Mooncake, and renders the GUI. The LVGL display config is removed.

**Scope (minimal, iterative):** Get the launcher + one HOME page working first.

---

## Architecture

```
ESPHome (main loop, ~143Hz)
├── WiFi, API, OTA, SNTP          (unchanged YAML)
├── I2C: AW9523, LM75B, RX8130    (unchanged YAML)
├── Safety interval (5s)           (unchanged YAML)
├── HA sensor imports              (unchanged YAML)
├── Relay + button hardware        (modified - remove LVGL refs from lambdas)
└── stamplc_m5gfx component        (NEW external component)
    ├── M5GFX display init (ST7789V2 via SPI)
    ├── Mooncake framework (app lifecycle)
    ├── SmoothUIToolkit (animations)
    ├── Launcher app (card-style selector)
    └── Home app (humidity, relay, demand, mode display)
```

**Key insight:** ESPHome's `loop()` calls our component's `loop()` on every iteration. Inside that, we call `Mooncake::update()` which handles input polling, animation ticks, and rendering. M5GFX pushes the framebuffer to the display via SPI.

---

## File Structure (new/modified files)

```
hvPLC/
├── esphome/
│   ├── stamplc_humidifier.yaml        (MODIFY - add external_component ref)
│   ├── packages/
│   │   ├── hardware.yaml              (MODIFY - remove display + LVGL blocks)
│   │   ├── gui.yaml                   (REPLACE - swap LVGL for stamplc_m5gfx config)
│   │   ├── ha_sensors.yaml            (MODIFY - remove LVGL label update lambdas)
│   │   └── safety.yaml                (unchanged)
│   └── components/
│       └── stamplc_m5gfx/             (NEW - external component)
│           ├── __init__.py            (ESPHome config schema + codegen)
│           ├── stamplc_m5gfx.h        (Component header)
│           ├── stamplc_m5gfx.cpp      (Component impl - setup/loop, sensor bindings)
│           ├── hal_esphome.h          (HAL subclass bridging ESPHome ↔ M5GFX)
│           ├── hal_esphome.cpp        (HAL impl - display init, button read, sensor access)
│           ├── app_home/              (HOME page app)
│           │   ├── app_home.h
│           │   └── app_home.cpp
│           └── libs/                  (vendored dependencies)
│               ├── M5GFX/             (git submodule or vendored)
│               ├── mooncake/          (git submodule or vendored)
│               └── smooth_ui_toolkit/ (git submodule or vendored)
```

---

## Implementation Steps

### Step 1: Create the external component skeleton
- `esphome/components/stamplc_m5gfx/__init__.py` — Python config schema
  - Declares `DEPENDENCIES = ["spi"]` (ESPHome manages SPI bus init)
  - Config schema accepts sensor/switch/global IDs for data binding
  - `to_code()` generates C++ that wires ESPHome entities to our component
- `stamplc_m5gfx.h/.cpp` — Main component class
  - Inherits `esphome::Component`
  - `setup()`: Initialize M5GFX display, create canvas, inject HAL, start Mooncake
  - `loop()`: Call `Mooncake::update()`, handle framebuffer push
  - Holds pointers to ESPHome sensors/switches/globals

### Step 2: Create HAL_ESPHome (bridge layer)
- `hal_esphome.h/.cpp` — Subclass of the stripped `HAL` base class
  - `type()` returns `"ESPHome"`
  - `init()`: Configure LGFX_Device for ST7789V2 (same pins as current hardware.yaml: SPI GPIO7/8/9, DC GPIO6, CS GPIO12, panel offsets 52/40, invert=true, 135x240)
  - `getButton()`: Read from ESPHome binary_sensor components (buttons A/B/C on PI4IOE5V6408)
  - `millis()`, `delay()`: Delegate to ESP-IDF `esp_timer_get_time()` / `vTaskDelay()`
  - `beep()`, `beepStop()`: Call ESPHome RTTTL or LEDC output
  - Expose `setSensorValue()` methods for the component to push ESPHome data into

### Step 3: Vendor the GUI libraries
- Add M5GFX (v0.1.17), mooncake (v1.2.1), smooth_ui_toolkit (v1.1.2) as git submodules under `esphome/components/stamplc_m5gfx/libs/`
- Create a `CMakeLists.txt` in the component dir that registers source files for the ESP-IDF build
- The `__init__.py` will use `cg.add_library()` or `cg.add_build_flag()` to include the library paths

### Step 4: Port the launcher app
- Copy `app_launcher/` from our stripped framework
- Copy `apps/utils/system/` (button input handler, select_menu_page)
- Copy `hal/utils/lgfx_fx/` (sprite effects)
- Copy asset framework (`assets/` — fonts, theme, localization skeleton)
- Adapt file paths for the new component directory structure
- Launcher needs at least one app to display — register the HOME app

### Step 5: Create the HOME app
- New app: `app_home/app_home.h/.cpp`
- Inherits `APP_PACKER_BASE` and `APP_BASE` (Mooncake pattern)
- Displays: indoor humidity, target humidity, relay state, demand, interlock, mode
- Reads data from HAL_ESPHome bridge (which gets it from ESPHome sensors)
- Theme color: reuse one of the original colors (e.g., 0xF3FFD5 light green)
- Icon: Reuse `icon_app_dashboard.png` (70x70) from original repo, convert to RGB565

### Step 6: Modify ESPHome YAML
- **hardware.yaml**: Remove `display:` mipi_spi block entirely (M5GFX owns the display). Keep SPI bus config (needed by ESPHome for pin reservation). Remove backlight switch (M5GFX handles it). Keep buttons but simplify lambdas — remove all LVGL calls, just publish button events.
- **gui.yaml**: Replace entirely — remove all LVGL config (fonts, styles, widgets, pages, interval). Add `stamplc_m5gfx:` component config with sensor/switch/global bindings.
- **ha_sensors.yaml**: Remove all `on_value:` lambdas that call `lv_label_set_text()`. Keep the sensor imports (the component reads `.state` directly).
- **stamplc_humidifier.yaml**: Add local external_component source path.

### Step 7: Handle the asset pipeline
- On ESP32, the original demo uses a pre-compiled `AssetPool.bin` in a flash partition
- For ESPHome, we'll embed the font .vlw files and icon PNG data as `PROGMEM` const arrays in C++ headers (generated from the PNGs at build time or pre-converted)
- The `_copy_png_image()` desktop pipeline won't work on ESP32 — instead we'll use the original repo's approach: `image2c` or similar to convert PNGs to C arrays at build time
- Alternatively, embed the RGB565 arrays directly as static const data in the component source

### Step 8: Build and test
- `esphome compile esphome/stamplc_humidifier.yaml`
- Verify: SPI display initializes, Mooncake renders launcher, button navigation works
- Verify: HOME app displays real sensor data from HA via ESPHome bridge
- Verify: Relay, safety, OTA, WiFi all still work (untouched YAML)

---

## Key Technical Decisions

1. **M5GFX owns SPI display directly** — ESPHome's `display:` component is removed. M5GFX configures ST7789V2 via its own LGFX_Device panel config. No conflict since ESPHome SPI component just initializes the bus.

2. **Buttons stay as ESPHome binary_sensors** — The PI4IOE5V6408 expander is on I2C (separate from SPI). ESPHome continues to own button reading. The HAL_ESPHome bridge polls button state from ESPHome binary_sensor components.

3. **Sensor data flows ESPHome → HAL_ESPHome → GUI** — The component holds pointers to ESPHome sensor/switch/global objects. HAL_ESPHome exposes getter methods. GUI apps call these to get current values for rendering.

4. **No FreeRTOS dual-core needed** — Mooncake's `update()` is non-blocking and designed to run in a cooperative loop. ESPHome's main loop calls it every ~7ms, which is plenty for 30+ fps rendering.

5. **Assets embedded as C arrays** — Pre-convert fonts and icons to C header files with uint8_t/uint16_t arrays. No filesystem needed. Fonts: embed the .vlw binary data. Icons: convert PNG → RGB565 → C array.

---

## Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| M5GFX SPI conflicts with ESPHome SPI | Display fails to init | Remove ESPHome display component; M5GFX inits SPI independently |
| ESPHome loop too slow for animations | Choppy UI | Mooncake is designed for 15-50ms update intervals; ESPHome loop is ~7ms — more than enough |
| Flash size (M5GFX + mooncake + fonts) | Won't fit in 8MB | M5GFX core is ~200KB compiled; fonts + icons ~100KB; plenty of room |
| Button polling latency | Missed presses | ESPHome polls GPIO expander on I2C at loop speed; HAL reads cached state |
| Build complexity (vendored libs) | Hard to maintain | Use git submodules with pinned versions |

---

## Verification

1. **Compile check**: `esphome compile esphome/stamplc_humidifier.yaml` succeeds
2. **Display test**: Launcher renders with card animation on ST7789V2
3. **Button test**: A/B/C buttons navigate launcher and HOME page
4. **Data test**: HOME page shows live humidity/relay/demand values from HA
5. **Safety test**: Relay toggle, lockout, runtime trip all still work (YAML unchanged)
6. **OTA test**: Can still upload firmware via ESPHome OTA
7. **HA integration test**: Device appears in HA, sensors/switches work
