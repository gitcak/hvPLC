#include "app_overview.h"

#include <cmath>

#include "assets/assets.h"
#include "esphome/core/log.h"
#include "mooncake_runtime.h"
#include "reference_upstream/app/hal/hal.h"

using esphome::stamplc_m5gfx::GuiSnapshot;
using esphome::stamplc_m5gfx::mooncake_runtime_snapshot;

namespace MOONCAKE {
namespace APPS {

static const char *const TAG_APP_OVERVIEW = "AppOverview";

namespace {
constexpr uint32_t kOverviewThemeColor = 0xF7FFD6;  // light olive – header bg
constexpr uint32_t kOverviewColorBlack = 0xFFFFE0; // Cream
constexpr uint32_t kOverviewColorWhite = 0x000000; // Black Text
constexpr uint32_t kOverviewColorMuted = 0x555555; // Dark Grey
constexpr uint32_t kOverviewAccent = 0x8FA876;        // darker olive – main values on black
} // namespace

const char *AppOverviewPacker::getAppName() { return "Overview"; }
void *AppOverviewPacker::getAppIcon() {
  return (void *)AssetPool::GetImage().AppLauncher.icon_app_dashboard;
}
void *AppOverviewPacker::getCustomData() { return (void *)(&kOverviewThemeColor); }

void AppOverview::onResume() {
  ESP_LOGI(TAG_APP_OVERVIEW, "Overview onResume");
  this->prev_btn_a_ = HAL::GetButton(Gamepad::BTN_A);
  this->last_render_ms_ = 0;
  this->render_();
}

void AppOverview::onRunning() {
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

void AppOverview::onDestroy() { ESP_LOGI(TAG_APP_OVERVIEW, "Overview onDestroy"); }

void AppOverview::render_() {
  auto *canvas = HAL::GetCanvas();
  if (canvas == nullptr) {
    return;
  }
  const GuiSnapshot *snap = mooncake_runtime_snapshot();
  if (snap == nullptr) {
    return;
  }
  const auto &images = AssetPool::GetImage();

  // Background
  canvas->fillScreen(kOverviewColorBlack);
  canvas->fillRect(0, 0, 240, 20, kOverviewThemeColor);
  canvas->setTextColor(kOverviewColorWhite, kOverviewThemeColor);
  AssetPool::LoadFont16(canvas);
  canvas->setTextDatum(textdatum_t::top_left);
  canvas->drawString("Overview", 6, 2);

  // Clock
  canvas->setTextDatum(textdatum_t::top_right);
  canvas->drawString(snap->current_time.c_str(), 234, 2);

  AssetPool::LoadFont10(canvas);
  canvas->setTextColor(kOverviewColorWhite, kOverviewThemeColor);
  canvas->setTextDatum(textdatum_t::top_left);
  canvas->drawString("Humidifier", 82, 5);

  canvas->pushImage(172, 3, 15, 15,
                    snap->api_connected ? images.AppDashboard.icon_wifi_ok
                                        : images.AppDashboard.icon_wifi_not_ok);
  canvas->pushImage(192, 5, 15, 15,
                    snap->sensors_healthy ? images.AppDashboard.icon_ezdata_ok
                                          : images.AppDashboard.icon_ezdata_not_ok);

  canvas->setTextColor(kOverviewAccent, kOverviewColorBlack);
  AssetPool::LoadFont24(canvas);
  canvas->setCursor(8, 30);
  if (!std::isnan(snap->indoor_humidity) && !std::isnan(snap->target_humidity)) {
    canvas->printf("%.0f%%  /  %.0f%%", snap->indoor_humidity, snap->target_humidity);
  } else {
    canvas->printf("--%%  /  --%%");
  }

  canvas->setTextColor(kOverviewColorWhite, kOverviewColorBlack);
  AssetPool::LoadFont12(canvas);
  canvas->setCursor(8, 68);
  if (!std::isnan(snap->outdoor_temp)) {
    canvas->printf("Outdoor Temp: %.1f C", snap->outdoor_temp);
  } else {
    canvas->printf("Outdoor Temp: --");
  }
  canvas->setCursor(8, 86);
  canvas->printf("Mode: %s", snap->mode.empty() ? "--" : snap->mode.c_str());
  canvas->setCursor(8, 104);
  if (!std::isnan(snap->hourly_runtime_min)) {
    canvas->printf("Hourly Runtime: %.1f min", snap->hourly_runtime_min);
  } else {
    canvas->printf("Hourly Runtime: --");
  }

  AssetPool::LoadFont10(canvas);
  canvas->setTextColor(kOverviewColorMuted, kOverviewColorBlack);
  canvas->setTextDatum(textdatum_t::bottom_left);
  canvas->drawString("A: Exit", 8, 130);

  HAL::CanvasUpdate();
}

} // namespace APPS
} // namespace MOONCAKE
