#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/BenPort.h"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * UPDATE_ROOM_STATE
 */

nlohmann::json Anchor::PrepRoomState() {
    nlohmann::json payload;
    payload["ownerClientId"] = ownClientId;
    payload["pvpMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.PvpMode", 0);
    payload["teleportMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.TeleportMode", 1);
    payload["showLocationsMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.ShowLocationsMode", 1);
    payload["syncItemsAndFlags"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.SyncItemsAndFlags", 1);
    payload["teams"] = roomState.teams;

    return payload;
}

void Anchor::SendPacket_UpdateRoomState() {
    nlohmann::json payload;
    payload["type"] = UPDATE_ROOM_STATE;
    payload["state"] = PrepRoomState();

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateRoomState(nlohmann::json payload) {
    if (!payload.contains("state")) {
        return;
    }

    roomState.ownerClientId = payload["state"]["ownerClientId"].get<uint32_t>();
    roomState.pvpMode = payload["state"]["pvpMode"].get<u8>();
    roomState.teleportMode = payload["state"]["teleportMode"].get<u8>();
    roomState.showLocationsMode = payload["state"]["showLocationsMode"].get<u8>();
    roomState.syncItemsAndFlags = payload["state"]["syncItemsAndFlags"].get<u8>();
    roomState.teams = payload["state"]["teams"].get<std::vector<std::string>>();
}
