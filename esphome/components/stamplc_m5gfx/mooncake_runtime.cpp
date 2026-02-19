#include "mooncake_runtime.h"

#include <unordered_map>

#include "esphome/core/log.h"
#include "reference_upstream/app/app.h"

// Includes for Apps and Assets
// Now accessing assets via the same path as app.h does, which is updated
#include "app_home/app_home.h"
#include "app_launcher/app_launcher.h"
#include "app_overview/app_overview.h"
#include "app_status/app_status.h"
#include "app_system/app_system.h"
#include "assets/assets.h"

namespace esphome {
namespace stamplc_m5gfx {

static const char *const TAG = "stamplc_m5gfx.runtime";

namespace {

bool s_running = false;
HalEsphome *s_hal_ref = nullptr;
uint32_t s_last_interaction_ms = 0;
bool s_backlight_on = true;

class EsphomeBridgeHal final : public HAL {
public:
  explicit EsphomeBridgeHal(HalEsphome *hal) : hal_(hal) {}

  std::string type() override { return "ESPHomeBridge"; }

  void init() override {
    if (this->hal_ == nullptr || !this->hal_->display_ready()) {
      spdlog::error("hal not ready");
      return;
    }

    this->_data.display = this->hal_->display();
    this->_data.canvas = this->hal_->canvas();
    this->_data.config.brightness = 255;
    this->_data.config.beepOn = true;
    this->_data.config.localeCode = locale_code_en;
  }

  void canvasUpdate() override {
    if (this->hal_ != nullptr) {
      this->hal_->canvas_update();
    }
  }

  unsigned long millis() override {
    return this->hal_ != nullptr ? this->hal_->millis() : 0UL;
  }

  void delay(unsigned long milliseconds) override {
    if (this->hal_ != nullptr) {
      this->hal_->delay_ms(static_cast<uint32_t>(milliseconds));
    }
  }

  void beep(float frequency, uint32_t duration) override {
    if (this->hal_ != nullptr) {
      this->hal_->beep(frequency, duration);
    }
  }

  void beepStop() override {
    if (this->hal_ != nullptr) {
      this->hal_->beep_stop();
    }
  }

  bool getButton(Gamepad::GamepadButton_t button) override {
    if (this->hal_ == nullptr) {
      return false;
    }
    bool pressed = false;
    switch (button) {
    case Gamepad::BTN_A:
      pressed = this->hal_->get_button(HalButton::BTN_A);
      break;
    case Gamepad::BTN_B:
      pressed = this->hal_->get_button(HalButton::BTN_B);
      break;
    case Gamepad::BTN_C:
      pressed = this->hal_->get_button(HalButton::BTN_C);
      break;
    default:
      break;
    }
    if (pressed) {
      s_last_interaction_ms = this->hal_->millis();
      if (!s_backlight_on) {
        ESP_LOGI(TAG, "User interaction: Display backlight ON");
        this->hal_->set_backlight(true);
        s_backlight_on = true;
      }
    }
    return pressed;
  }

  bool nvsSet(const char *key, const int32_t &value) override {
    if (key == nullptr) {
      return false;
    }
    this->nvs_[key] = value;
    return true;
  }

  int32_t nvsGet(const char *key) override {
    if (key == nullptr) {
      return 0;
    }
    const auto it = this->nvs_.find(key);
    return it == this->nvs_.end() ? 0 : it->second;
  }

  void applySystemConfig() override {
    if (this->_data.display != nullptr) {
      // Hardware doesn't support PWM, so we just use ON/OFF
      bool on = this->_data.config.brightness > 0;
      this->hal_->set_backlight(on);
      s_backlight_on = on;
    }
  }

private:
  HalEsphome *hal_{nullptr};
  std::unordered_map<std::string, int32_t> nvs_{};
};

} // namespace

bool mooncake_runtime_setup(HalEsphome *hal) {
  if (s_running) {
    return true;
  }
  if (hal == nullptr || !hal->display_ready()) {
    ESP_LOGE(TAG, "Cannot start runtime: display not ready");
    return false;
  }
  s_hal_ref = hal;

  APP::SetupCallback_t callback;
  callback.AssetPoolInjection = []() {
    auto *asset = AssetPool::CreateStaticAsset();
    if (!AssetPool::InjectStaticAsset(asset)) {
      delete asset;
    }
  };
  callback.HalInjection = [hal]() { HAL::Inject(new EsphomeBridgeHal(hal)); };

  APP::Setup(callback);

  // Register Apps
  auto *launcher = new MOONCAKE::APPS::AppLauncher_Packer();
  APP::Install(launcher, nullptr, nullptr);

  auto *home = new MOONCAKE::APPS::AppHomePacker();
  APP::Install(home, nullptr, nullptr);

  auto *overview = new MOONCAKE::APPS::AppOverviewPacker();
  APP::Install(overview, nullptr, nullptr);

  auto *status = new MOONCAKE::APPS::AppStatusPacker();
  APP::Install(status, nullptr, nullptr);

  auto *system = new MOONCAKE::APPS::AppSystemPacker();
  APP::Install(system, nullptr, nullptr);

  // Start Launcher
  if (APP::GetMooncake() &&
      APP::GetMooncake()->getInstalledAppList().size() > 0) {
    APP::GetMooncake()->createAndStartApp(
        APP::GetMooncake()->getInstalledAppList()[0]);
  } else {
    ESP_LOGE(TAG, "No apps installed!");
    return false;
  }

  s_running = true;
  s_last_interaction_ms = hal->millis();
  s_backlight_on = true;
  ESP_LOGI(TAG, "Mooncake runtime started");
  return true;
}

void mooncake_runtime_loop() {
  if (!s_running) {
    return;
  }
  APP::Loop();

  // Screen timeout logic
  if (s_hal_ref != nullptr && s_backlight_on) {
    int32_t timeout = HAL::NvsGet("display_timeout");
    if (timeout == 0) {
        timeout = 300000; // Default 5 min
    }
    if (s_hal_ref->millis() - s_last_interaction_ms > (uint32_t)timeout) {
        ESP_LOGI(TAG, "Display timeout (%d ms) reached: Display backlight OFF", timeout);
        s_hal_ref->set_backlight(false);
        s_backlight_on = false;
    }
  }
}

void mooncake_runtime_destroy() {
  if (!s_running) {
    return;
  }
  APP::Destroy();
  s_running = false;
  s_hal_ref = nullptr;
  ESP_LOGI(TAG, "Mooncake runtime stopped");
}

bool mooncake_runtime_running() { return s_running; }
const GuiSnapshot *mooncake_runtime_snapshot() {
  return s_hal_ref == nullptr ? nullptr : &s_hal_ref->snapshot();
}

} // namespace stamplc_m5gfx
} // namespace esphome
