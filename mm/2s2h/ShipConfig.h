#ifndef SHIP_CONFIG_H
#define SHIP_CONFIG_H

#ifdef __cplusplus
#include <nlohmann/json.hpp>
#include <string>

extern "C" {
#endif

typedef struct EnhancementsConfig {
    int someInt;
    int someOtherInt;

#ifdef __cplusplus
    std::string myName;
#endif
} EnhancementsConfig;

typedef struct RandoConfig {
    int someInt;
    int someOtherInt;

#ifdef __cplusplus
    std::string spoiler;
#endif
} RandoConfig;

typedef struct ShipConfig {
    int someInt;
    RandoConfig* rando;
    EnhancementsConfig* enhancements;

#ifdef __cplusplus
    std::string cppString;
#endif
} ShipConfig;

// Global config object
extern ShipConfig gShipConfig;

#ifdef __cplusplus
}

#endif

#endif // SHIP_CONFIG_H
