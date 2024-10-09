#ifdef ENABLE_NETWORKING

#include "Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/NameTag/NameTag.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenJsonConversions.hpp"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "z64.h"
#include "build.h"

#include "objects/object_test3/object_test3.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];
extern TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX];
extern EffectBlureInit2 D_8085D30C;
extern EffectTireMarkInit D_8085D330;
void Player_DrawGameplay(PlayState* play, Player* player, s32 lod, Gfx* cullDList,
                         OverrideLimbDrawFlex overrideLimbDraw);
}

const std::string Anchor::clientVersion = gBuildVersion;

// Packet types //
const std::string ALL_CLIENT_DATA = "ALL_CLIENT_DATA";
const std::string UPDATE_CLIENT_DATA = "UPDATE_CLIENT_DATA";
const std::string PLAYER_UPDATE = "PLAYER_UPDATE";
const std::string REQUEST_SAVE_STATE = "REQUEST_SAVE_STATE";
const std::string PUSH_SAVE_STATE = "PUSH_SAVE_STATE";

// Move this to BenJsonConversions.hpp?
using json = nlohmann::json;

void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = { 255, 255, 255 };
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = 0;
    j.contains("isSaveLoaded") ? j.at("isSaveLoaded").get_to(client.isSaveLoaded) : client.isSaveLoaded = false;
    j.contains("isGameComplete") ? j.at("isGameComplete").get_to(client.isGameComplete) : client.isGameComplete = false;
    j.contains("sceneId") ? j.at("sceneId").get_to(client.sceneId) : client.sceneId = SCENE_MAX;
    j.contains("entrance") ? j.at("entrance").get_to(client.entrance) : client.entrance = 0;
    j.contains("roomIndex") ? j.at("roomIndex").get_to(client.roomIndex) : client.roomIndex = 0;
    j.contains("transformation") ? j.at("transformation").get_to(client.transformation)
                                 : client.transformation = PLAYER_FORM_HUMAN;
    j.contains("posRot") ? j.at("posRot").get_to(client.posRot) : client.posRot = { -9999, -9999, -9999, 0, 0, 0 };
    j.contains("jointTable") && j.at("jointTable").get_to(client.jointTable);
}

// MARK: - Overrides

void Anchor::Enable() {
    Network::Enable(CVarGetString("gNetwork.Anchor.Host", "127.0.0.1"), CVarGetInteger("gNetwork.Anchor.Port", 43385));
}

void Anchor::Disable() {
    Network::Disable();

    clients.clear();
    RefreshClientActors();
}

void Anchor::OnConnected() {
    SendPacket_UpdateClientData();
    RegisterHooks();

    if (IsSaveLoaded()) {
        SendPacket_RequestSaveState();
    }
}

void Anchor::OnDisconnected() {
    RegisterHooks();
}

void Anchor::SendJsonToRemote(nlohmann::json payload) {
    if (!isConnected) {
        return;
    }

    payload["roomId"] = CVarGetString("gNetwork.Anchor.RoomId", "");
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

    // Ignore packets from mismatched clients, except for ALL_CLIENT_DATA or UPDATE_CLIENT_DATA
    if (packetType != ALL_CLIENT_DATA && packetType != UPDATE_CLIENT_DATA) {
        if (payload.contains("clientId")) {
            uint32_t clientId = payload["clientId"].get<uint32_t>();
            if (clients.contains(clientId) && clients[clientId].clientVersion != clientVersion) {
                return;
            }
        }
    }

    // packetType here is a string so we can't use a switch statement
    if (packetType == ALL_CLIENT_DATA) {
        HandlePacket_AllClientData(payload);
    } else if (packetType == UPDATE_CLIENT_DATA) {
        HandlePacket_UpdateClientData(payload);
    } else if (packetType == PLAYER_UPDATE) {
        HandlePacket_PlayerUpdate(payload);
    } else if (packetType == REQUEST_SAVE_STATE) {
        HandlePacket_RequestSaveState(payload);
    } else if (packetType == PUSH_SAVE_STATE) {
        HandlePacket_PushSaveState(payload);
    }
}

// MARK: - Packets

/**
 * ALL_CLIENT_DATA
 *
 * Contains a list of all clients and their CLIENT_DATA currently connected to the server
 *
 * The server itself sends this packet to all clients when a client connects or disconnects
 */

void Anchor::HandlePacket_AllClientData(nlohmann::json payload) {
    std::vector<AnchorClient> newClients = payload["clients"].get<std::vector<AnchorClient>>();

    // add new clients
    for (auto& client : newClients) {
        if (!clients.contains(client.clientId)) {
            clients[client.clientId] = client;
        }
    }

    // remove clients that are no longer in the list
    std::vector<uint32_t> clientsToRemove;
    for (auto& [clientId, client] : clients) {
        if (std::find_if(newClients.begin(), newClients.end(),
                         [clientId](AnchorClient& c) { return c.clientId == clientId; }) == newClients.end()) {
            clientsToRemove.push_back(clientId);
        }
    }
    // (seperate loop to avoid iterator invalidation)
    for (auto& clientId : clientsToRemove) {
        clients.erase(clientId);
    }

    RefreshClientActors();
}

/**
 * UPDATE_CLIENT_DATA
 *
 * Contains a small subset of data that is cached on the server and important for the client to know for various reasons
 *
 * Sent on various events, such as changing scenes, soft resetting, finishing the game, opening file select, etc.
 *
 * Note: This packet should be cross version compatible, so if you add anything here don't assume all clients will be
 * providing it, consider doing a `contains` check before accessing any version specific data
 */

void Anchor::SendPacket_UpdateClientData() {
    nlohmann::json payload;
    payload["type"] = UPDATE_CLIENT_DATA;
    payload["data"]["name"] = CVarGetString("gNetwork.Anchor.Name", "");
    payload["data"]["color"] = CVarGetColor24("gNetwork.Anchor.Color", { 100, 255, 100 });
    payload["data"]["clientVersion"] = clientVersion;

    if (IsSaveLoaded()) {
        payload["data"]["seed"] = 0; // TODO: Implement seed
        payload["data"]["isSaveLoaded"] = true;
        payload["data"]["isGameComplete"] = false; // TODO: Implement game completion check
        payload["data"]["sceneId"] = gPlayState->sceneId;
        payload["data"]["entrance"] = gSaveContext.save.entrance;
    } else {
        payload["data"]["seed"] = 0;
        payload["data"]["isSaveLoaded"] = false;
        payload["data"]["isGameComplete"] = false;
        payload["data"]["sceneId"] = SCENE_MAX;
        payload["data"]["entrance"] = 0x00;
    }

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateClientData(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();

    if (clients.contains(clientId)) {
        AnchorClient client = payload["data"].get<AnchorClient>();
        clients[clientId].clientId = clientId;
        clients[clientId].name = client.name;
        clients[clientId].color = client.color;
        clients[clientId].clientVersion = client.clientVersion;
        clients[clientId].seed = client.seed;
        clients[clientId].isSaveLoaded = client.isSaveLoaded;
        clients[clientId].isGameComplete = client.isGameComplete;
        clients[clientId].sceneId = client.sceneId;
        clients[clientId].entrance = client.entrance;
    }
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
        if (client.sceneId == gPlayState->sceneId) {
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
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId) {
            payload["targetClientId"] = clientId;
            SendJsonToRemote(payload);
        }
    }
}

void Anchor::HandlePacket_PlayerUpdate(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();

    bool shouldRefreshActors = false;

    if (clients.contains(clientId)) {
        auto& client = clients[clientId];

        if (client.transformation != payload["transformation"].get<uint8_t>()) {
            shouldRefreshActors = true;
        }

        client.sceneId = payload["sceneId"].get<int16_t>();
        client.entrance = payload["entrance"].get<int32_t>();
        client.roomIndex = payload["roomIndex"].get<int8_t>();
        client.transformation = payload["transformation"].get<uint8_t>();
        client.posRot = payload["posRot"].get<PosRot>();
        for (int i = 0; i < 159; i++) {
            client.jointTable[i] = payload["jointTable"][i].get<uint8_t>();
        }
    }

    if (shouldRefreshActors) {
        RefreshClientActors();
    }
}

/**
 * REQUEST_SAVE_STATE
 *
 * Requests a save state from the server, this will be sent to all clients connected, all clients that have a save
 * loaded will emit a PUSH_SAVE_STATE packet, but only the first client to respond will fulfill the request
 *
 * This fires when loading into a file while Anchor is connected, or when Anchor is connected while a file is already
 * loaded
 *
 * Note: This can additionally be fired with a button in the menus to fix any desyncs that may have occurred in the save
 * state
 */

void Anchor::SendPacket_RequestSaveState() {
    nlohmann::json payload;
    payload["type"] = REQUEST_SAVE_STATE;

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_RequestSaveState(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    SendPacket_PushSaveState();
}

/**
 * PUSH_SAVE_STATE
 *
 * Pushes the current save state to the server, this will be sent to any requesting clients
 */

void Anchor::SendPacket_PushSaveState() {
    if (!IsSaveLoaded()) {
        return;
    }

    json payload = gSaveContext;
    payload["type"] = PUSH_SAVE_STATE;
    // TODO: Manually update scene flags from actorCtx

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_PushSaveState(nlohmann::json payload) {
    // This can happen in between file select and the game starting, so we cant use this check, but we need to ensure we
    // be careful to wrap PlayState usage in this check
    // if (!IsSaveLoaded()) {
    //     return;
    // }

    SaveContext loadedData = payload.get<SaveContext>();

    // Restore bottle contents (unless it's the Deku Princess)
    for (int i = 0; i < 6; i++) {
        if (gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE &&
            gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_DEKU_PRINCESS) {
            loadedData.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] =
                gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i];
        }
    }

    // Restore ammo if it's non-zero, unless it's beans
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.inventory.ammo); i++) {
        if (gSaveContext.save.saveInfo.inventory.ammo[i] != 0 && i != SLOT(ITEM_MAGIC_BEANS)) {
            loadedData.save.saveInfo.inventory.ammo[i] = gSaveContext.save.saveInfo.inventory.ammo[i];
        }
    }

    gSaveContext.save.saveInfo.inventory = loadedData.save.saveInfo.inventory;
}

// MARK: - Actor

// Primarily modeled after EnTest3_Init and Player_Init
void DummyInit(Actor* actor, PlayState* play) {
    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (!Anchor::Instance->clients.contains(clientId)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[clientId];

    client.player.actor.room = -1;
    client.player.csId = CS_ID_NONE;
    client.player.transformation = client.transformation;
    client.player.ageProperties = &sPlayerAgeProperties[client.player.transformation];
    client.player.heldItemAction = PLAYER_IA_NONE;
    client.player.heldItemId = ITEM_OCARINA_OF_TIME;

    Player_SetModelGroup(&client.player, PLAYER_MODELGROUP_DEFAULT);
    play->playerInit(&client.player, play, gPlayerSkeletons[client.player.transformation]);

    Effect_Add(play, &client.player.meleeWeaponEffectIndex[0], EFFECT_BLURE2, 0, 0, &D_8085D30C);
    Effect_Add(play, &client.player.meleeWeaponEffectIndex[1], EFFECT_BLURE2, 0, 0, &D_8085D30C);
    Effect_Add(play, &client.player.meleeWeaponEffectIndex[2], EFFECT_TIRE_MARK, 0, 0, &D_8085D330);

    client.player.maskObjectSegment = ZeldaArena_Malloc(0x3800);
    play->func_18780(&client.player, play);

    // Ensures the actor is always updating/drawing even when culled/out of distance
    actor->flags = ACTOR_FLAG_10 | ACTOR_FLAG_20 | ACTOR_FLAG_40;
    // Will ensure the actor is always updating even when in a separate room than the player
    actor->room = -1;

    NameTag_RegisterForActorWithOptions(actor, client.name.c_str(), { .yOffset = 30 });
}

// Mostly used to update the actor with new data from the client
void DummyUpdate(Actor* actor, PlayState* play) {
    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (Anchor::Instance->clients.contains(clientId)) {
        if (Anchor::Instance->clients[clientId].sceneId == gPlayState->sceneId) {
            AnchorClient& client = Anchor::Instance->clients[clientId];
            Math_Vec3s_Copy(&actor->shape.rot, &client.posRot.rot);
            Math_Vec3f_Copy(&actor->world.pos, &client.posRot.pos);
            memcpy(&client.player.jointTableBuffer, &client.jointTable, 159);
        } else {
            actor->world.pos.x = -9999.0f;
            actor->world.pos.y = -9999.0f;
            actor->world.pos.z = -9999.0f;
        }
    } else {
        Actor_Kill(actor);
    }
}

void DummyDraw(Actor* actor, PlayState* play) {
    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (Anchor::Instance->clients.contains(clientId) &&
        Anchor::Instance->clients[clientId].sceneId == gPlayState->sceneId) {
        AnchorClient& client = Anchor::Instance->clients[clientId];
        Player_DrawGameplay(play, &client.player, 1, gCullBackDList, Player_OverrideLimbDrawGameplayDefault);
    }
}

// No-op to replace the ACTOR_ITEM_INBOX destroy
void DummyDestroy(Actor* actor, PlayState* play) {
}

// Kills all existing anchor actors and respawns them with the new client data
void Anchor::RefreshClientActors() {
    if (!IsSaveLoaded()) {
        return;
    }

    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_ITEM_INBOX && actor->update == DummyUpdate) {
            NameTag_RemoveAllForActor(actor);
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    actorIndexToClientId.clear();
    refreshingActors = true;
    for (auto [clientId, client] : clients) {
        actorIndexToClientId.push_back(clientId);
        // We are using a hook `ShouldActorInit` to override the init/update/draw/destroy functions of whatever actor we
        // spawn here, after some looking I landed on ACTOR_ITEM_INBOX because it seemed to be the most simple un-used
        // actor, but it could be any actor that doesn't have various side effects. (EnTest3 for example has various
        // side effects we don't want)
        auto dummy = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_ITEM_INBOX, client.posRot.pos.x,
                                 client.posRot.pos.y, client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y,
                                 client.posRot.rot.z, actorIndexToClientId.size() - 1);
    }
    refreshingActors = false;
}

// MARK: - Misc/Helpers

void Anchor::RegisterHooks() {
    static HOOK_ID onSceneSpawnActorsHookId = 0;
    static HOOK_ID shouldActorInitHookId = 0;
    static HOOK_ID onPlayerUpdateHook = 0;
    static HOOK_ID onSaveLoadHookId = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneSpawnActorsHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(shouldActorInitHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSaveLoad>(onSaveLoadHookId);

    onSceneSpawnActorsHookId = 0;
    shouldActorInitHookId = 0;
    onPlayerUpdateHook = 0;
    onSaveLoadHookId = 0;

    if (!isConnected) {
        return;
    }

    onSceneSpawnActorsHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([&]() {
        if (!isConnected) {
            return;
        }

        SendPacket_UpdateClientData();

        if (IsSaveLoaded()) {
            RefreshClientActors();
        }
    });

    // See note within DummyInit for why we are using ACTOR_ITEM_INBOX
    shouldActorInitHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_ITEM_INBOX, [&](Actor* actor, bool* should) {
            if (!isConnected) {
                return;
            }

            if (refreshingActors) {
                actor->init = DummyInit;
                actor->update = DummyUpdate;
                actor->draw = DummyDraw;
                actor->destroy = DummyDestroy;
            }
        });

    onPlayerUpdateHook =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, [&](Actor* actor) {
            if (!isConnected) {
                return;
            }

            SendPacket_PlayerUpdate();
        });

    onSaveLoadHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>([&](s16 fileNum) {
        if (!isConnected) {
            return;
        }

        SendPacket_RequestSaveState();
    });
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

    return true;
}

// MARK: - UI

void Anchor::DrawMenu() {
    ImGui::SeparatorText("Anchor");
    UIWidgets::Tooltip("Anchor is a networking protocol designed to facilitate remote "
                       "control of the Ship of Harkinian client. It is intended to "
                       "be utilized alongside a Anchor server, for which we provide a "
                       "few straightforward implementations on our GitHub. The current "
                       "implementations available allow integration with Twitch chat "
                       "and SAMMI Bot, feel free to contribute your own!\n"
                       "\n"
                       "Click the question mark to copy the link to the Anchor Github "
                       "page to your clipboard.");
    if (ImGui::IsItemClicked()) {
        ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
    }

    static std::string ip = CVarGetString("gNetwork.Anchor.Host", "127.0.0.1");
    static uint16_t port = CVarGetInteger("gNetwork.Anchor.Port", 43385);
    static std::string AnchorName = CVarGetString("gNetwork.Anchor.Name", "");
    static std::string anchorRoomId = CVarGetString("gNetwork.Anchor.RoomId", "");
    bool isFormValid = !isStringEmpty(CVarGetString("gNetwork.Anchor.Host", "127.0.0.1")) && port > 1024 &&
                       port < 65535 && !isStringEmpty(CVarGetString("gNetwork.Anchor.Name", "")) &&
                       !isStringEmpty(CVarGetString("gNetwork.Anchor.RoomId", ""));

    ImGui::BeginDisabled(isEnabled);

    ImGui::Text("Remote IP & Port");
    UIWidgets::PushStyleSlider();
    if (ImGui::InputText("##gNetwork.Anchor.Host", (char*)ip.c_str(), ip.capacity() + 1)) {
        CVarSetString("gNetwork.Anchor.Host", ip.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
    if (ImGui::InputScalar("##gNetwork.Anchor.Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger("gNetwork.Anchor.Port", port);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    ImGui::Text("Fairy Color & Name");
    static Color_RGBA8 color = CVarGetColor("gNetwork.Anchor.Color", { 100, 255, 100, 255 });
    static ImVec4 colorVec = ImVec4(color.r / 255.0, color.g / 255.0, color.b / 255.0, 1);
    if (ImGui::ColorEdit3("##gNetwork.Anchor.Color", (float*)&colorVec,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        color.r = colorVec.x * 255.0;
        color.g = colorVec.y * 255.0;
        color.b = colorVec.z * 255.0;

        CVarSetColor("gNetwork.Anchor.Color", color);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("##gNetwork.Anchor.Name", (char*)AnchorName.c_str(), AnchorName.capacity() + 1)) {
        CVarSetString("gNetwork.Anchor.Name", AnchorName.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::Text("Room ID");
    int flags = 0;
    if (isEnabled) {
        flags = ImGuiInputTextFlags_Password;
    }
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("##gNetwork.Anchor.RoomId", (char*)anchorRoomId.c_str(), anchorRoomId.capacity() + 1, flags)) {
        CVarSetString("gNetwork.Anchor.RoomId", anchorRoomId.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    UIWidgets::PopStyleSlider();

    ImGui::PopItemWidth();
    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = isEnabled ? "Disable Anchor" : "Enable Anchor";
    if (UIWidgets::Button(buttonLabel)) {
        if (isEnabled) {
            CVarClear("gNetwork.Anchor.Enabled");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            Anchor::Instance->Disable();
        } else {
            CVarSetInteger("gNetwork.Anchor.Enabled", 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            Anchor::Instance->Enable();
        }
    }
    ImGui::EndDisabled();

    if (isEnabled) {
        ImGui::Spacing();
        if (isConnected) {
            ImGui::Text("Connected");

            ImGui::Text("Players in Room:");
            ImGui::Text("%s - %s", CVarGetString("gNetwork.Anchor.Name", ""),
                        getSceneName(gPlayState == NULL ? SCENE_MAX : gPlayState->sceneId));
            for (auto& [clientId, client] : Anchor::clients) {
                ImGui::Text("%s - %s", client.name.c_str(), getSceneName(client.sceneId));
                // if (client.clientVersion != Anchor::clientVersion) {
                //     ImGui::SameLine();
                //     ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                //     if (ImGui::IsItemHovered()) {
                //         ImGui::BeginTooltip();
                //         ImGui::Text("Incompatible version! Will not work together!");
                //         ImGui::Text("Yours: %s", Anchor::clientVersion.c_str());
                //         ImGui::Text("Theirs: %s", client.clientVersion.c_str());
                //         ImGui::EndTooltip();
                //     }
                // }
                // if (client.seed != gSaveContext.finalSeed && client.fileNum != 0xFF && gSaveContext.fileNum != 0xFF)
                // {
                //     ImGui::SameLine();
                //     ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                //     if (ImGui::IsItemHovered()) {
                //         ImGui::BeginTooltip();
                //         ImGui::Text("Seed mismatch! Continuing will break things!");
                //         ImGui::Text("Yours: %u", gSaveContext.finalSeed);
                //         ImGui::Text("Theirs: %u", client.seed);
                //         ImGui::EndTooltip();
                //     }
                // }
            }
        } else {
            ImGui::Text("Connecting...");
        }
    }
}

#endif
