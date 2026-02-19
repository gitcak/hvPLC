# Adding a New App to the Launcher

Apps appear in the launcher as animated cards. The launcher auto-discovers all installed apps —
you never need to touch `app_launcher.cpp`. The process is always the same three steps.

---

## Step 1 — Create Your App Files

Copy `app/apps/app_template/` as your starting point and rename it:

```
app/apps/
└── app_home/
    ├── app_home.h
    └── app_home.cpp
```

### `app_home.h`

Rename the two classes (`AppTemplate` → your app, `AppTemplate_Packer` → your packer):

```cpp
#pragma once
#include <cstdint>
#include <mooncake.h>

namespace MOONCAKE {
namespace APPS {

class AppHome : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        std::uint32_t time_count = 0;
    };
    Data_t _data;
};

class AppHome_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override    { return new AppHome; }
    void deleteApp(void* app) override { delete (AppHome*)app; }
    void* getCustomData() override;
};

}  // namespace APPS
}  // namespace MOONCAKE
```

### `app_home.cpp`

Three things to define up front — name, theme color, icon — then three lifecycle methods:

```cpp
#include "app_home.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// 1. App name — shown on the launcher card
const char* AppHome_Packer::getAppName() { return "Home"; }

// 2. Theme color — launcher card background color (RGB888)
constexpr static uint32_t _theme_color = 0xF3FFD5;
void* AppHome_Packer::getCustomData()   { return (void*)(&_theme_color); }

// 3. Icon — pointer to a 70x70 RGB565 array, or nullptr for no icon
void* AppHome_Packer::getAppIcon()
{
    return AssetPool::GetImage().AppLauncher.icon_app_dashboard;
    // or: return nullptr;
}

// Called once when the app opens (like setup())
void AppHome::onResume()
{
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::CanvasUpdate();
}

// Called every frame (like loop())
void AppHome::onRunning()
{
    Button::Update();
    if (Button::Back()->wasClicked()) destroyApp();  // returns to launcher

    // Draw your UI here using HAL::GetCanvas()...
    // Call HAL::CanvasUpdate() when you want to push a frame.
}

// Called on close — free any heap allocations here
void AppHome::onDestroy() {}
```

---

## Step 2 — Register in `apps.h`

Add your include and one `installApp` call. Order here determines card order in the launcher.

```cpp
// app/apps/apps.h

#include "app_template/app_template.h"
#include "app_launcher/app_launcher.h"
#include "app_home/app_home.h"           // ← add your include
/* Header files locator (Don't remove) */

...

inline void app_install_apps(MOONCAKE::Mooncake* mooncake)
{
    /* Install app locator (Don't remove) */
    mooncake->installApp(new MOONCAKE::APPS::AppHome_Packer);    // card 1
    mooncake->installApp(new MOONCAKE::APPS::AppSafety_Packer);  // card 2
    mooncake->installApp(new MOONCAKE::APPS::AppDevice_Packer);  // card 3
    // ...
}
```

The launcher iterates `getInstalledAppList()` automatically — your card appears without
touching any launcher code.

---

## Step 3 — (Optional) Add a Custom Icon

To use your own 70×70 icon instead of reusing an existing one:

**1. Drop the PNG into the launcher images directory:**
```
app/assets/images/app_launcher/icon_app_home.png   (must be 70×70 px)
```

**2. Add the array to `app/assets/images/types.h`:**
```cpp
struct AppLauncher_t {
    uint16_t icon_app_dashboard[4900];
    // ...existing entries...
    uint16_t icon_app_home[4900];        // ← add
};
```

**3. Add the load call to `app/assets/assets.cpp` inside `CreateStaticAsset()`:**
```cpp
// Launcher icons
// ...existing calls...
_copy_png_image("../../app/assets/images/app_launcher/icon_app_home.png",
                asset_pool->Image.AppLauncher.icon_app_home);
```

**4. Point your packer at it:**
```cpp
void* AppHome_Packer::getAppIcon()
{
    return AssetPool::GetImage().AppLauncher.icon_app_home;
}
```

---

## App Lifecycle Reference

| Method | When called | Use for |
|--------|-------------|---------|
| `onCreate()` | Once at install | Rarely needed; call `startApp()` here to auto-open |
| `onResume()` | Every time the app comes to foreground | Initial render, allocate view resources |
| `onRunning()` | Every frame while foregrounded | Input polling, animation, rendering |
| `onRunningBG()` | Every frame while backgrounded | Background tasks (e.g. launcher uses this) |
| `onPause()` | When another app opens on top | Free view resources |
| `onDestroy()` | When app is fully closed | Free heap allocations |

**Key calls inside `onRunning()`:**

```cpp
Button::Update();                          // must call before reading buttons
Button::Ok()->wasClicked()                 // BTN_B single click
Button::Next()->wasClicked()               // BTN_C single click
Button::Back()->wasClicked()               // BTN_A single click
Button::Ok()->wasHold()                    // BTN_B held

destroyApp();                              // close this app, return to launcher
HAL::GetCanvas()->fillScreen(0x000000);    // draw to framebuffer
HAL::CanvasUpdate();                       // push framebuffer to display
AssetPool::LoadFont24(HAL::GetCanvas());   // set active font (10/12/16/24)
HAL::Millis();                             // milliseconds since boot
```

---

## Available Theme Colors (from original demo)

These are the original app colors for reference — pick any RGB888 value you like:

| App | Color | Hex |
|-----|-------|-----|
| Dashboard | Light yellow-green | `0xF3FFD5` |
| EzData | Light blue | `0x9DB6F7` |
| Log Monitor | Light pink | `0xFFAAAA` |
| Timer Relay | Light peach | `0xFFDCB0` |
| Trigger Relay | Light teal | `0x74D6C5` |
| Setting | Light green | `0xB4DCBB` |

---

## Available Icons (from `AssetPool::GetImage()`)

### Launcher icons (70×70)
| Field | Description |
|-------|-------------|
| `AppLauncher.icon_app_dashboard` | Dashboard / general home |
| `AppLauncher.icon_app_ezdata` | Cloud / data |
| `AppLauncher.icon_app_log_monitor` | Log / console |
| `AppLauncher.icon_app_setting` | Settings / gear |
| `AppLauncher.icon_app_timer_relay` | Timer |
| `AppLauncher.icon_app_trigger_relay` | Trigger / relay |

### Status icons (15×15)
| Field | Description |
|-------|-------------|
| `AppDashboard.icon_wifi_ok` | WiFi connected |
| `AppDashboard.icon_wifi_not_ok` | WiFi disconnected |
| `AppDashboard.icon_ezdata_ok` | API/cloud connected |
| `AppDashboard.icon_ezdata_not_ok` | API/cloud disconnected |

### Utility icons (18×18)
| Field | Description |
|-------|-------------|
| `AppTriggerRelay.icon_save` | Save |
| `AppTriggerRelay.icon_load` | Load |

---

## Quick Checklist

- [ ] Created `app/apps/app_name/app_name.h` and `.cpp`
- [ ] Implemented `getAppName()`, `getCustomData()` (theme color), `getAppIcon()`
- [ ] Implemented `onResume()`, `onRunning()`, `onDestroy()`
- [ ] `onRunning()` calls `Button::Update()` and handles `Button::Back()->wasClicked()`
- [ ] Added `#include` to `apps.h`
- [ ] Added `mooncake->installApp(new AppName_Packer)` to `app_install_apps()` in `apps.h`
- [ ] (If custom icon) Added PNG to `app_launcher/`, updated `types.h` and `assets.cpp`
