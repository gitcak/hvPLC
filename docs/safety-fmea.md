# Safety FMEA (v1)

| Failure mode | Effect | Detection | Mitigation |
|---|---|---|---|
| HA offline / API disconnected from StamPLC | Relay could remain energized without central policy | ESPHome API connected binary sensor | ESPHome `on_client_disconnected` forces relay off |
| Indoor RH sensor unavailable | Bad demand calculation | `binary_sensor.hvplc_critical_sensors_healthy` | HA safety automation forces relay off |
| Outdoor weather source unavailable | Bad target calculation in auto mode | Outdoor sensor availability logic | Target falls back to midpoint clamp; sensor-fault notification path |
| Thermostat interlock missing/false | Humidifier runs without airflow | `binary_sensor.hvplc_hvac_interlock_ok` | Controller permit blocks relay on; interlock notification |
| Relay stuck ON request loop | Over-humidification / equipment stress | Continuous runtime check | HA hard-stop runtime automation + lockout + runtime trip flag |
| Frequent cycling | Relay wear / unstable humidity control | Relay history and on-time trend | Hysteresis helpers + min-off lockout |
| Excessive hourly runtime | Duct saturation / wear | `sensor.hvplc_relay_on_time_last_hour_minutes` | Runtime cap permit (`binary_sensor.hvplc_runtime_cap_ok`) |
| Invalid pin mapping on StamPLC | Wrong physical channel actuates | Commissioning relay validation | Manual verification required before go-live |

## Residual Risks

- Hardware-level relay weld failure is not detectable in this v1 software scaffold.
- Furnace-specific airflow proofs may require dedicated binary sensors for stronger interlock guarantees.
- Outdoor temperature from weather integration may not match microclimate at the home envelope.

