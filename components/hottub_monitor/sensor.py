import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

from . import HotTubMonitor, hottub_ns

CONF_HOTTUB_MONITOR_ID = "hottub_monitor_id"

CONF_ERROR_MESSAGES = "error_messages"
CONF_MEASURED_TEMPERATURE = "measured_temperature"
CONF_LIGHT_STATUS = "light_status"

ErrorMessagesSensor = hottub_ns.class_(
    "ErrorMessagesSensor", sensor.Sensor, cg.PollingComponent
)
MeasuredTemperatureSensor = hottub_ns.class_(
    "MeasuredTemperatureSensor", sensor.Sensor, cg.PollingComponent
)
LightStatusSensor = hottub_ns.class_(
    "LightStatusSensor", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HOTTUB_MONITOR_ID): cv.use_id(HotTubMonitor),
        cv.Optional(CONF_ERROR_MESSAGES): sensor.sensor_schema(ErrorMessagesSensor),
        cv.Optional(CONF_MEASURED_TEMPERATURE): sensor.sensor_schema(
            MeasuredTemperatureSensor
        ),
        cv.Optional(CONF_LIGHT_STATUS): sensor.sensor_schema(LightStatusSensor),
    }
)

# Poll every 1s like your HA_SERVICE_UPDATE_INTERVAL
UPDATE_MS = 1000


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOTTUB_MONITOR_ID])

    if CONF_ERROR_MESSAGES in config:
        s = await sensor.new_sensor(config[CONF_ERROR_MESSAGES])
        cg.add(cg.new_Pvariable(s.id, parent, UPDATE_MS))

    if CONF_MEASURED_TEMPERATURE in config:
        s = await sensor.new_sensor(config[CONF_MEASURED_TEMPERATURE])
        cg.add(cg.new_Pvariable(s.id, parent, UPDATE_MS))

    if CONF_LIGHT_STATUS in config:
        s = await sensor.new_sensor(config[CONF_LIGHT_STATUS])
        cg.add(cg.new_Pvariable(s.id, parent, UPDATE_MS))