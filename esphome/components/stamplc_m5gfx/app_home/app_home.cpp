#include "app_home.h"

#include <cmath>

#include "assets/assets.h"
#include "esphome/core/log.h"
#include "mooncake_runtime.h"
#include "reference_upstream/app/hal/hal.h"

using esphome::stamplc_m5gfx::GuiSnapshot;
using esphome::stamplc_m5gfx::mooncake_runtime_snapshot;

namespace MOONCAKE {
namespace APPS {

static const char *const TAG_APP_HOME = "AppHome";

namespace {
constexpr uint32_t kHomeThemeColor = 0x3BB2D6;  // teal – matches icon_app_home background
constexpr uint32_t kHomeAccent = 0x3BB2D6;
constexpr uint32_t kHomeColorBlack = 0xFFFFE0; // Cream
constexpr uint32_t kHomeColorWhite = 0x000000; // Black Text
constexpr uint32_t kHomeColorGood = 0x00AA00; // Dark Green
constexpr uint32_t kHomeColorMuted = 0x555555; // Dark Grey
} // namespace

const char *AppHomePacker::getAppName() { return "Humidifier"; }
void *AppHomePacker::getAppIcon() {
  return (void *)AssetPool::GetImage().AppLauncher.icon_app_humidifier;
}
void *AppHomePacker::getCustomData() { return (void *)(&kHomeThemeColor); }

void AppHome::onResume() {
  ESP_LOGI(TAG_APP_HOME, "Humidifier onResume");
  this->prev_btn_a_ = HAL::GetButton(Gamepad::BTN_A);
  this->last_render_ms_ = 0;
  this->render_();
}

void AppHome::onRunning() {
  const bool btn_a = HAL::GetButton(Gamepad::BTN_A);
  if (btn_a && !this->prev_btn_a_) {
    destroyApp();
    this->prev_btn_a_ = btn_a;
    return;
  }
  this->prev_btn_a_ = btn_a;

  const uint32_t now = HAL::Millis();
  if (this->last_render_ms_ == 0 || (now - this->last_render_ms_) >= 150U) {
    this->last_render_ms_ = now;
    this->render_();
  }
}

void AppHome::onDestroy() { ESP_LOGI(TAG_APP_HOME, "Humidifier onDestroy"); }

void AppHome::render_() {
  auto *canvas = HAL::GetCanvas();
  if (canvas == nullptr) {
    return;
  }
  const GuiSnapshot *snap = mooncake_runtime_snapshot();
  if (snap == nullptr) {
    return;
  }

  // Background
  canvas->fillScreen(kHomeColorBlack);
  
  // Header
  canvas->fillRect(0, 0, 240, 20, kHomeThemeColor);
  canvas->setTextColor(kHomeColorWhite, kHomeThemeColor);
  AssetPool::LoadFont16(canvas);
  canvas->setTextDatum(textdatum_t::top_left);
  canvas->drawString("Humidifier", 6, 2);
  
  // Clock
  canvas->setTextDatum(textdatum_t::top_right);
  canvas->drawString(snap->current_time.c_str(), 234, 2);

  // Main Values (Centered)
  canvas->setTextColor(kHomeThemeColor, kHomeColorBlack);
  AssetPool::LoadFont24(canvas);
  canvas->setTextDatum(textdatum_t::top_center);
  
  char buff[64];
  if (!std::isnan(snap->indoor_humidity) && !std::isnan(snap->target_humidity)) {
    snprintf(buff, sizeof(buff), "%.0f%%  /  %.0f%%", snap->indoor_humidity, snap->target_humidity);
  } else {
    snprintf(buff, sizeof(buff), "--%%  /  --%%");
  }
  canvas->drawString(buff, 120, 36);

  // Labels for Main Values
  canvas->setTextColor(kHomeColorWhite, kHomeColorBlack);
  AssetPool::LoadFont10(canvas);
  canvas->drawString("Current / Target", 120, 62);

  // Details (Left Aligned)
  canvas->setTextDatum(textdatum_t::top_left);
  AssetPool::LoadFont12(canvas);
  int y = 80;
  int x = 8;
  int dy = 16;

  // Relay Status
  canvas->setTextColor(kHomeColorWhite, kHomeColorBlack);
  canvas->drawString("Status: ", x, y);
  canvas->setTextColor(snap->humidifier_relay_on ? kHomeColorGood : kHomeColorMuted, kHomeColorBlack);
  canvas->drawString(snap->humidifier_relay_on ? "ON" : "OFF", x + 50, y);
  y += dy;

  // Outdoor Temp
  canvas->setTextColor(kHomeColorWhite, kHomeColorBlack);
  if (!std::isnan(snap->outdoor_temp)) {
    snprintf(buff, sizeof(buff), "Outdoor: %.1f C", snap->outdoor_temp);
  } else {
    snprintf(buff, sizeof(buff), "Outdoor: --");
  }
  canvas->drawString(buff, x, y);
  y += dy;

  // Mode
  snprintf(buff, sizeof(buff), "Mode: %s", snap->mode.empty() ? "--" : snap->mode.c_str());
  canvas->drawString(buff, x, y);
  y += dy;

  // Footer
  AssetPool::LoadFont10(canvas);
  canvas->setTextColor(kHomeColorMuted, kHomeColorBlack);
  canvas->setTextDatum(textdatum_t::bottom_left);
  canvas->drawString("A: Exit", 8, 130);

  HAL::CanvasUpdate();
}

} // namespace APPS
} // namespace MOONCAKE
