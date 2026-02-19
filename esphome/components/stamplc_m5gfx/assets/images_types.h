#pragma once
#include <cstdint>

struct ImagePool_t {
  struct AppLauncher_t {
    const uint16_t *icon_app_humidifier;
    const uint16_t *icon_app_dashboard;
    const uint16_t *icon_app_ezdata;
    const uint16_t *icon_app_log_monitor;
    const uint16_t *icon_app_setting;
    const uint16_t *icon_app_timer_relay;
    const uint16_t *icon_app_trigger_relay;
  } AppLauncher;

  struct AppDashboard_t {
    const uint16_t *icon_wifi_ok;
    const uint16_t *icon_wifi_not_ok;
    const uint16_t *icon_ezdata_ok;
    const uint16_t *icon_ezdata_not_ok;
    const uint16_t *tag_console;
    const uint16_t *tag_io;
  } AppDashboard;

  struct AppTriggerRelay_t {
    const uint16_t *icon_load;
    const uint16_t *icon_save;
  } AppTriggerRelay;
};
