#include "assets/assets.h"
#include "esphome/core/log.h"
#include <cstring>
#include "assets/image_app_dashboard_icon_ezdata_not_ok.h"
#include "assets/image_app_dashboard_icon_ezdata_ok.h"
#include "assets/image_app_dashboard_icon_wifi_not_ok.h"
#include "assets/image_app_dashboard_icon_wifi_ok.h"
#include "assets/image_app_dashboard_tag_console.h"
#include "assets/image_app_dashboard_tag_io.h"
#include "assets/image_app_launcher_icon_app_dashboard.h"
#include "assets/image_app_launcher_icon_app_ezdata.h"
#include "assets/image_app_launcher_icon_app_log_monitor.h"
#include "assets/image_app_launcher_icon_app_setting.h"
#include "assets/image_app_launcher_icon_app_timer_relay.h"
#include "assets/image_app_launcher_icon_app_trigger_relay.h"
#include "assets/image_app_trigger_relay_icon_load.h"
#include "assets/image_app_trigger_relay_icon_save.h"

static const char *const TAG = "stamplc_m5gfx.assets";

StaticAsset_t *AssetPool::_static_asset = nullptr;

bool AssetPool::InjectStaticAsset(StaticAsset_t *asset) {
  if (_static_asset != nullptr) {
    ESP_LOGW(TAG, "Static asset already injected");
    return false;
  }
  _static_asset = asset;
  ESP_LOGI(TAG, "Static asset injected");
  return true;
}

StaticAsset_t *AssetPool::CreateStaticAsset() {
  auto *asset = new StaticAsset_t();

  // Assign Fonts (using pointers to PROGMEM data)
  asset->Font.montserrat_semibold_italic_10 = montserrat_semibolditalic_10;
  asset->Font.montserrat_semibold_italic_12 = montserrat_semibolditalic_12;
  asset->Font.montserrat_semibold_italic_16 = montserrat_semibolditalic_16;
  asset->Font.montserrat_semibold_italic_24 = montserrat_semibolditalic_24;
  asset->Font.montserrat_semibold_14 = montserrat_semibolditalic_16;

  // Assign Images
  asset->Image.AppLauncher.icon_app_humidifier = icon_app_home;
  asset->Image.AppLauncher.icon_app_dashboard = img_app_launcher_icon_app_dashboard;
  asset->Image.AppLauncher.icon_app_ezdata = img_app_launcher_icon_app_ezdata;
  asset->Image.AppLauncher.icon_app_log_monitor = img_app_launcher_icon_app_log_monitor;
  asset->Image.AppLauncher.icon_app_setting = img_app_launcher_icon_app_setting;
  asset->Image.AppLauncher.icon_app_timer_relay = img_app_launcher_icon_app_timer_relay;
  asset->Image.AppLauncher.icon_app_trigger_relay = img_app_launcher_icon_app_trigger_relay;

  asset->Image.AppDashboard.icon_wifi_ok = img_app_dashboard_icon_wifi_ok;
  asset->Image.AppDashboard.icon_wifi_not_ok = img_app_dashboard_icon_wifi_not_ok;
  asset->Image.AppDashboard.icon_ezdata_ok = img_app_dashboard_icon_ezdata_ok;
  asset->Image.AppDashboard.icon_ezdata_not_ok = img_app_dashboard_icon_ezdata_not_ok;
  asset->Image.AppDashboard.tag_console = img_app_dashboard_tag_console;
  asset->Image.AppDashboard.tag_io = img_app_dashboard_tag_io;

  asset->Image.AppTriggerRelay.icon_load = img_app_trigger_relay_icon_load;
  asset->Image.AppTriggerRelay.icon_save = img_app_trigger_relay_icon_save;

  return asset;
}

const ImagePool_t &AssetPool::GetImage() { return _static_asset->Image; }

const ColorPool_t &AssetPool::GetColor() { return _static_asset->Color; }

const TextPool_t &AssetPool::GetTextPool() { return _static_asset->Text; }

void AssetPool::LoadFont10(LGFX_SpriteFx *lgfxDevice) {
  if (!_static_asset)
    return;
  lgfxDevice->loadFont(_static_asset->Font.montserrat_semibold_italic_10);
}

void AssetPool::LoadFont12(LGFX_SpriteFx *lgfxDevice) {
  if (!_static_asset)
    return;
  lgfxDevice->loadFont(_static_asset->Font.montserrat_semibold_italic_12);
}

void AssetPool::LoadFont14(LGFX_SpriteFx *lgfxDevice) {
  if (!_static_asset)
    return;
  lgfxDevice->loadFont(_static_asset->Font.montserrat_semibold_14);
}

void AssetPool::LoadFont16(LGFX_SpriteFx *lgfxDevice) {
  if (!_static_asset)
    return;
  lgfxDevice->loadFont(_static_asset->Font.montserrat_semibold_italic_16);
}

void AssetPool::LoadFont24(LGFX_SpriteFx *lgfxDevice) {
  if (!_static_asset)
    return;
  lgfxDevice->loadFont(_static_asset->Font.montserrat_semibold_italic_24);
}
