import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

from . import HotTubMonitor, hottub_ns

CONF_HOTTUB_MONITOR_ID = "hottub_monitor_id"

CONF_ERROR_MESSAGES = "error_messages"
CONF_MEASURED_TEMPERATURE = "measured_temperature"
CONF_LIGHT_STATUS = "light_status"

# unique wrapper-id keys (important!)
CONF_ERROR_MESSAGES_WRAPPER_ID = "error_messages_wrapper_id"
CONF_MEASURED_TEMPERATURE_WRAPPER_ID = "measured_temperature_wrapper_id"
CONF_LIGHT_STATUS_WRAPPER_ID = "light_status_wrapper_id"

UPDATE_MS = 1000

ErrorMessagesSensor = hottub_ns.class_("ErrorMessagesSensor", cg.PollingComponent)
MeasuredTemperatureSensor = hottub_ns.class_("MeasuredTemperatureSensor", cg.PollingComponent)
LightStatusSensor = hottub_ns.class_("LightStatusSensor", cg.PollingComponent)

def _wrapped_sensor_schema(wrapper_cls, wrapper_id_key):
    # This creates a normal ESPHome sensor (with its own id handled by ESPHome),
    # plus a separate, uniquely-named ID for our wrapper component.
    return sensor.sensor_schema().extend(
        {
            cv.GenerateID(wrapper_id_key): cv.declare_id(wrapper_cls),
        }
    )

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HOTTUB_MONITOR_ID): cv.use_id(HotTubMonitor),

        cv.Optional(CONF_ERROR_MESSAGES): _wrapped_sensor_schema(
            ErrorMessagesSensor, CONF_ERROR_MESSAGES_WRAPPER_ID
        ),
        cv.Optional(CONF_MEASURED_TEMPERATURE): _wrapped_sensor_schema(
            MeasuredTemperatureSensor, CONF_MEASURED_TEMPERATURE_WRAPPER_ID
        ),
        cv.Optional(CONF_LIGHT_STATUS): _wrapped_sensor_schema(
            LightStatusSensor, CONF_LIGHT_STATUS_WRAPPER_ID
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOTTUB_MONITOR_ID])

    if CONF_ERROR_MESSAGES in config:
        sens = await sensor.new_sensor(config[CONF_ERROR_MESSAGES])
        wrapper = cg.new_Pvariable(
            config[CONF_ERROR_MESSAGES][CONF_ERROR_MESSAGES_WRAPPER_ID], parent, UPDATE_MS
        )
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_ERROR_MESSAGES])

    if CONF_MEASURED_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MEASURED_TEMPERATURE])
        wrapper = cg.new_Pvariable(
            config[CONF_MEASURED_TEMPERATURE][CONF_MEASURED_TEMPERATURE_WRAPPER_ID],
            parent,
            UPDATE_MS,
        )
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_MEASURED_TEMPERATURE])

    if CONF_LIGHT_STATUS in config:
        sens = await sensor.new_sensor(config[CONF_LIGHT_STATUS])
        wrapper = cg.new_Pvariable(
            config[CONF_LIGHT_STATUS][CONF_LIGHT_STATUS_WRAPPER_ID], parent, UPDATE_MS
        )
        cg.add(wrapper.set_sensor(sens))
        await cg.register_component(wrapper, config[CONF_LIGHT_STATUS])