import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_CLK_PIN,
    CONF_DIO_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_INTENSITY,
    CONF_LENGTH
)
from esphome.const import __version__ as ESPHOME_VERSION

CODEOWNERS = ["@glmnet"]

TM1650_ns = cg.esphome_ns.namespace("tm1650")
TM1650Display = TM1650_ns.class_("TM1650Display", cg.PollingComponent)
TM1650DisplayRef = TM1650Display.operator("ref")

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(TM1650Display),
        cv.Optional(CONF_INTENSITY, default=7): cv.All(
            cv.uint8_t, cv.Range(min=0, max=7)
        ),
        cv.Optional(CONF_LENGTH, default=6): cv.All(cv.uint8_t, cv.Range(min=1, max=6)),
        cv.Required(CONF_CLK_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DIO_PIN): pins.gpio_output_pin_schema
    }
).extend(cv.polling_component_schema("1s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    if cv.Version.parse(ESPHOME_VERSION) < cv.Version.parse("2023.12.0"):
        await cg.register_component(var, config)

    clk = await cg.gpio_pin_expression(config[CONF_CLK_PIN])
    cg.add(var.set_clk_pin(clk))
    dio = await cg.gpio_pin_expression(config[CONF_DIO_PIN])
    cg.add(var.set_dio_pin(dio))
    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    cg.add(var.set_length(config[CONF_LENGTH]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(TM1650DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))