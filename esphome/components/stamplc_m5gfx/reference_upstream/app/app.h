/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "assets/assets.h"
#include "hal/hal.h"
#include <functional>
#include <mooncake.h>

namespace APP {
struct SetupCallback_t {
  std::function<void()> sharedDataInjection = nullptr;
  std::function<void()> AssetPoolInjection = nullptr;
  std::function<void()> HalInjection = nullptr;
};

void Setup(SetupCallback_t callback);
void Loop();
void Destroy();

// Added for integration
void Install(MOONCAKE::APP_PACKER_BASE *app_packer, void *app_data,
             void *user_data);
void Start();
MOONCAKE::Mooncake *GetMooncake();

} // namespace APP
