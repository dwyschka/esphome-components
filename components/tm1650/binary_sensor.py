import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_KEY

CONF_TM1650_ID = "tm1650_id"

tm1650_ns = cg.esphome_ns.namespace("tm1650")
TM1650Display = tm1650_ns.class_("TM1650Display", cg.PollingComponent)
TM1650Key = tm1650_ns.class_("TM1650Key", binary_sensor.BinarySensor)

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(TM1650Key).extend(
    {
        cv.GenerateID(CONF_TM1650_ID): cv.use_id(TM1650Display),
        cv.Required(CONF_KEY): cv.int_range(min=0, max=15),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    cg.add(var.set_keycode(config[CONF_KEY]))
    hub = await cg.get_variable(config[CONF_TM1650_ID])
    cg.add(hub.add_tm1650_key(var))