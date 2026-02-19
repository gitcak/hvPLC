#include "app_system.h"

#include <cmath>

#include "assets/assets.h"
#include "esphome/core/log.h"
#include "mooncake_runtime.h"
#include "reference_upstream/app/hal/hal.h"

using esphome::stamplc_m5gfx::GuiSnapshot;
using esphome::stamplc_m5gfx::mooncake_runtime_snapshot;

namespace MOONCAKE {
namespace APPS {

static const char *const TAG_APP_SYSTEM = "AppSystem";

namespace {
constexpr uint32_t kSystemThemeColor = 0xB5DFBD;  // mint green – matches icon_app_setting bg
constexpr uint32_t kSystemColorBlack = 0xFFFFE0; // Cream
constexpr uint32_t kSystemColorWhite = 0x000000; // Black Text
} // namespace

const char *AppSystemPacker::getAppName() { return "System"; }
void *AppSystemPacker::getAppIcon() {
  return (void *)AssetPool::GetImage().AppLauncher.icon_app_setting;
}
void *AppSystemPacker::getCustomData() { return (void *)(&kSystemThemeColor); }

void AppSystem::onResume() {
  ESP_LOGI(TAG_APP_SYSTEM, "System onResume");
  this->prev_btn_a_ = HAL::GetButton(Gamepad::BTN_A);
  this->prev_btn_b_ = HAL::GetButton(Gamepad::BTN_B);
  this->prev_btn_c_ = HAL::GetButton(Gamepad::BTN_C);
  this->last_render_ms_ = 0;
  this->render_();
}

void AppSystem::onRunning() {
  const bool btn_a = HAL::GetButton(Gamepad::BTN_A);
  const bool btn_b = HAL::GetButton(Gamepad::BTN_B);
  const bool btn_c = HAL::GetButton(Gamepad::BTN_C);

  if (btn_a && !this->prev_btn_a_) {
    destroyApp();
    this->prev_btn_a_ = btn_a;
    return;
  }

  // Btn B: (Reserved for future items)
  if (btn_b && !this->prev_btn_b_) {
    // No-op for single item
    this->render_();
  }

  // Btn C: Toggle Timeout
  if (btn_c && !this->prev_btn_c_) {
    int32_t current = HAL::NvsGet("display_timeout");
    if (current == 0) current = 300000;
    // Toggle 5 min (300000) <-> 1 hour (3600000)
    int32_t next = (current <= 300000) ? 3600000 : 300000;
    HAL::NvsSet("display_timeout", next);
    ESP_LOGI(TAG_APP_SYSTEM, "Display timeout set to %d ms (%s)", next, next <= 300000 ? "5m" : "1h");
    this->render_();
  }

  this->prev_btn_a_ = btn_a;
  this->prev_btn_b_ = btn_b;
  this->prev_btn_c_ = btn_c;

  const uint32_t now = HAL::Millis();
  if (this->last_render_ms_ == 0 || (now - this->last_render_ms_) >= 500U) {
    this->last_render_ms_ = now;
    this->render_(); // Refresh mainly for potential external updates
  }
}

void AppSystem::onDestroy() { ESP_LOGI(TAG_APP_SYSTEM, "System onDestroy"); }

void AppSystem::render_() {
  auto *canvas = HAL::GetCanvas();
  if (canvas == nullptr) {
    return;
  }
  const GuiSnapshot *snap = mooncake_runtime_snapshot();
  if (snap == nullptr) {
    return;
  }

  // Clear
  canvas->fillScreen(kSystemColorBlack);
  canvas->fillRect(0, 0, 240, 20, kSystemThemeColor);
  canvas->setTextColor(kSystemColorWhite, kSystemThemeColor);
  AssetPool::LoadFont16(canvas);
  canvas->setTextDatum(textdatum_t::top_left);
  canvas->drawString("Settings & Info", 6, 2);

  // Clock
  canvas->setTextDatum(textdatum_t::top_right);
  canvas->drawString(snap->current_time.c_str(), 234, 2);

  // --- Toggle Section ---
  canvas->setTextColor(kSystemColorWhite, kSystemColorBlack);
  AssetPool::LoadFont16(canvas);
  canvas->setCursor(8, 30);
  canvas->printf("> Timeout");

  int32_t timeout = HAL::NvsGet("display_timeout");
  if (timeout == 0) timeout = 300000;
  canvas->setTextColor(kSystemColorWhite, kSystemColorBlack);
  canvas->setCursor(160, 30);
  canvas->printf(timeout <= 300000 ? "5m" : "1h");

  // --- Info Section ---
  canvas->setTextColor(kSystemColorWhite, kSystemColorBlack);
  AssetPool::LoadFont12(canvas);

  canvas->setCursor(8, 52);
  if (!std::isnan(snap->hourly_runtime_min)) {
    canvas->printf("Runtime/h: %.1f min", snap->hourly_runtime_min);
  } else {
    canvas->printf("Runtime/h: --");
  }

  canvas->setCursor(8, 66);
  if (!std::isnan(snap->max_runtime_min)) {
    canvas->printf("Max Runtime: %.0f min", snap->max_runtime_min);
  } else {
    canvas->printf("Max Runtime: --");
  }

  canvas->setCursor(8, 80);
  if (!std::isnan(snap->min_off_lockout_min)) {
    canvas->printf("Min Off: %.1f min", snap->min_off_lockout_min);
  } else {
    canvas->printf("Min Off: --");
  }

  canvas->setCursor(8, 94);
  if (!std::isnan(snap->board_temp_c) && !std::isnan(snap->bus_voltage_v)) {
    canvas->printf("Board %.1f C  Bus %.2f V", snap->board_temp_c, snap->bus_voltage_v);
  } else {
    canvas->printf("Board/Bus: --");
  }

  canvas->setCursor(8, 108);
  if (!std::isnan(snap->uptime_s)) {
    canvas->printf("Uptime: %.0f h", snap->uptime_s / 3600.0f);
  } else {
    canvas->printf("Uptime: --");
  }

  // Footer
  AssetPool::LoadFont10(canvas);
  canvas->setTextColor(kSystemColorWhite, kSystemColorBlack);
  
  canvas->setTextDatum(textdatum_t::bottom_left);
  canvas->drawString("A: Exit", 8, 130);

  canvas->setTextDatum(textdatum_t::bottom_right);
  canvas->drawString("C: Toggle", 232, 130);

  HAL::CanvasUpdate();
}

} // namespace APPS
} // namespace MOONCAKE
