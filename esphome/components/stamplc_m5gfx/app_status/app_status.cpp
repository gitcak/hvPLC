#include "app_status.h"

#include <cmath>

#include "assets/assets.h"
#include "esphome/core/log.h"
#include "mooncake_runtime.h"
#include "reference_upstream/app/hal/hal.h"

using esphome::stamplc_m5gfx::GuiSnapshot;
using esphome::stamplc_m5gfx::mooncake_runtime_snapshot;

namespace MOONCAKE {
namespace APPS {

static const char *const TAG_APP_STATUS = "AppStatus";

namespace {
constexpr uint32_t kStatusThemeColor = 0xFFAAAD;  // pink/salmon – matches icon_app_log_monitor bg
constexpr uint32_t kStatusColorBlack = 0xFFFFE0; // Cream
constexpr uint32_t kStatusColorWhite = 0x000000; // Black Text
constexpr uint32_t kStatusColorMuted = 0x555555; // Dark Grey
} // namespace

const char *AppStatusPacker::getAppName() { return "Status"; }
void *AppStatusPacker::getAppIcon() {
  return (void *)AssetPool::GetImage().AppLauncher.icon_app_log_monitor;
}
void *AppStatusPacker::getCustomData() { return (void *)(&kStatusThemeColor); }

void AppStatus::onResume() {
  ESP_LOGI(TAG_APP_STATUS, "Status onResume");
  this->prev_btn_a_ = HAL::GetButton(Gamepad::BTN_A);
  this->last_render_ms_ = 0;
  this->render_();
}

void AppStatus::onRunning() {
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

void AppStatus::onDestroy() { ESP_LOGI(TAG_APP_STATUS, "Status onDestroy"); }

void AppStatus::render_() {
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
  canvas->fillScreen(kStatusColorBlack);
  
  // Header
  canvas->fillRect(0, 0, 240, 20, kStatusThemeColor);
  canvas->setTextColor(kStatusColorWhite, kStatusThemeColor);
  AssetPool::LoadFont16(canvas);
  canvas->setTextDatum(textdatum_t::top_left);
  canvas->drawString("Status", 6, 2);

  // Clock
  canvas->setTextDatum(textdatum_t::top_right);
  canvas->drawString(snap->current_time.c_str(), 234, 2);

  // Body
  canvas->setTextColor(kStatusColorWhite, kStatusColorBlack);
  AssetPool::LoadFont12(canvas);
  canvas->setTextDatum(textdatum_t::top_left);

  int y = 36;
  int icon_x = 8;
  int text_x = 28;
  int dy = 20;

  // WiFi
  canvas->pushImage(icon_x, y - 2, 15, 15,
                    snap->api_connected ? images.AppDashboard.icon_wifi_ok
                                        : images.AppDashboard.icon_wifi_not_ok);
  if (!std::isnan(snap->wifi_signal_dbm)) {
    canvas->drawFloat(snap->wifi_signal_dbm, 0, text_x, y);
    canvas->drawString(" dBm", text_x + 35, y); 
  } else {
    canvas->drawString("WiFi: --", text_x, y);
  }
  y += dy;

  // Sensors
  canvas->pushImage(icon_x, y - 2, 15, 15,
                    snap->sensors_healthy ? images.AppDashboard.icon_ezdata_ok
                                          : images.AppDashboard.icon_ezdata_not_ok);
  canvas->drawString(snap->sensors_healthy ? "Sensors: OK" : "Sensors: CHECK", text_x, y);
  y += dy;

  // API
  canvas->pushImage(icon_x, y - 2, 15, 15,
                    snap->api_connected ? images.AppDashboard.icon_ezdata_ok
                                        : images.AppDashboard.icon_ezdata_not_ok);
  canvas->drawString(snap->api_connected ? "HA API: Connected" : "HA API: Offline", text_x, y);
  y += dy + 4;

  // Logic Status
  AssetPool::LoadFont10(canvas);
  char buff[64];
  snprintf(buff, sizeof(buff), "Demand:%d  Interlock:%d  Cap:%d", 
           snap->humidity_demand, snap->interlock_ok, snap->runtime_cap_ok);
  canvas->drawString(buff, 8, y);
  y += 14;

  snprintf(buff, sizeof(buff), "Trip:%d  Lockout:%d", 
           snap->runtime_trip, snap->min_off_lockout_active);
  canvas->drawString(buff, 8, y);

  // Footer
  canvas->setTextColor(kStatusColorMuted, kStatusColorBlack);
  canvas->setTextDatum(textdatum_t::bottom_left);
  canvas->drawString("A: Exit", 8, 130);

  HAL::CanvasUpdate();
}

} // namespace APPS
} // namespace MOONCAKE
