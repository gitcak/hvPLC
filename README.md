# HVPLC StamPLC Humidifier Controller

[![CI](https://github.com/gitcak/hvPLC/actions/workflows/ci.yaml/badge.svg)](https://github.com/gitcak/hvPLC/actions/workflows/ci.yaml)
[![Build Firmware Artifacts](https://github.com/gitcak/hvPLC/actions/workflows/build-factory.yaml/badge.svg)](https://github.com/gitcak/hvPLC/actions/workflows/build-factory.yaml)

ESPHome firmware + Home Assistant packages for controlling a whole-home humidifier from an M5Stack StamPLC (ESP32-S3).

## What This Repo Does

- Device-side safety and relay enforcement in ESPHome.
- 4-page LVGL GUI on the StamPLC display.
- Home Assistant policy logic, dashboards, and notifications.
- Build pipelines for `firmware.factory.bin` and `firmware.ota.bin`.

## Project Layout

```text
esphome/
  stamplc_humidifier.yaml
  secrets.example.yaml
  packages/
    hardware.yaml
    safety.yaml
    ha_sensors.yaml
    gui.yaml
homeassistant/
  packages/hvplc_humidifier/
  dashboards/hvplc_humidifier_dashboard.yaml
docs/
tools/
  build_factory.sh
  load_ha_entities.ps1
```

## Quick Start (ESPHome Style)

### 1. Build Firmware

Option A: local build (Podman)

```bash
./tools/build_factory.sh
```

Output is written to `artifacts/`:
- `*.factory.bin` for first USB flash
- `*.ota.bin` for OTA updates

Option B: GitHub Actions

- Run `Build Firmware Artifacts` workflow manually, or push a `v*` tag.
- On tag push (`v*`), a GitHub Release is created with `firmware.ota.bin` and `firmware.md5` so devices can OTA from URL.
- Download artifacts:
  - `stamplc-humidifier-factory`
  - `stamplc-humidifier-ota`
  - `stamplc-humidifier-manifest`
  - `stamplc-humidifier-pairing` (contains `api_encryption_key` required by HA adopt flow)

### 2. First Flash (Factory)

- Open ESPHome Web and install the `*.factory.bin`.
- Use Improv Serial / Wi-Fi provisioning in ESPHome Web.

### 3. Add to Home Assistant

- Adopt the device in ESPHome integration.
- If prompted for the encryption key, open `pairing-info.txt` from the `stamplc-humidifier-pairing` artifact and use `api_encryption_key`.
- Copy `homeassistant/packages/hvplc_humidifier/*.yaml` into your HA `packages/` directory.
- Ensure `configuration.yaml` has:

```yaml
homeassistant:
  packages: !include_dir_named packages
```

- Import `homeassistant/dashboards/hvplc_humidifier_dashboard.yaml`.
- Replace placeholder entities in `homeassistant/packages/hvplc_humidifier/01_sensors.yaml`.

## ESPHome Notes

- Main config: `esphome/stamplc_humidifier.yaml`
- External components: `github://beormund/esphome-m5stamplc@main`
- Wi-Fi provisioning uses `improv_serial`.
- API encryption is enabled.
- **OTA via HTTP**: Device can pull firmware from a URL (e.g. GitHub Releases). In `esphome/secrets.yaml` set `ota_firmware_base_url` to the base download URL (e.g. `https://github.com/YOUR_ORG/hvPLC/releases/latest/download`). Then use the **Install OTA from release** button (ESPHome/HA) or call the `ota_install_from_url` script to install the latest release.

## Safety Layers

Device safety (ESPHome):
- Relay default OFF on boot.
- API disconnect forces relay OFF.
- Local max runtime limit.
- Local minimum off lockout.

HA safety:
- Runtime hard-stop automation.
- Lockout timer automation.
- Sensor stale/fault shutdown automation.

## Development

Validate locally:

```bash
yamllint -c .yamllint .
```

CI checks (`.github/workflows/ci.yaml`):
- `esphome config esphome/stamplc_humidifier.yaml`
- `yamllint`

## Secrets and Git Hygiene

- Keep real secrets only in `esphome/secrets.yaml` (gitignored).
- Required secrets: `api_key`, `ota_password`, `wifi_ap_password`, `ota_firmware_base_url` (for HTTP OTA; use your repo’s releases URL, e.g. `https://github.com/YOUR_ORG/hvPLC/releases/latest/download`).
- CI/workflows generate ephemeral secrets for validation/build.
- Ignored local outputs:
  - `.cache/`
  - `artifacts/`
  - `esphome/.esphome/`

## Hardware Summary

- Board: M5Stack StamPLC (ESP32-S3, 8MB)
- Display: ST7789V2 240x135
- Expanders: PI4IOE5V6408 (0x43), AW9523 (0x59)
- Sensors: INA226 (0x40), LM75B (0x48), RX8130 RTC

## References

- `docs/architecture.md`
- `docs/commissioning-checklist.md`
- `docs/safety-fmea.md`
- `docs/tuning-guide.md`
