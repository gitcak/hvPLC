/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_launcher.h"
#include "assets/assets.h"
#include "esphome/core/log.h"
#include "reference_upstream/app/hal/hal.h"
#include <cmath>
#include <cstdint>
#include <mooncake.h>


static const char *const TAG = "AppLauncher";

using namespace MOONCAKE::APPS;
using namespace SmoothUIToolKit;

static bool _is_just_boot_up = true;

// Helper to blend color
// static uint32_t BlendColorInDifference(uint32_t c1, uint32_t c2) {
//     return 0x000000;
// }

void AppLauncher::onCreate() {
  ESP_LOGI(TAG, "%s onCreate", getAppName());
  startApp();
  setAllowBgRunning(true);
}

void AppLauncher::onResume() {
  ESP_LOGI(TAG, "%s onResume", getAppName());

  // If just boot up, open history app directly
  if (_is_just_boot_up) {
    _is_just_boot_up = false;
    const auto &installed_apps = mcAppGetFramework()->getInstalledAppList();
    if (installed_apps.size() <= 1) {
      ESP_LOGW(TAG, "no launchable apps installed");
      _create_launcher_view();
      return;
    }

    // Create app (Placeholder logic for now)
    // auto history_app_index = HAL::NvsGet("APP_HISTORY");
    // ...

    // Fallthrough to create launcher view
  }

  _create_launcher_view();
}

void AppLauncher::onRunning() { _update_launcher_view(); }

void AppLauncher::onRunningBG() {
  // If only launcher left, treat app layer 0 as normal app
  if (mcAppGetFramework()->getCreatedAppNumByLayer(0) <= 1) {
    // Back to the game
    startApp();
  }
}

void AppLauncher::onPause() { _destroy_launcher_view(); }

void AppLauncher::onDestroy() { ESP_LOGI(TAG, "%s onDestroy", getAppName()); }

/* -------------------------------------------------------------------------- */
/*                                    View                                    */
/* -------------------------------------------------------------------------- */
void AppLauncher::_create_launcher_view() {
  // Create menu
  _data.launcher_view = new VIEW::LauncherView;

  // Add app option
  for (const auto &app_packer : mcAppGetFramework()->getInstalledAppList()) {
    // Pass launcher
    if (app_packer == getAppPacker()) {
      continue;
    }

    VIEW::LauncherView::AppOptionProps_t new_app_option;

    if (app_packer->getCustomData() != nullptr) {
      new_app_option.themeColor = *(uint32_t *)app_packer->getCustomData();
    }
    new_app_option.appNameColor = 0x555555; // Darker grey for launcher text
    new_app_option.name = app_packer->getAppName();
    new_app_option.icon = (const uint16_t *)app_packer->getAppIcon();

    _data.launcher_view->addAppOption(new_app_option);
  }

  // App opened callback
  _data.launcher_view->app_open_callback = [&](const std::string &appName,
                                               int appOptionIndex) {
    ESP_LOGI(TAG, "open app: %s %d", appName.c_str(), appOptionIndex);

    if (!mcAppGetFramework()->createAndStartApp(
            mcAppGetFramework()->getInstalledAppByName(appName))) {
      ESP_LOGE(TAG, "create and start app failed");
      return;
    }

    // Stack launcher into background
    closeApp();
  };

  _data.launcher_view->init();
}

void AppLauncher::_update_launcher_view() {
  _data.launcher_view->update(HAL::Millis());
}

void AppLauncher::_destroy_launcher_view() {
  if (_data.launcher_view != nullptr) {
    delete _data.launcher_view;
    _data.launcher_view = nullptr;
  }
}
