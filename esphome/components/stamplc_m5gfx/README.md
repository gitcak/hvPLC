# stamplc_m5gfx setup scaffold

This directory is an inactive external-component scaffold for the M5GFX/Mooncake migration.

## Current status
- `__init__.py` registers a compile-safe ESPHome component.
- `stamplc_m5gfx.*` provides skeleton setup/loop hooks.
- `hal_esphome.*` and `app_home/*` are placeholders for bridge + HOME app.
- `libs/` is reserved for vendored GUI dependencies.

## Next setup step
Run:

```powershell
tools/import_m5stamplc_scaffold.ps1
tools/setup_m5gfx_libs.ps1
```

The first script copies the local UserDemo scaffold into `reference_upstream/`.
The second script pulls pinned versions of the GUI dependencies into `libs/`.