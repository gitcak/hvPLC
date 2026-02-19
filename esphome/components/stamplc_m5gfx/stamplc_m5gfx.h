#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/component.h"
#include "hal_esphome.h"
#include "mooncake_runtime.h"

namespace esphome {
namespace stamplc_m5gfx {

class StamplcM5GfxComponent : public Component {
 public:
  void set_enable_display_driver(bool value) { this->enable_display_driver_ = value; }
  void set_display_invert(bool value) { this->display_invert_ = value; }
  void set_indoor_humidity_sensor(sensor::Sensor *value) { this->indoor_humidity_sensor_ = value; }
  void set_target_humidity_sensor(sensor::Sensor *value) { this->target_humidity_sensor_ = value; }
  void set_outdoor_temp_sensor(sensor::Sensor *value) { this->outdoor_temp_sensor_ = value; }
  void set_hourly_runtime_sensor(sensor::Sensor *value) { this->hourly_runtime_sensor_ = value; }
  void set_board_temp_sensor(sensor::Sensor *value) { this->board_temp_sensor_ = value; }
  void set_bus_voltage_sensor(sensor::Sensor *value) { this->bus_voltage_sensor_ = value; }
  void set_wifi_signal_sensor(sensor::Sensor *value) { this->wifi_signal_sensor_ = value; }
  void set_uptime_sensor(sensor::Sensor *value) { this->uptime_sensor_ = value; }

  void set_humidity_demand_binary_sensor(binary_sensor::BinarySensor *value) { this->humidity_demand_binary_sensor_ = value; }
  void set_interlock_ok_binary_sensor(binary_sensor::BinarySensor *value) { this->interlock_ok_binary_sensor_ = value; }
  void set_sensors_healthy_binary_sensor(binary_sensor::BinarySensor *value) { this->sensors_healthy_binary_sensor_ = value; }
  void set_runtime_cap_ok_binary_sensor(binary_sensor::BinarySensor *value) { this->runtime_cap_ok_binary_sensor_ = value; }
  void set_runtime_trip_binary_sensor(binary_sensor::BinarySensor *value) { this->runtime_trip_binary_sensor_ = value; }
  void set_min_off_lockout_binary_sensor(binary_sensor::BinarySensor *value) { this->min_off_lockout_binary_sensor_ = value; }
  void set_api_connected_binary_sensor(binary_sensor::BinarySensor *value) { this->api_connected_binary_sensor_ = value; }
  void set_btn_a_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_a_binary_sensor_ = value; }
  void set_btn_b_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_b_binary_sensor_ = value; }
  void set_btn_c_binary_sensor(binary_sensor::BinarySensor *value) { this->btn_c_binary_sensor_ = value; }

  void set_mode_text_sensor(text_sensor::TextSensor *value) { this->mode_text_sensor_ = value; }
  void set_reason_text_sensor(text_sensor::TextSensor *value) { this->reason_text_sensor_ = value; }

  void set_humidifier_relay_switch(switch_::Switch *value) { this->humidifier_relay_switch_ = value; }
  void set_lcd_backlight_switch(switch_::Switch *value) { this->lcd_backlight_switch_ = value; }

  void set_max_runtime_number(number::Number *value) { this->max_runtime_number_ = value; }
  void set_min_off_lockout_number(number::Number *value) { this->min_off_lockout_number_ = value; }
  void set_buzzer_output(output::FloatOutput *value) { this->buzzer_output_ = value; }
  void set_time(time::RealTimeClock *value) { this->time_ = value; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

 private:
  void bind_hal_();
  void render_diagnostics_();

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

  HalEsphome hal_{};
  bool enable_display_driver_{false};
  bool display_invert_{false};
  bool initialized_{false};
  bool mooncake_started_{false};
  uint32_t last_render_ms_{0};
};

}  // namespace stamplc_m5gfx
}  // namespace esphome
