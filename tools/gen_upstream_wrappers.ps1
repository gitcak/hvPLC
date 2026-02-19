$root = "esphome/components/stamplc_m5gfx"
$paths = @(
  "libs/mooncake/src/mooncake.cpp",
  "libs/mooncake/src/app/app_manager.cpp",
  "libs/mooncake/src/app/app_register.cpp",
  "libs/smooth_ui_toolkit/src/core/types/types.cpp",
  "libs/smooth_ui_toolkit/src/core/math/math.cpp",
  "libs/smooth_ui_toolkit/src/core/math/color.cpp",
  "libs/smooth_ui_toolkit/src/core/math/graphic.cpp",
  "libs/smooth_ui_toolkit/src/core/easing_path/easing_path.cpp",
  "libs/smooth_ui_toolkit/src/core/smooth_drag/smooth_drag.cpp",
  "libs/smooth_ui_toolkit/src/core/transition/transition.cpp",
  "libs/smooth_ui_toolkit/src/core/transition2d/transition2d.cpp",
  "libs/smooth_ui_toolkit/src/core/transition3d/transition3d.cpp",
  "libs/smooth_ui_toolkit/src/core/transition4d/transition4d.cpp",
  "libs/smooth_ui_toolkit/src/misc/water_wave_generator/water_wave_generator.cpp",
  "libs/smooth_ui_toolkit/src/select_menu/smooth_selector/smooth_selector.cpp",
  "libs/smooth_ui_toolkit/src/select_menu/smooth_options/smooth_options.cpp",
  "libs/smooth_ui_toolkit/src/widgets/base/base.cpp",
  "libs/smooth_ui_toolkit/src/widgets/smooth_widget/smooth_widget.cpp",
  "libs/smooth_ui_toolkit/src/widgets/button/button.cpp",
  "libs/smooth_ui_toolkit/src/widgets/selector/base/selector.cpp",
  "libs/smooth_ui_toolkit/src/widgets/selector/smooth_selector/smooth_selector.cpp",
  "libs/smooth_ui_toolkit/src/chart/smooth_line_chart/smooth_line_chart.cpp",
  "reference_upstream/app/hal/utils/lgfx_fx/lgfx_fx.cpp",
  "reference_upstream/app/hal/hal.cpp",
  "assets/assets.cpp",
  "reference_upstream/app/shared/shared.cpp",
  "utils/system/inputs/button/m5unified/Button_Class.cpp",
  "utils/system/inputs/button/button.cpp",
  "app_launcher/view/view.cpp",
  "app_launcher/app_launcher.cpp",
  "app_home/app_home.cpp",
  "reference_upstream/app/app.cpp"
)

Get-ChildItem -Path $root -Filter "upstream_src_*.cpp" -ErrorAction SilentlyContinue | Remove-Item -Force
for ($i = 0; $i -lt $paths.Count; $i++) {
  $name = "upstream_src_{0:D2}.cpp" -f ($i + 1)
  $content = "#include `"$($paths[$i])`""
  Set-Content -Path (Join-Path $root $name) -Value $content
}
