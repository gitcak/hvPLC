import esphome.codegen as cg
import esphome.config_validation as cv
from pathlib import Path
from esphome.components import binary_sensor, number, output, sensor, switch, text_sensor, time
from esphome.const import CONF_ID, CONF_TIME_ID

AUTO_LOAD = ["sensor", "binary_sensor", "switch", "number", "text_sensor"]
DEPENDENCIES = []
CONF_ENABLE_DISPLAY_DRIVER = "enable_display_driver"
CONF_DISPLAY_INVERT = "display_invert"

CONF_INDOOR_HUMIDITY_SENSOR = "indoor_humidity_sensor"
CONF_TARGET_HUMIDITY_SENSOR = "target_humidity_sensor"
CONF_OUTDOOR_TEMP_SENSOR = "outdoor_temp_sensor"
CONF_HOURLY_RUNTIME_SENSOR = "hourly_runtime_sensor"
CONF_BOARD_TEMP_SENSOR = "board_temp_sensor"
CONF_BUS_VOLTAGE_SENSOR = "bus_voltage_sensor"
CONF_WIFI_SIGNAL_SENSOR = "wifi_signal_sensor"
CONF_UPTIME_SENSOR = "uptime_sensor"

CONF_HUMIDITY_DEMAND_BINARY_SENSOR = "humidity_demand_binary_sensor"
CONF_INTERLOCK_OK_BINARY_SENSOR = "interlock_ok_binary_sensor"
CONF_SENSORS_HEALTHY_BINARY_SENSOR = "sensors_healthy_binary_sensor"
CONF_RUNTIME_CAP_OK_BINARY_SENSOR = "runtime_cap_ok_binary_sensor"
CONF_RUNTIME_TRIP_BINARY_SENSOR = "runtime_trip_binary_sensor"
CONF_MIN_OFF_LOCKOUT_BINARY_SENSOR = "min_off_lockout_binary_sensor"
CONF_API_CONNECTED_BINARY_SENSOR = "api_connected_binary_sensor"
CONF_BTN_A_BINARY_SENSOR = "btn_a_binary_sensor"
CONF_BTN_B_BINARY_SENSOR = "btn_b_binary_sensor"
CONF_BTN_C_BINARY_SENSOR = "btn_c_binary_sensor"

CONF_MODE_TEXT_SENSOR = "mode_text_sensor"
CONF_REASON_TEXT_SENSOR = "reason_text_sensor"

CONF_HUMIDIFIER_RELAY_SWITCH = "humidifier_relay_switch"
CONF_LCD_BACKLIGHT_SWITCH = "lcd_backlight_switch"

CONF_MAX_RUNTIME_NUMBER = "max_runtime_number"
CONF_MIN_OFF_LOCKOUT_NUMBER = "min_off_lockout_number"
CONF_BUZZER_OUTPUT = "buzzer_output"

stamplc_m5gfx_ns = cg.esphome_ns.namespace("stamplc_m5gfx")
StamplcM5GfxComponent = stamplc_m5gfx_ns.class_("StamplcM5GfxComponent", cg.Component)
COMPONENT_DIR = str(Path(__file__).resolve().parent).replace("\\", "/")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StamplcM5GfxComponent),
        cv.Optional(CONF_ENABLE_DISPLAY_DRIVER, default=False): cv.boolean,
        cv.Optional(CONF_DISPLAY_INVERT, default=False): cv.boolean,
        cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional(CONF_INDOOR_HUMIDITY_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_TARGET_HUMIDITY_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_OUTDOOR_TEMP_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_HOURLY_RUNTIME_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_BOARD_TEMP_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_BUS_VOLTAGE_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_WIFI_SIGNAL_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_UPTIME_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_HUMIDITY_DEMAND_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_INTERLOCK_OK_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_SENSORS_HEALTHY_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_RUNTIME_CAP_OK_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_RUNTIME_TRIP_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_MIN_OFF_LOCKOUT_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_API_CONNECTED_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_BTN_A_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_BTN_B_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_BTN_C_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_MODE_TEXT_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_REASON_TEXT_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_HUMIDIFIER_RELAY_SWITCH): cv.use_id(switch.Switch),
        cv.Optional(CONF_LCD_BACKLIGHT_SWITCH): cv.use_id(switch.Switch),
        cv.Optional(CONF_MAX_RUNTIME_NUMBER): cv.use_id(number.Number),
        cv.Optional(CONF_MIN_OFF_LOCKOUT_NUMBER): cv.use_id(number.Number),
        cv.Optional(CONF_BUZZER_OUTPUT): cv.use_id(output.FloatOutput),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add_library("m5stack/M5GFX", "0.2.19")
    cg.add_build_flag(f"-I{COMPONENT_DIR}")
    cg.add_build_flag(f"-I{COMPONENT_DIR}/reference_upstream/app")
    cg.add_build_flag(f"-I{COMPONENT_DIR}/libs/mooncake/src")
    cg.add_build_flag(f"-I{COMPONENT_DIR}/libs/mooncake/src/spdlog/include")
    cg.add_build_flag(f"-I{COMPONENT_DIR}/libs/smooth_ui_toolkit/src")
    cg.add(var.set_enable_display_driver(config[CONF_ENABLE_DISPLAY_DRIVER]))
    cg.add(var.set_display_invert(config[CONF_DISPLAY_INVERT]))

    if CONF_TIME_ID in config:
        time_comp = await cg.get_variable(config[CONF_TIME_ID])
        cg.add(var.set_time(time_comp))

    if CONF_INDOOR_HUMIDITY_SENSOR in config:
        sens = await cg.get_variable(config[CONF_INDOOR_HUMIDITY_SENSOR])
        cg.add(var.set_indoor_humidity_sensor(sens))
    if CONF_TARGET_HUMIDITY_SENSOR in config:
        sens = await cg.get_variable(config[CONF_TARGET_HUMIDITY_SENSOR])
        cg.add(var.set_target_humidity_sensor(sens))
    if CONF_OUTDOOR_TEMP_SENSOR in config:
        sens = await cg.get_variable(config[CONF_OUTDOOR_TEMP_SENSOR])
        cg.add(var.set_outdoor_temp_sensor(sens))
    if CONF_HOURLY_RUNTIME_SENSOR in config:
        sens = await cg.get_variable(config[CONF_HOURLY_RUNTIME_SENSOR])
        cg.add(var.set_hourly_runtime_sensor(sens))
    if CONF_BOARD_TEMP_SENSOR in config:
        sens = await cg.get_variable(config[CONF_BOARD_TEMP_SENSOR])
        cg.add(var.set_board_temp_sensor(sens))
    if CONF_BUS_VOLTAGE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_BUS_VOLTAGE_SENSOR])
        cg.add(var.set_bus_voltage_sensor(sens))
    if CONF_WIFI_SIGNAL_SENSOR in config:
        sens = await cg.get_variable(config[CONF_WIFI_SIGNAL_SENSOR])
        cg.add(var.set_wifi_signal_sensor(sens))
    if CONF_UPTIME_SENSOR in config:
        sens = await cg.get_variable(config[CONF_UPTIME_SENSOR])
        cg.add(var.set_uptime_sensor(sens))

    if CONF_HUMIDITY_DEMAND_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_HUMIDITY_DEMAND_BINARY_SENSOR])
        cg.add(var.set_humidity_demand_binary_sensor(bin_sens))
    if CONF_INTERLOCK_OK_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_INTERLOCK_OK_BINARY_SENSOR])
        cg.add(var.set_interlock_ok_binary_sensor(bin_sens))
    if CONF_SENSORS_HEALTHY_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_SENSORS_HEALTHY_BINARY_SENSOR])
        cg.add(var.set_sensors_healthy_binary_sensor(bin_sens))
    if CONF_RUNTIME_CAP_OK_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_RUNTIME_CAP_OK_BINARY_SENSOR])
        cg.add(var.set_runtime_cap_ok_binary_sensor(bin_sens))
    if CONF_RUNTIME_TRIP_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_RUNTIME_TRIP_BINARY_SENSOR])
        cg.add(var.set_runtime_trip_binary_sensor(bin_sens))
    if CONF_MIN_OFF_LOCKOUT_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_MIN_OFF_LOCKOUT_BINARY_SENSOR])
        cg.add(var.set_min_off_lockout_binary_sensor(bin_sens))
    if CONF_API_CONNECTED_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_API_CONNECTED_BINARY_SENSOR])
        cg.add(var.set_api_connected_binary_sensor(bin_sens))
    if CONF_BTN_A_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_BTN_A_BINARY_SENSOR])
        cg.add(var.set_btn_a_binary_sensor(bin_sens))
    if CONF_BTN_B_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_BTN_B_BINARY_SENSOR])
        cg.add(var.set_btn_b_binary_sensor(bin_sens))
    if CONF_BTN_C_BINARY_SENSOR in config:
        bin_sens = await cg.get_variable(config[CONF_BTN_C_BINARY_SENSOR])
        cg.add(var.set_btn_c_binary_sensor(bin_sens))

    if CONF_MODE_TEXT_SENSOR in config:
        txt_sens = await cg.get_variable(config[CONF_MODE_TEXT_SENSOR])
        cg.add(var.set_mode_text_sensor(txt_sens))
    if CONF_REASON_TEXT_SENSOR in config:
        txt_sens = await cg.get_variable(config[CONF_REASON_TEXT_SENSOR])
        cg.add(var.set_reason_text_sensor(txt_sens))

    if CONF_HUMIDIFIER_RELAY_SWITCH in config:
        relay_sw = await cg.get_variable(config[CONF_HUMIDIFIER_RELAY_SWITCH])
        cg.add(var.set_humidifier_relay_switch(relay_sw))
    if CONF_LCD_BACKLIGHT_SWITCH in config:
        bl_sw = await cg.get_variable(config[CONF_LCD_BACKLIGHT_SWITCH])
        cg.add(var.set_lcd_backlight_switch(bl_sw))

    if CONF_MAX_RUNTIME_NUMBER in config:
        num = await cg.get_variable(config[CONF_MAX_RUNTIME_NUMBER])
        cg.add(var.set_max_runtime_number(num))
    if CONF_MIN_OFF_LOCKOUT_NUMBER in config:
        num = await cg.get_variable(config[CONF_MIN_OFF_LOCKOUT_NUMBER])
        cg.add(var.set_min_off_lockout_number(num))
    if CONF_BUZZER_OUTPUT in config:
        out = await cg.get_variable(config[CONF_BUZZER_OUTPUT])
        cg.add(var.set_buzzer_output(out))
