#include "hal_esphome.h"

#include <utility>

#include "lgfx/v1/panel/Panel_ST7789.hpp"
#include "lgfx/v1/platforms/esp32/Bus_SPI.hpp"
#include "esphome/core/log.h"

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace esphome {
namespace stamplc_m5gfx {

static const char *const TAG = "stamplc_m5gfx.hal";

class StamplcLgfx final : public lgfx::LGFX_Device {
 public:
  explicit StamplcLgfx(const HalDisplayConfig &cfg) : cfg_(cfg) {
    auto bus_cfg = this->bus_.config();
    bus_cfg.freq_write = 40000000;
    bus_cfg.freq_read = 16000000;
    bus_cfg.pin_sclk = this->cfg_.pin_clk;
    bus_cfg.pin_mosi = this->cfg_.pin_mosi;
    bus_cfg.pin_miso = this->cfg_.pin_miso;
    bus_cfg.pin_dc = this->cfg_.pin_dc;
    bus_cfg.spi_mode = 0;
    bus_cfg.spi_3wire = false;
    bus_cfg.use_lock = true;
    this->bus_.config(bus_cfg);
    this->panel_.setBus(&this->bus_);

    auto panel_cfg = this->panel_.config();
    panel_cfg.pin_cs = this->cfg_.pin_cs;
    panel_cfg.pin_rst = -1;  // GPIO3 is owned by power_supply for I2C expanders
    panel_cfg.pin_busy = -1;
    panel_cfg.memory_width = 240;
    panel_cfg.memory_height = 320;
    panel_cfg.panel_width = this->cfg_.panel_width;
    panel_cfg.panel_height = this->cfg_.panel_height;
    panel_cfg.offset_x = this->cfg_.offset_x;
    panel_cfg.offset_y = this->cfg_.offset_y;
    panel_cfg.offset_rotation = 0;
    panel_cfg.invert = this->cfg_.invert;
    panel_cfg.rgb_order = false;
    panel_cfg.bus_shared = true;
    this->panel_.config(panel_cfg);

    this->setPanel(&this->panel_);
  }

 private:
  HalDisplayConfig cfg_;
  lgfx::Bus_SPI bus_{};
  lgfx::Panel_ST7789 panel_{};
};

void HalEsphome::init() {
  ESP_LOGI(TAG, "HAL init (%s)", this->type());
  ESP_LOGI(TAG, "Display config: ST7789V2 native=%ux%u offset(%u,%u) dc=%d cs=%d invert=%s rotation=1",
           this->display_config_.panel_width, this->display_config_.panel_height, this->display_config_.offset_x,
           this->display_config_.offset_y, this->display_config_.pin_dc, this->display_config_.pin_cs,
           this->display_config_.invert ? "true" : "false");

  if (this->enable_display_) {
    this->display_ = std::make_unique<StamplcLgfx>(this->display_config_);
    if (!this->display_->init()) {
      ESP_LOGE(TAG, "Display initialization failed");
      this->display_.reset();
    } else {
      this->display_->setColorDepth(lgfx::rgb565_2Byte);
      this->display_->setRotation(1);

      this->canvas_ = std::make_unique<LGFX_SpriteFx>(this->display_.get());
      this->canvas_->setColorDepth(lgfx::rgb565_2Byte);
      this->canvas_->setSwapBytes(true);
      const uint16_t canvas_w = this->display_->width();
      const uint16_t canvas_h = this->display_->height();
      if (this->canvas_->createSprite(canvas_w, canvas_h) == nullptr) {
        ESP_LOGE(TAG, "Canvas allocation failed (%ux%u)", canvas_w, canvas_h);
        this->canvas_.reset();
        this->display_.reset();
      } else {
        this->canvas_->fillScreen(TFT_BLACK);
        this->canvas_update();
        this->display_ready_ = true;
      }
    }
  } else {
    ESP_LOGI(TAG, "Display initialization skipped (enable_display_driver=false)");
  }

  this->snapshot_.mode.clear();
  this->snapshot_.reason.clear();
}

void HalEsphome::tick() {
  this->btn_a_pressed_event_ = false;
  this->btn_b_pressed_event_ = false;
  this->btn_c_pressed_event_ = false;
  this->btn_a_released_event_ = false;
  this->btn_b_released_event_ = false;
  this->btn_c_released_event_ = false;

  this->snapshot_.indoor_humidity = this->read_sensor_or_nan_(this->indoor_humidity_sensor_);
  this->snapshot_.target_humidity = this->read_sensor_or_nan_(this->target_humidity_sensor_);
  this->snapshot_.outdoor_temp = this->read_sensor_or_nan_(this->outdoor_temp_sensor_);
  this->snapshot_.hourly_runtime_min = this->read_sensor_or_nan_(this->hourly_runtime_sensor_);
  this->snapshot_.board_temp_c = this->read_sensor_or_nan_(this->board_temp_sensor_);
  this->snapshot_.bus_voltage_v = this->read_sensor_or_nan_(this->bus_voltage_sensor_);
  this->snapshot_.wifi_signal_dbm = this->read_sensor_or_nan_(this->wifi_signal_sensor_);
  this->snapshot_.uptime_s = this->read_sensor_or_nan_(this->uptime_sensor_);
  this->snapshot_.max_runtime_min = this->max_runtime_number_ != nullptr ? this->max_runtime_number_->state : NAN;
  this->snapshot_.min_off_lockout_min =
      this->min_off_lockout_number_ != nullptr ? this->min_off_lockout_number_->state : NAN;

  this->snapshot_.humidifier_relay_on = this->humidifier_relay_switch_ != nullptr && this->humidifier_relay_switch_->state;
  this->snapshot_.humidity_demand = this->read_binary_sensor_or_false_(this->humidity_demand_binary_sensor_);
  this->snapshot_.interlock_ok = this->read_binary_sensor_or_false_(this->interlock_ok_binary_sensor_);
  this->snapshot_.sensors_healthy = this->read_binary_sensor_or_false_(this->sensors_healthy_binary_sensor_);
  this->snapshot_.runtime_cap_ok = this->read_binary_sensor_or_false_(this->runtime_cap_ok_binary_sensor_);
  this->snapshot_.runtime_trip = this->read_binary_sensor_or_false_(this->runtime_trip_binary_sensor_);
  this->snapshot_.min_off_lockout_active = this->read_binary_sensor_or_false_(this->min_off_lockout_binary_sensor_);
  this->snapshot_.api_connected = this->read_binary_sensor_or_false_(this->api_connected_binary_sensor_);
  this->snapshot_.btn_a_pressed = this->read_binary_sensor_or_false_(this->btn_a_binary_sensor_);
  this->snapshot_.btn_b_pressed = this->read_binary_sensor_or_false_(this->btn_b_binary_sensor_);
  this->snapshot_.btn_c_pressed = this->read_binary_sensor_or_false_(this->btn_c_binary_sensor_);
  this->btn_a_pressed_event_ = this->snapshot_.btn_a_pressed && !this->prev_btn_a_;
  this->btn_b_pressed_event_ = this->snapshot_.btn_b_pressed && !this->prev_btn_b_;
  this->btn_c_pressed_event_ = this->snapshot_.btn_c_pressed && !this->prev_btn_c_;
  this->btn_a_released_event_ = !this->snapshot_.btn_a_pressed && this->prev_btn_a_;
  this->btn_b_released_event_ = !this->snapshot_.btn_b_pressed && this->prev_btn_b_;
  this->btn_c_released_event_ = !this->snapshot_.btn_c_pressed && this->prev_btn_c_;
  this->prev_btn_a_ = this->snapshot_.btn_a_pressed;
  this->prev_btn_b_ = this->snapshot_.btn_b_pressed;
  this->prev_btn_c_ = this->snapshot_.btn_c_pressed;

  this->snapshot_.mode = this->read_text_sensor_or_empty_(this->mode_text_sensor_);
  this->snapshot_.reason = this->read_text_sensor_or_empty_(this->reason_text_sensor_);

  if (this->time_ != nullptr && this->time_->now().is_valid()) {
    this->snapshot_.current_time = this->time_->now().strftime("%H:%M");
  } else {
    this->snapshot_.current_time = "--:--";
  }

  if (this->beep_active_ && this->beep_duration_ms_ != UINT32_MAX) {
    const uint32_t elapsed = this->millis() - this->beep_started_ms_;
    if (elapsed >= this->beep_duration_ms_) {
      this->beep_stop();
    }
  }
}

bool HalEsphome::get_button(HalButton button) const {
  switch (button) {
    case HalButton::BTN_A:
      return this->snapshot_.btn_a_pressed;
    case HalButton::BTN_B:
      return this->snapshot_.btn_b_pressed;
    case HalButton::BTN_C:
      return this->snapshot_.btn_c_pressed;
    default:
      return false;
  }
}

bool HalEsphome::button_pressed_event(HalButton button) const {
  switch (button) {
    case HalButton::BTN_A:
      return this->btn_a_pressed_event_;
    case HalButton::BTN_B:
      return this->btn_b_pressed_event_;
    case HalButton::BTN_C:
      return this->btn_c_pressed_event_;
    default:
      return false;
  }
}

bool HalEsphome::button_released_event(HalButton button) const {
  switch (button) {
    case HalButton::BTN_A:
      return this->btn_a_released_event_;
    case HalButton::BTN_B:
      return this->btn_b_released_event_;
    case HalButton::BTN_C:
      return this->btn_c_released_event_;
    default:
      return false;
  }
}

uint32_t HalEsphome::millis() const { return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL); }

void HalEsphome::delay_ms(uint32_t milliseconds) const { vTaskDelay(pdMS_TO_TICKS(milliseconds)); }

void HalEsphome::beep(float frequency, uint32_t duration_ms) {
  this->beep_frequency_ = frequency;
  this->beep_duration_ms_ = duration_ms;
  this->beep_started_ms_ = this->millis();
  this->beep_active_ = true;

  if (this->buzzer_output_ != nullptr) {
    this->buzzer_output_->set_level(0.5f);
  } else {
    ESP_LOGD(TAG, "Beep output unavailable");
  }
  ESP_LOGD(TAG, "Beep start freq=%.1f duration=%u", frequency, duration_ms);
}

void HalEsphome::beep_stop() {
  if (!this->beep_active_) {
    return;
  }
  this->beep_active_ = false;
  this->beep_frequency_ = 0.0f;
  this->beep_duration_ms_ = 0;
  this->beep_started_ms_ = 0;
  if (this->buzzer_output_ != nullptr) {
    this->buzzer_output_->set_level(0.0f);
  }
  ESP_LOGD(TAG, "Beep stop");
}

void HalEsphome::canvas_update() {
  if (!this->display_ready_ || this->canvas_ == nullptr) {
    return;
  }
  this->canvas_->pushSprite(0, 0);
}

const GuiSnapshot &HalEsphome::snapshot() const { return this->snapshot_; }

float HalEsphome::read_sensor_or_nan_(const sensor::Sensor *sensor) const {
  if (sensor == nullptr || !sensor->has_state()) {
    return NAN;
  }
  return sensor->state;
}

bool HalEsphome::read_binary_sensor_or_false_(const binary_sensor::BinarySensor *sensor) const {
  if (sensor == nullptr || !sensor->has_state()) {
    return false;
  }
  return sensor->state;
}

std::string HalEsphome::read_text_sensor_or_empty_(const text_sensor::TextSensor *sensor) const {
  if (sensor == nullptr || !sensor->has_state()) {
    return {};
  }
  return sensor->state;
}

void HalEsphome::set_backlight(bool state) {
  if (this->lcd_backlight_switch_ != nullptr) {
    if (state) {
      this->lcd_backlight_switch_->turn_on();
    } else {
      this->lcd_backlight_switch_->turn_off();
    }
  }
}

}  // namespace stamplc_m5gfx
}  // namespace esphome
