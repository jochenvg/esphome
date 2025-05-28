import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

fujitsu_general_ar_je5_ns = cg.esphome_ns.namespace("fujitsu_general_ar_je5")
FujitsuGeneralClimate = fujitsu_general_ar_je5_ns.class_(
    "FujitsuGeneralClimate", climate_ir.ClimateIR
)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(FujitsuGeneralClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)
