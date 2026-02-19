# Tuning Guide

## Default Strategy

- Mode: `auto_curve`
- Min RH: `30%`
- Max RH: `40%`
- Curve slope: `0.35`
- Dry tolerance: `2%`
- Wet tolerance: `1%`
- Max continuous runtime: `20 min`
- Min off lockout: `10 min`
- Hourly runtime cap: `35 min`

## Tuning Order

1. Verify sensor quality first (calibration and placement).
2. Tune humidity bounds (`min`/`max`) for comfort and window condensation avoidance.
3. Tune curve slope based on outdoor temperature response.
4. Tune hysteresis (`dry_tolerance` and `wet_tolerance`) to reduce relay chatter.
5. Tune runtime safeguards only after comfort behavior is stable.

## Practical Adjustments

- Home too dry on cold days:
  - Increase `hvplc_humidity_max` by 1-2%
  - Decrease curve slope slightly

- Condensation on windows:
  - Decrease `hvplc_humidity_max`
  - Increase curve slope slightly so target falls faster as outdoor temp drops

- Relay short cycling:
  - Increase `hvplc_dry_tolerance`
  - Increase `hvplc_runtime_min_off_min`

- Not reaching target quickly enough:
  - Reduce `hvplc_runtime_min_off_min` modestly
  - Increase `hvplc_runtime_hourly_cap_min` only if safe for equipment

## Seasonal Notes

- Winter shoulder season often benefits from lower slope than deep winter.
- Re-check settings when weather shifts substantially.
- Keep `manual` mode for troubleshooting only; return to `auto_curve` for regular operation.

