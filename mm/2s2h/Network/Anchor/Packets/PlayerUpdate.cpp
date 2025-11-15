#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * PLAYER_UPDATE
 *
 * Contains real-time data necessary to update other clients in the same scene as the player
 *
 * Sent every frame to other clients within the same scene
 *
 * Note: This packet is sent _a lot_, so please do not include any unnecessary data in it
 */

void Anchor::SendPacket_PlayerUpdate() {
    if (!IsSaveLoaded()) {
        return;
    }

    uint32_t currentPlayerCount = 0;
    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId && client.online && client.isSaveLoaded && !client.self) {
            currentPlayerCount++;
        }
    }
    if (currentPlayerCount == 0) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    nlohmann::json payload;

    payload["type"] = PLAYER_UPDATE;
    payload["sceneId"] = gPlayState->sceneId;
    payload["entrance"] = gSaveContext.save.entrance;
    payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
    payload["transformation"] = player->transformation;
    payload["posRot"]["pos"] = player->actor.world.pos;
    payload["posRot"]["rot"] = player->actor.shape.rot;
    payload["jointTable"] = player->jointTableBuffer;
    payload["upperJointTable"] = player->jointTableUpperBuffer;
    payload["currentMask"] = player->currentMask;
    payload["rightHandType"] = player->rightHandType;
    payload["leftHandType"] = player->leftHandType;
    payload["currentShield"] = player->currentShield;
    payload["sheathType"] = player->sheathType;
    payload["heldItemAction"] = player->heldItemAction;
    payload["heldItemId"] = player->heldItemId;
    payload["itemAction"] = player->itemAction;
    payload["stateFlags1"] = player->stateFlags1;
    payload["stateFlags2"] = player->stateFlags2;
    payload["stateFlags3"] = player->stateFlags3;
    payload["unk_B0C"] = player->unk_B0C;
    payload["unk_B28"] = player->unk_B28;
    payload["unk_ACC"] = player->unk_ACC;
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId && client.online && client.isSaveLoaded && !client.self) {
            payload["targetClientId"] = clientId;
            SendJsonToRemote(payload);
        }
    }
}

void Anchor::HandlePacket_PlayerUpdate(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();

    if (clients.contains(clientId)) {
        auto& client = clients[clientId];

        if (client.transformation != payload["transformation"].get<uint8_t>()) {
            shouldRefreshActors = true;
        }

        client.sceneId = payload["sceneId"].get<int16_t>();
        client.entrance = payload["entrance"].get<int32_t>();
        client.transformation = payload["transformation"].get<uint8_t>();
        client.posRot = payload["posRot"].get<PosRot>();
        for (int i = 0; i < 159; i++) {
            client.jointTable[i] = payload["jointTable"][i].get<uint8_t>();
            client.upperJointTable[i] = payload["upperJointTable"][i].get<uint8_t>();
        }
        client.currentMask = payload["currentMask"].get<uint8_t>();
        client.rightHandType = payload["rightHandType"].get<uint8_t>();
        client.leftHandType = payload["leftHandType"].get<uint8_t>();
        client.currentShield = payload["currentShield"].get<int8_t>();
        client.sheathType = payload["sheathType"].get<uint8_t>();
        client.heldItemAction = payload["heldItemAction"].get<int8_t>();
        client.heldItemId = payload["heldItemId"].get<uint8_t>();
        client.itemAction = payload["itemAction"].get<int8_t>();
        client.stateFlags1 = payload["stateFlags1"].get<uint32_t>();
        client.stateFlags2 = payload["stateFlags2"].get<uint32_t>();
        client.stateFlags3 = payload["stateFlags3"].get<uint32_t>();
        client.unk_B0C = payload["unk_B0C"].get<float>();
        client.unk_B28 = payload["unk_B28"].get<int16_t>();
        client.unk_ACC = payload["unk_ACC"].get<int16_t>();
    }
}
