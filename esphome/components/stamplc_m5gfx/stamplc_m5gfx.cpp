#include "stamplc_m5gfx.h"

#include <cmath>

#include "esphome/core/log.h"

namespace esphome {
namespace stamplc_m5gfx {

static const char *const TAG = "stamplc_m5gfx";

void StamplcM5GfxComponent::setup() {
  ESP_LOGI(TAG, "stamplc_m5gfx setup");
  this->bind_hal_();
  this->hal_.init();
  if (this->enable_display_driver_ && this->hal_.display_ready()) {
    if (this->lcd_backlight_switch_ != nullptr) {
      this->lcd_backlight_switch_->turn_on();
      ESP_LOGD(TAG, "LCD backlight on");
    }
    this->mooncake_started_ = mooncake_runtime_setup(&this->hal_);
    if (!this->mooncake_started_) {
      ESP_LOGW(TAG, "Mooncake startup failed, keeping diagnostics fallback renderer");
    }
  }
  this->initialized_ = true;
}

void StamplcM5GfxComponent::loop() {
  if (!this->initialized_) {
    return;
  }

  this->hal_.tick();
  if (this->mooncake_started_) {
    mooncake_runtime_loop();
    return;
  }

  if (this->enable_display_driver_ && this->hal_.display_ready()) {
    const uint32_t now = this->hal_.millis();
    if ((now - this->last_render_ms_) >= 250U) {
      this->last_render_ms_ = now;
      this->render_diagnostics_();
    }
  }
}

void StamplcM5GfxComponent::bind_hal_() {
  this->hal_.set_enable_display(this->enable_display_driver_);
  this->hal_.set_display_invert(this->display_invert_);
  this->hal_.bind_indoor_humidity_sensor(this->indoor_humidity_sensor_);
  this->hal_.bind_target_humidity_sensor(this->target_humidity_sensor_);
  this->hal_.bind_outdoor_temp_sensor(this->outdoor_temp_sensor_);
  this->hal_.bind_hourly_runtime_sensor(this->hourly_runtime_sensor_);
  this->hal_.bind_board_temp_sensor(this->board_temp_sensor_);
  this->hal_.bind_bus_voltage_sensor(this->bus_voltage_sensor_);
  this->hal_.bind_wifi_signal_sensor(this->wifi_signal_sensor_);
  this->hal_.bind_uptime_sensor(this->uptime_sensor_);

  this->hal_.bind_humidity_demand_binary_sensor(this->humidity_demand_binary_sensor_);
  this->hal_.bind_interlock_ok_binary_sensor(this->interlock_ok_binary_sensor_);
  this->hal_.bind_sensors_healthy_binary_sensor(this->sensors_healthy_binary_sensor_);
  this->hal_.bind_runtime_cap_ok_binary_sensor(this->runtime_cap_ok_binary_sensor_);
  this->hal_.bind_runtime_trip_binary_sensor(this->runtime_trip_binary_sensor_);
  this->hal_.bind_min_off_lockout_binary_sensor(this->min_off_lockout_binary_sensor_);
  this->hal_.bind_api_connected_binary_sensor(this->api_connected_binary_sensor_);
  this->hal_.bind_btn_a_binary_sensor(this->btn_a_binary_sensor_);
  this->hal_.bind_btn_b_binary_sensor(this->btn_b_binary_sensor_);
  this->hal_.bind_btn_c_binary_sensor(this->btn_c_binary_sensor_);

  this->hal_.bind_mode_text_sensor(this->mode_text_sensor_);
  this->hal_.bind_reason_text_sensor(this->reason_text_sensor_);

  this->hal_.bind_humidifier_relay_switch(this->humidifier_relay_switch_);
  this->hal_.bind_lcd_backlight_switch(this->lcd_backlight_switch_);

  this->hal_.bind_max_runtime_number(this->max_runtime_number_);
  this->hal_.bind_min_off_lockout_number(this->min_off_lockout_number_);
  this->hal_.bind_buzzer_output(this->buzzer_output_);
  this->hal_.bind_time(this->time_);
}

void StamplcM5GfxComponent::render_diagnostics_() {
  auto *canvas = this->hal_.canvas();
  if (canvas == nullptr) {
    return;
  }
  const auto &snap = this->hal_.snapshot();

  canvas->fillScreen(TFT_BLACK);
  canvas->setTextColor(TFT_WHITE, TFT_BLACK);
  canvas->setTextSize(1);
  canvas->setCursor(4, 4);
  canvas->printf("M5GFX HAL READY");
  canvas->setCursor(4, 22);
  canvas->printf("Relay: %s  API: %s", snap.humidifier_relay_on ? "ON" : "OFF", snap.api_connected ? "OK" : "NO");
  canvas->setCursor(4, 36);
  canvas->printf("Btn A/B/C: %d %d %d", snap.btn_a_pressed ? 1 : 0, snap.btn_b_pressed ? 1 : 0,
                 snap.btn_c_pressed ? 1 : 0);
  canvas->setCursor(4, 50);
  if (!std::isnan(snap.indoor_humidity) && !std::isnan(snap.target_humidity)) {
    canvas->printf("Humidity: %.0f / %.0f %%", snap.indoor_humidity, snap.target_humidity);
  } else {
    canvas->printf("Humidity: -- / --");
  }
  canvas->setCursor(4, 64);
  if (!std::isnan(snap.outdoor_temp)) {
    canvas->printf("Outdoor: %.1f", snap.outdoor_temp);
  } else {
    canvas->printf("Outdoor: --");
  }
  canvas->setCursor(4, 78);
  canvas->printf("Mode: %s", snap.mode.empty() ? "--" : snap.mode.c_str());
  canvas->setCursor(4, 92);
  if (!std::isnan(snap.wifi_signal_dbm)) {
    canvas->printf("WiFi: %.0f dBm", snap.wifi_signal_dbm);
  } else {
    canvas->printf("WiFi: --");
  }

  this->hal_.canvas_update();
}

void StamplcM5GfxComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "stamplc_m5gfx:");
  ESP_LOGCONFIG(TAG, "  Enable Display Driver: %s", this->enable_display_driver_ ? "true" : "false");
  ESP_LOGCONFIG(TAG, "  Display Invert: %s", this->display_invert_ ? "true" : "false");
  LOG_SENSOR("  ", "Indoor Humidity Sensor", this->indoor_humidity_sensor_);
  LOG_SENSOR("  ", "Target Humidity Sensor", this->target_humidity_sensor_);
  LOG_SENSOR("  ", "Outdoor Temp Sensor", this->outdoor_temp_sensor_);
  LOG_SENSOR("  ", "Hourly Runtime Sensor", this->hourly_runtime_sensor_);
  LOG_SENSOR("  ", "Board Temp Sensor", this->board_temp_sensor_);
  LOG_SENSOR("  ", "Bus Voltage Sensor", this->bus_voltage_sensor_);
  LOG_SENSOR("  ", "WiFi Signal Sensor", this->wifi_signal_sensor_);
  LOG_SENSOR("  ", "Uptime Sensor", this->uptime_sensor_);

  LOG_BINARY_SENSOR("  ", "Humidity Demand", this->humidity_demand_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Interlock OK", this->interlock_ok_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Sensors Healthy", this->sensors_healthy_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Runtime Cap OK", this->runtime_cap_ok_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Runtime Trip", this->runtime_trip_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Min Off Lockout", this->min_off_lockout_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "API Connected", this->api_connected_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Button A", this->btn_a_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Button B", this->btn_b_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Button C", this->btn_c_binary_sensor_);

  LOG_TEXT_SENSOR("  ", "Mode", this->mode_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Reason", this->reason_text_sensor_);

  LOG_SWITCH("  ", "Humidifier Relay", this->humidifier_relay_switch_);
  LOG_SWITCH("  ", "LCD Backlight", this->lcd_backlight_switch_);

  LOG_NUMBER("  ", "Max Runtime Number", this->max_runtime_number_);
  LOG_NUMBER("  ", "Min Off Lockout Number", this->min_off_lockout_number_);
  if (this->buzzer_output_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Buzzer Output: configured");
  } else {
    ESP_LOGCONFIG(TAG, "  Buzzer Output: not configured");
  }
  ESP_LOGCONFIG(TAG, "  HAL Type: %s", this->hal_.type());
  ESP_LOGCONFIG(TAG, "  HAL Display Ready: %s", this->hal_.display_ready() ? "true" : "false");
  ESP_LOGCONFIG(TAG, "  Mooncake Runtime Running: %s", this->mooncake_started_ ? "true" : "false");
}

float StamplcM5GfxComponent::get_setup_priority() const { return setup_priority::PROCESSOR; }

}  // namespace stamplc_m5gfx
}  // namespace esphome
