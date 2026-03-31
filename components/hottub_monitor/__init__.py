import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

hottub_ns = cg.esphome_ns.namespace("hottub_monitor")
HotTubMonitor = hottub_ns.class_("HotTubMonitor", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HotTubMonitor),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)