#ifndef NETWORK_ANCHOR_JSON_CONVERSIONS_H
#define NETWORK_ANCHOR_JSON_CONVERSIONS_H
#ifdef __cplusplus

#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "Anchor.h"
#include "BenJsonConversions.hpp"

extern "C" {
}

using json = nlohmann::json;

inline void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = { 255, 255, 255 };
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("teamId") ? j.at("teamId").get_to(client.teamId) : client.teamId = "default";
    j.contains("online") ? j.at("online").get_to(client.online) : client.online = false;
    j.contains("self") ? j.at("self").get_to(client.self) : client.self = false;
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = 0;
    j.contains("isSaveLoaded") ? j.at("isSaveLoaded").get_to(client.isSaveLoaded) : client.isSaveLoaded = false;
    j.contains("isGameComplete") ? j.at("isGameComplete").get_to(client.isGameComplete) : client.isGameComplete = false;
    j.contains("sceneId") ? j.at("sceneId").get_to(client.sceneId) : client.sceneId = SCENE_MAX;
    j.contains("entrance") ? j.at("entrance").get_to(client.entrance) : client.entrance = 0;
}

#endif // __cplusplus
#endif // NETWORK_ANCHOR_JSON_CONVERSIONS_H
