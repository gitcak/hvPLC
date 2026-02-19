/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app.h"
// #include "apps/apps.h"
#include "assets/assets.h"
#include "hal/hal.h"
#include "shared/shared.h"
#include "spdlog/spdlog.h"
#include <mooncake.h>

using namespace MOONCAKE;
static Mooncake *_mooncake = nullptr;

void APP::Setup(SetupCallback_t callback) {
  spdlog::info("app setup");

  /* --------------------------------------------------------------------------
   */
  /*                            Asset pool injection */
  /* --------------------------------------------------------------------------
   */
  spdlog::info("asset pool injection:");
  if (callback.AssetPoolInjection != nullptr) {
    callback.AssetPoolInjection();
  } else {
    spdlog::warn("empty callback");
  }

  /* --------------------------------------------------------------------------
   */
  /*                            Shared data injection */
  /* --------------------------------------------------------------------------
   */
  spdlog::info("shared data injection:");
  if (callback.sharedDataInjection != nullptr) {
    callback.sharedDataInjection();
  } else {
    spdlog::info("empty callback, inject type base");
    SharedData::Inject(new SharedData);
  }

  /* --------------------------------------------------------------------------
   */
  /*                                HAL injection */
  /* --------------------------------------------------------------------------
   */
  spdlog::info("hal injection:");
  if (callback.HalInjection != nullptr) {
    callback.HalInjection();
  } else {
    spdlog::warn("empty callback");
  }

  /* --------------------------------------------------------------------------
   */
  /*                                Mooncake apps */
  /* --------------------------------------------------------------------------
   */
  _mooncake = new Mooncake;
  _mooncake->init();

  // Removed default apps installation
  // app_run_startup_anim(_mooncake);
  // app_install_default_startup_app(_mooncake);
  // app_install_apps(_mooncake);
}

void APP::Loop() {
  if (_mooncake)
    _mooncake->update();
  HAL::FeedTheDog();
}

void APP::Destroy() {
  if (_mooncake)
    delete _mooncake;
  _mooncake = nullptr;
  HAL::Destroy();
  spdlog::warn("app destroy");
}

void APP::Install(MOONCAKE::APP_PACKER_BASE *app_packer, void *app_data,
                  void *user_data) {
  if (_mooncake) {
    // user_data and app_data are ignored in standard installApp?
    // Based on error message, installApp takes only 1 arg.
    _mooncake->installApp(app_packer);
  }
}

void APP::Start() {}

MOONCAKE::Mooncake *APP::GetMooncake() { return _mooncake; }
