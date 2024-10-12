#include "ShipConfig.h"

EnhancementsConfig gEnhancementsConfig = {};
RandoConfig gRandoConfig = {};
ShipConfig gShipConfig = {
    .someInt = 1,
    .rando = &gRandoConfig,
    .enhancements = &gEnhancementsConfig,
    .cppString = "cppString"
};

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnhancementsConfig, myName, someInt, someOtherInt)
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RandoConfig, spoiler, someInt, someOtherInt)
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShipConfig, cppString, someInt, rando, enhancements)

