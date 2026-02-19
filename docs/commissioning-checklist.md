# Commissioning Checklist

## Pre-Power Checks

- Confirm humidifier control input is 24 VAC dry-contact compatible.
- Confirm StamPLC relay channel wiring and polarity per furnace/humidifier manual.
- Confirm common/transformer wiring is isolated from PLC logic rails.
- Confirm emergency furnace shutdown access before first live run.

## ESPHome Bring-Up

- Populate `esphome/secrets.yaml` from `secrets.example.yaml`.
- Flash `esphome/stamplc_humidifier.yaml`.
- Confirm entities appear in HA:
  - `switch.stamplc_humidifier_relay`
  - diagnostics (`StamPLC API Connected`, lockout/trip sensors, Wi-Fi signal)
- Toggle relay in HA and verify physical relay click + dry-contact continuity.
- Disconnect HA API (or network) and confirm relay fails off.

## Home Assistant Package Bring-Up

- Ensure packages are enabled (`homeassistant.packages` include).
- Copy package files from `homeassistant/packages/hvplc_humidifier/`.
- Replace placeholders in `01_sensors.yaml`:
  - indoor RH entities
  - thermostat entity for `hvac_action`
- Reload helpers/automations/templates or restart HA.

## Functional Tests

- Set controller enabled, set manual mode and low manual target; verify relay remains off above target.
- Raise manual target above current RH; verify demand and relay behavior.
- Force interlock false (thermostat idle); verify relay turns off even with demand.
- Force relay on and reduce max runtime to trip quickly; verify hard stop and notification.
- Confirm min-off lockout blocks immediate restart.
- Confirm hourly cap behavior by lowering cap and observing `binary_sensor.hvplc_runtime_cap_ok`.

## Dashboard Validation

- Import `homeassistant/dashboards/hvplc_humidifier_dashboard.yaml`.
- Verify all cards resolve entities with no unavailable placeholders.
- Confirm troubleshooting rows update:
  - `input_text.hvplc_last_controller_reason`
  - last-triggered sensors

## Go-Live Gate

- Run 24-hour observation in monitor mode.
- Validate no short-cycling and no condensation risk at windows.
- Lock final tuning values in `input_number` helpers.

