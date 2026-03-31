import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID  # <-- add this

from . import HotTubMonitor, hottub_ns

CONF_HOTTUB_MONITOR_ID = "hottub_monitor_id"

CONF_ERROR_MESSAGES = "error_messages"
CONF_MEASURED_TEMPERATURE = "measured_temperature"
CONF_LIGHT_STATUS = "light_status"

UPDATE_MS = 1000

ErrorMessagesSensor = hottub_ns.class_("ErrorMessagesSensor", cg.PollingComponent)
MeasuredTemperatureSensor = hottub_ns.class_("MeasuredTemperatureSensor", cg.PollingComponent)
LightStatusSensor = hottub_ns.class_("LightStatusSensor", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HOTTUB_MONITOR_ID): cv.use_id(HotTubMonitor),

        cv.Optional(CONF_ERROR_MESSAGES): sensor.sensor_schema().extend(
            {cv.GenerateID(): cv.declare_id(ErrorMessagesSensor)}
        ),
        cv.Optional(CONF_MEASURED_TEMPERATURE): sensor.sensor_schema().extend(
            {cv.GenerateID(): cv.declare_id(MeasuredTemperatureSensor)}
        ),
        cv.Optional(CONF_LIGHT_STATUS): sensor.sensor_schema().extend(
            {cv.GenerateID(): cv.declare_id(LightStatusSensor)}
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOTTUB_MONITOR_ID])

    if CONF_ERROR_MESSAGES in config:
        sens = await sensor.new_sensor(config[CONF_ERROR_MESSAGES])
        wrapper = cg.new_Pvariable(config[CONF_ERROR_MESSAGES][CONF_ID], parent, UPDATE_MS)
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_ERROR_MESSAGES])

    if CONF_MEASURED_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MEASURED_TEMPERATURE])
        wrapper = cg.new_Pvariable(config[CONF_MEASURED_TEMPERATURE][CONF_ID], parent, UPDATE_MS)
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_MEASURED_TEMPERATURE])

    if CONF_LIGHT_STATUS in config:
        sens = await sensor.new_sensor(config[CONF_LIGHT_STATUS])
        wrapper = cg.new_Pvariable(config[CONF_LIGHT_STATUS][CONF_ID], parent, UPDATE_MS)
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_LIGHT_STATUS])