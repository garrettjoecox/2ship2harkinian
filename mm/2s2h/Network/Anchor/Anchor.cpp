#include "Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/NameTag/NameTag.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/Rando/Spoiler/Spoiler.h"
#include "2s2h/Rando/MiscBehavior/MiscBehavior.h"

extern "C" {
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

// MARK: - Overrides

void Anchor::Enable() {
    Network::Enable(CVarGetString("gNetwork.Anchor.Host", "anchor.proxysaw.dev"),
                    CVarGetInteger("gNetwork.Anchor.Port", 43383));
    ownClientId = CVarGetInteger("gNetwork.Anchor.LastClientId", 0);
    roomState.ownerClientId = 0;
}

void Anchor::Disable() {
    Network::Disable();

    clients.clear();
    roomState.teams.clear();
    RefreshClientActors();
}

void Anchor::OnConnected() {
    SendPacket_Handshake();
    RegisterHooks();

    if (IsSaveLoaded()) {
        SendPacket_RequestTeamState();
    }
}

void Anchor::OnDisconnected() {
    RegisterHooks();
}

void Anchor::SendJsonToRemote(nlohmann::json payload) {
    if (!isConnected) {
        return;
    }

    payload["clientId"] = ownClientId;
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Sending payload:\n{}", payload.dump());
    }
    Network::SendJsonToRemote(payload);
}

void Anchor::OnIncomingJson(nlohmann::json payload) {
    // If it doesn't contain a type, it's not a valid payload
    if (!payload.contains("type")) {
        return;
    }

    // If it's not a quiet payload, log it
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Received payload:\n{}", payload.dump());
    }

    std::string packetType = payload["type"].get<std::string>();

    // Ignore packets from mismatched clients, except for ALL_CLIENT_STATE or UPDATE_CLIENT_STATE
    if (packetType != ALL_CLIENT_STATE && packetType != UPDATE_CLIENT_STATE) {
        if (payload.contains("clientId")) {
            uint32_t clientId = payload["clientId"].get<uint32_t>();
            if (clients.contains(clientId) && clients[clientId].clientVersion != clientVersion) {
                return;
            }
        }
    }

    // Handle PLAYER_UPDATE packets immediately, no need to queue
    if (packetType == PLAYER_UPDATE) {
        HandlePacket_PlayerUpdate(payload);
        return;
    }

    // Queue all packets to be processed on the game thread
    std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);
    incomingPacketQueue.push(payload);
}

void Anchor::ProcessIncomingPacketQueue() {
    std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);

    while (!incomingPacketQueue.empty()) {
        nlohmann::json payload = incomingPacketQueue.front();
        incomingPacketQueue.pop();

        std::string packetType = payload["type"].get<std::string>();

        isProcessingIncomingPacket = true;

        // packetType here is a string so we can't use a switch statement
        if (packetType == ALL_CLIENT_STATE)
            HandlePacket_AllClientState(payload);
        else if (packetType == DAMAGE_PLAYER)
            HandlePacket_DamagePlayer(payload);
        else if (packetType == DISABLE_ANCHOR)
            HandlePacket_DisableAnchor(payload);
        else if (packetType == GAME_COMPLETE)
            HandlePacket_GameComplete(payload);
        else if (packetType == GIVE_ITEM)
            HandlePacket_GiveItem(payload);
        else if (packetType == PLAYER_SFX)
            HandlePacket_PlayerSfx(payload);
        else if (packetType == PLAYER_UPDATE)
            HandlePacket_PlayerUpdate(payload);
        else if (packetType == UPDATE_TEAM_STATE)
            HandlePacket_UpdateTeamState(payload);
        else if (packetType == REQUEST_TEAM_STATE)
            HandlePacket_RequestTeamState(payload);
        else if (packetType == REQUEST_TELEPORT)
            HandlePacket_RequestTeleport(payload);
        else if (packetType == SERVER_MESSAGE)
            HandlePacket_ServerMessage(payload);
        else if (packetType == SET_CHECK_STATUS)
            HandlePacket_SetCheckStatus(payload);
        else if (packetType == SET_FLAG)
            HandlePacket_SetFlag(payload);
        else if (packetType == TELEPORT_TO)
            HandlePacket_TeleportTo(payload);
        else if (packetType == UNSET_FLAG)
            HandlePacket_UnsetFlag(payload);
        else if (packetType == UPDATE_CLIENT_STATE)
            HandlePacket_UpdateClientState(payload);
        else if (packetType == UPDATE_ROOM_STATE)
            HandlePacket_UpdateRoomState(payload);
        else if (packetType == UPDATE_DUNGEON_ITEMS)
            HandlePacket_UpdateDungeonItems(payload);

        isProcessingIncomingPacket = false;
    }
}

static bool justReset = false;
static bool sGainedControl = false;

void Anchor::RegisterHooks() {
    COND_HOOK(OnSceneSpawnActors, isConnected, [&]() {
        SendPacket_UpdateClientState();
        justReset = false;

        if (IsSaveLoaded()) {
            RefreshClientActors();
        }
    });

    // HOOK(OnPresentFileSelect, isConnected, [&]() {
    //     SendPacket_UpdateClientState();
    // });

    COND_ID_HOOK(ShouldActorInit, ACTOR_PLAYER, isConnected, [&](void* actorRef, bool* should) {
        Actor* actor = (Actor*)actorRef;

        if (refreshingActors) {
            // By the time we get here, the actor was already added to the ACTORCAT_PLAYER list, so we need to move it
            Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_NPC);
            actor->id = ACTOR_ITEM_INBOX;
            actor->category = ACTORCAT_NPC;
            actor->init = DummyPlayer_Init;
            actor->update = DummyPlayer_Update;
            actor->draw = DummyPlayer_Draw;
            actor->destroy = DummyPlayer_Destroy;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, isConnected, [&](Actor* actor) {
        sGainedControl = true;
        SendPacket_PlayerUpdate();

        if (shouldRefreshActors) {
            shouldRefreshActors = false;
            RefreshClientActors();
        }
    });

    COND_HOOK(OnPlayerSfx, isConnected, [&](u16 sfxId) { SendPacket_PlayerSfx(sfxId); });

    COND_HOOK(OnSaveLoad, isConnected, [&](s16 fileNum) {
        sGainedControl = false;
        SendPacket_RequestTeamState();
    });

    COND_HOOK(OnConsoleLogoUpdate, isConnected, [&]() {
        if (!justReset) {
            SendPacket_UpdateClientState();
            justReset = true;
        }
    });

    COND_HOOK(AfterOwlSave, isConnected, [&]() {
        if (gPlayState != NULL) {
            SendPacket_UpdateTeamState(CVarGetString("gNetwork.Anchor.TeamId", "default"));
        }
    });

    COND_HOOK(AfterEndOfCycleSave, isConnected, [&]() {
        if (gPlayState != NULL) {
            SendPacket_UpdateTeamState(CVarGetString("gNetwork.Anchor.TeamId", "default"));
        }
    });

    COND_HOOK(OnGameStateUpdate, isConnected, [&]() { ProcessIncomingPacketQueue(); });

    // COND_HOOK(OnFlagSet, isConnected, [&](s16 flagType, s16 flag) { SendPacket_SetFlag(SCENE_MAX, flagType, flag);
    // });

    // COND_HOOK(OnFlagUnset, isConnected,
    //           [&](s16 flagType, s16 flag) { SendPacket_UnsetFlag(SCENE_MAX, flagType, flag); });

    // COND_HOOK(OnSceneFlagSet, isConnected,
    //           [&](s16 sceneId, s16 flagType, s16 flag) { SendPacket_SetFlag(sceneId, flagType, flag); });

    // COND_HOOK(OnSceneFlagUnset, isConnected,
    //           [&](s16 sceneId, s16 flagType, s16 flag) { SendPacket_UnsetFlag(sceneId, flagType, flag); });
}

// MARK: - Misc/Helpers

// Kills all existing anchor actors and respawns them with the new client data
void Anchor::RefreshClientActors() {
    if (!IsSaveLoaded()) {
        return;
    }

    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_ITEM_INBOX && actor->update == DummyPlayer_Update) {
            NameTag_RemoveAllForActor(actor);
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    actorIndexToClientId.clear();
    refreshingActors = true;
    for (auto& [clientId, client] : clients) {
        if (!client.online || client.self) {
            continue;
        }

        actorIndexToClientId.push_back(clientId);
        // We are using a hook `ShouldActorInit` to override the init/update/draw/destroy functions of the Player we
        // spawn We quickly store a mapping of "index" to clientId, then within the init function we use this to get the
        // clientId and store it on player->zTargetActiveTimer (unused s32 for the dummy) for convenience
        auto dummy = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_PLAYER, client.posRot.pos.x,
                                 client.posRot.pos.y, client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y,
                                 client.posRot.rot.z, actorIndexToClientId.size() - 1);
        client.player = (Player*)dummy;
    }
    refreshingActors = false;
}

bool Anchor::IsSaveLoaded() {
    if (gPlayState == nullptr) {
        return false;
    }

    if (GET_PLAYER(gPlayState) == nullptr) {
        return false;
    }

    if (gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return false;
    }

    if (gSaveContext.gameMode != GAMEMODE_NORMAL) {
        return false;
    }

    // Not in daytelop
    if (!sGainedControl) {
        return false;
    }

    return true;
}

void Anchor::InitializeMultiWorld(std::set<std::string>& teams) {
    roomState.teams = std::vector<std::string>(teams.begin(), teams.end());
    teams.clear();
    SendPacket_UpdateRoomState();

    std::string previousSpoiler = CVarGetString("gRando.SpoilerFile", "");
    int previousSpoilerFileIndex = CVarGetInteger("gRando.SpoilerFileIndex", 0);
    CVarSetInteger("gRando.SpoilerFileIndex", -1);

    Ship_Random_Seed(Ship_Random(0, 1000000));

    std::vector<SaveContext> worlds;
    std::vector<std::pair<RandoCheckId, int>> checkPool;
    std::vector<std::pair<RandoItemId, int>> itemPool;

    for (int i = 0; i < roomState.teams.size(); i++) {
        CVarSetString("gRando.SpoilerFile", (roomState.teams[i] + ".json").c_str());
        Sram_InitNewSave();
        Rando::MiscBehavior::OnFileCreate(0);

        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
                checkPool.push_back({ randoCheckId, i });
                itemPool.push_back({ RANDO_SAVE_CHECKS[randoCheckId].randoItemId, i });
            }
        }

        SaveContext save = gSaveContext;
        worlds.push_back(save);
    }

    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
    }

    for (int i = 0; i < roomState.teams.size(); i++) {
        gSaveContext = worlds[i];
        for (size_t j = 0; j < checkPool.size(); j++) {
            if (checkPool[j].second != i) {
                continue;
            }
            RANDO_SAVE_CHECKS[checkPool[j].first].randoItemId = itemPool[j].first;
            RANDO_SAVE_CHECKS[checkPool[j].first].multiWorldTeamIndex = itemPool[j].second;
        }

        SendPacket_UpdateTeamState(roomState.teams[i]);
    }

    CVarSetString("gRando.SpoilerFile", previousSpoiler.c_str());
    CVarSetInteger("gRando.SpoilerFileIndex", previousSpoilerFileIndex);
}
