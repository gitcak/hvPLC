#pragma once

#include "hal_esphome.h"

namespace esphome {
namespace stamplc_m5gfx {

bool mooncake_runtime_setup(HalEsphome *hal);
void mooncake_runtime_loop();
void mooncake_runtime_destroy();
bool mooncake_runtime_running();
const GuiSnapshot *mooncake_runtime_snapshot();

}  // namespace stamplc_m5gfx
}  // namespace esphome
