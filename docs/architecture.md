# Architecture

## Overview

The humidifier controller is split into policy and safety planes:

- Policy plane: Home Assistant packages compute target humidity, evaluate demand, check permits, and command relay state.
- Safety plane: ESPHome and HA safety automations enforce fail-off behavior, max runtime, and lockout windows.

## Control Flow

1. Indoor RH sensors are averaged into `sensor.hvplc_indoor_humidity_mean`.
2. Indoor temperature sensors are averaged into `sensor.hvplc_indoor_temperature_mean`.
3. Outdoor temperature and humidity are selected from dropdown-configured entities and exposed as:
   - `sensor.hvplc_outdoor_temperature`
   - `sensor.hvplc_outdoor_humidity`
4. Target RH is calculated as `sensor.hvplc_target_humidity` using:
   - `manual` mode: fixed user target
   - `auto_curve` mode: outdoor-compensated target, clamped by min/max
5. Demand is derived by hysteresis in `binary_sensor.hvplc_humidity_demand`.
6. Permit checks include:
   - `input_boolean.hvplc_humidifier_enabled`
   - `binary_sensor.hvplc_hvac_interlock_ok`
   - `binary_sensor.hvplc_runtime_cap_ok`
   - `timer.hvplc_humidifier_min_off_lockout` idle
   - `input_boolean.hvplc_runtime_trip` off
   - `binary_sensor.hvplc_critical_sensors_healthy` on
7. `automation.hvplc_controller_evaluate_and_actuate` commands `switch.stamplc_humidifier_relay`.

## Safety Layers

### Device (ESPHome)

- Relay restore mode is always off.
- Relay is forced off when API client disconnects.
- Local min-off lockout prevents immediate restart.
- Local max continuous runtime trip forces off.

### Home Assistant

- Runtime hard-stop automation enforces configured continuous max.
- Min-off lockout timer starts every relay off transition.
- Sensor fault/stale data automation forces fail-off.
- Persistent notifications report interlock blocks, sensor faults, and runtime trips.

## On-Device GUI (LVGL)

The StamPLC's built-in 1.14" ST7789V2 LCD (240x135) runs an LVGL-based GUI with 4 pages:

1. **Home**: Indoor/target humidity, relay state, demand, interlock, mode, last reason
2. **Safety**: Runtime trip, lockout status, hourly runtime, sensor health, runtime cap
3. **Settings**: Mode toggle, max runtime, min off lockout (editable via buttons)
4. **Device Info**: WiFi signal, uptime, API status, board temperature, bus voltage

Navigation uses 3 physical buttons: A (prev/left), B (action/select), C (next/right).

HA entity states are imported via `homeassistant` platform sensors and bound to LVGL labels. Settings changes push back to HA via API service calls.

## Required Integrations

- `template`
- `history_stats`
- `min_max`
- `timer`
- `input_*` helpers
- ESPHome integration with StamPLC device

## Customization Points

- Use helper dropdowns in `00_helpers.yaml` and the dashboard to select:
  - indoor humidity sources (`input_select.hvplc_indoor_humidity_entity_*`)
  - indoor temperature sources (`input_select.hvplc_indoor_temperature_entity_*`)
  - outdoor temperature/humidity sources
  - HVAC climate entity for interlock
- Add or remove selectable entities by editing `input_select` options in `00_helpers.yaml`.
- AW9523 relay channel is pin 0 at address 0x59 (verified from [Beormund/esphome-m5stamplc](https://github.com/Beormund/esphome-m5stamplc)).

