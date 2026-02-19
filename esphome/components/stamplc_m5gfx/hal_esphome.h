#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>

#include "M5GFX.h"
#include "reference_upstream/app/hal/utils/lgfx_fx/lgfx_fx.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace stamplc_m5gfx {

enum class HalButton : uint8_t {
  BTN_A = 0,
  BTN_B = 1,
  BTN_C = 2,
};

struct GuiSnapshot {
  float indoor_humidity{NAN};
  float target_humidity{NAN};
  float outdoor_temp{NAN};
  float hourly_runtime_min{NAN};
  float board_temp_c{NAN};
  float bus_voltage_v{NAN};
  float wifi_signal_dbm{NAN};
  float uptime_s{NAN};
  float max_runtime_min{NAN};
  float min_off_lockout_min{NAN};
  bool humidifier_relay_on{false};
  bool humidity_demand{false};
  bool interlock_ok{false};
  bool sensors_healthy{false};
  bool runtime_cap_ok{false};
  bool runtime_trip{false};
  bool min_off_lockout_active{false};
  bool api_connected{false};
  bool btn_a_pressed{false};
  bool btn_b_pressed{false};
  bool btn_c_pressed{false};
  std::string mode;
  std::string reason;
  std::string current_time;
};

struct HalDisplayConfig {
  int8_t pin_clk{7};
  int8_t pin_mosi{8};
  int8_t pin_miso{9};
  int8_t pin_dc{6};
  int8_t pin_cs{12};
  uint16_t panel_width{135};
  uint16_t panel_height{240};
  uint16_t offset_x{52};
  uint16_t offset_y{40};
  bool invert{false};
};

class HalEsphome {
 public:
  const char *type() const { return "ESPHome"; }
  void set_enable_display(bool value) { this->enable_display_ = value; }
  void set_display_invert(bool value) { this->display_config_.invert = value; }

  void bind_indoor_humidity_sensor(sensor::Sensor *value) { this->indoor_humidity_sensor_ = value; }
  void bind_target_humidity_sensor(sensor::Sensor *value) { this->target_humidity_sensor_ = value; }
  void bind_outdoor_temp_sensor(sensor::Sensor *value) { this->outdoor_temp_sensor_ = value; }
  void bind_hourly_runtime_sensor(sensor::Sensor *value) { this->hourly_runtime_sensor_ = value; }
  void bind_board_temp_sensor(sensor::Sensor *value) { this->board_temp_sensor_ = value; }
  void bind_bus_voltage_sensor(sensor::Sensor *value) { this->bus_voltage_sensor_ = value; }
  void bind_wifi_signal_sensor(sensor::Sensor *value) { this->wifi_signal_sensor_ = value; }
  void bind_uptime_sensor(sensor::Sensor *value) { this->uptime_sensor_ = value; }

  void bind_humidity_demand_binary_sensor(binary_sensor::BinarySensor *value) { this->humidity_demand_binary_sensor_ = value; }
  void bind_interlock_ok_binary_sensor(binary_sensor::BinarySensor *value) { this->interlock_ok_binary_sensor_ = value; }
  void bind_sensors_healthy_binary_sensor(binary_sensor::BinarySensor *value) { this->sensors_healthy_binary_sensor_ = value; }
  void bind_runtime_cap_ok_binary_sensor(binary_sensor::BinarySensor *value) { this->runtime_cap_ok_binary_sensor_ = value; }
  void bind_runtime_trip_binary_sensor(binary_sensor::BinarySensor *value) { this->runtime_trip_binary_sensor_ = value; }
  void bind_min_off_lockout_binary_sensor(binary_sensor::BinarySensor *value) { this->min_off_lockout_binary_sensor_ = value; }
  void bind_api_connected_binary_sensor(binary_sensor::BinarySensor *value) { this->api_connected_binary_sensor_ = value; }
  void bind_btn_a_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_a_binary_sensor_ = value; }
  void bind_btn_b_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_b_binary_sensor_ = value; }
  void bind_btn_c_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_c_binary_sensor_ = value; }

  void bind_mode_text_sensor(text_sensor::TextSensor *value) { this->mode_text_sensor_ = value; }
  void bind_reason_text_sensor(text_sensor::TextSensor *value) { this->reason_text_sensor_ = value; }

  void bind_humidifier_relay_switch(switch_::Switch *value) { this->humidifier_relay_switch_ = value; }
  void bind_lcd_backlight_switch(switch_::Switch *value) { this->lcd_backlight_switch_ = value; }

  void bind_max_runtime_number(number::Number *value) { this->max_runtime_number_ = value; }
  void bind_min_off_lockout_number(number::Number *value) { this->min_off_lockout_number_ = value; }
  void bind_buzzer_output(output::FloatOutput *value) { this->buzzer_output_ = value; }
  void bind_time(time::RealTimeClock *value) { this->time_ = value; }

  void set_backlight(bool state);

  void init();
  void tick();
  bool get_button(HalButton button) const;
  bool button_pressed_event(HalButton button) const;
  bool button_released_event(HalButton button) const;
  uint32_t millis() const;
  void delay_ms(uint32_t milliseconds) const;
  void beep(float frequency, uint32_t duration_ms = UINT32_MAX);
  void beep_stop();
  const GuiSnapshot &snapshot() const;
  bool display_ready() const { return this->display_ready_; }
  LGFX_Device *display() const { return this->display_.get(); }
  LGFX_SpriteFx *canvas() const { return this->canvas_.get(); }
  void canvas_update();
  const HalDisplayConfig &display_config() const { return this->display_config_; }

 private:
  sensor::Sensor *indoor_humidity_sensor_{nullptr};
  sensor::Sensor *target_humidity_sensor_{nullptr};
  sensor::Sensor *outdoor_temp_sensor_{nullptr};
  sensor::Sensor *hourly_runtime_sensor_{nullptr};
  sensor::Sensor *board_temp_sensor_{nullptr};
  sensor::Sensor *bus_voltage_sensor_{nullptr};
  sensor::Sensor *wifi_signal_sensor_{nullptr};
  sensor::Sensor *uptime_sensor_{nullptr};

  binary_sensor::BinarySensor *humidity_demand_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *interlock_ok_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *sensors_healthy_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *runtime_cap_ok_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *runtime_trip_binary_sensor_{nullptr};

  binary_sensor::BinarySensor *min_off_lockout_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *api_connected_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *btn_a_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *btn_b_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *btn_c_binary_sensor_{nullptr};

  text_sensor::TextSensor *mode_text_sensor_{nullptr};
  text_sensor::TextSensor *reason_text_sensor_{nullptr};

  switch_::Switch *humidifier_relay_switch_{nullptr};
  switch_::Switch *lcd_backlight_switch_{nullptr};

  number::Number *max_runtime_number_{nullptr};
  number::Number *min_off_lockout_number_{nullptr};
  output::FloatOutput *buzzer_output_{nullptr};
  time::RealTimeClock *time_{nullptr};

  float read_sensor_or_nan_(const sensor::Sensor *sensor) const;
  bool read_binary_sensor_or_false_(const binary_sensor::BinarySensor *sensor) const;
  std::string read_text_sensor_or_empty_(const text_sensor::TextSensor *sensor) const;

  uint32_t beep_started_ms_{0};
  uint32_t beep_duration_ms_{0};
  float beep_frequency_{0.0f};
  bool beep_active_{false};
  bool prev_btn_a_{false};
  bool prev_btn_b_{false};
  bool prev_btn_c_{false};
  bool btn_a_pressed_event_{false};
  bool btn_b_pressed_event_{false};
  bool btn_c_pressed_event_{false};
  bool btn_a_released_event_{false};
  bool btn_b_released_event_{false};
  bool btn_c_released_event_{false};
  bool enable_display_{false};
  bool display_ready_{false};

  HalDisplayConfig display_config_{};
  std::unique_ptr<LGFX_Device> display_{};
  std::unique_ptr<LGFX_SpriteFx> canvas_{};
  GuiSnapshot snapshot_{};
};

}  // namespace stamplc_m5gfx
}  // namespace esphome
