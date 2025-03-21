#ifdef ENABLE_NETWORKING

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
            func_800BC154(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_NPC);
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
        if (!client.online) {
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

static std::set<std::string> teams;
void Anchor::InitializeMultiWorld() {
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
        // nlohmann::json spoiler = Rando::Spoiler::LoadFromFile(roomState.teams[i]);
        // Rando::Spoiler::ApplyToSaveContext(spoiler);
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

// MARK: - UI

void Anchor::DrawMenu() {
    ImGui::PushID("Anchor");

    ImGui::SeparatorText("Anchor");

    std::string host = CVarGetString("gNetwork.Anchor.Host", "anchor.proxysaw.dev");
    uint16_t port = CVarGetInteger("gNetwork.Anchor.Port", 43383);
    std::string anchorName = CVarGetString("gNetwork.Anchor.Name", "");
    std::string anchorTeamId = CVarGetString("gNetwork.Anchor.TeamId", "default");
    std::string anchorRoomId = CVarGetString("gNetwork.Anchor.RoomId", "");
    bool isFormValid = !isStringEmpty(host) && port > 1024 && port < 65535 && !isStringEmpty(anchorName) &&
                       !isStringEmpty(anchorRoomId) && !isStringEmpty(anchorTeamId);

    ImGui::BeginDisabled(isEnabled);

    if (UIWidgets::InputString("Host", &host)) {
        CVarSetString("gNetwork.Anchor.Host", host.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::Text("Port");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    UIWidgets::PushStyleSlider();
    if (ImGui::InputScalar("##gNetwork.Anchor.Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger("gNetwork.Anchor.Port", port);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    UIWidgets::PopStyleSlider();

    if (UIWidgets::InputString("Name", &anchorName)) {
        CVarSetString("gNetwork.Anchor.Name", anchorName.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    if (UIWidgets::InputString("Team ID", &anchorTeamId)) {
        CVarSetString("gNetwork.Anchor.TeamId", anchorTeamId.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    if (UIWidgets::InputString("Room ID", &anchorRoomId)) {
        CVarSetString("gNetwork.Anchor.RoomId", anchorRoomId.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = isEnabled ? "Disable Anchor" : "Enable Anchor";
    if (UIWidgets::Button(buttonLabel)) {
        if (isEnabled) {
            CVarClear("gNetwork.Anchor.Enabled");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            Anchor::Instance->Disable();
        } else {
            CVarSetInteger("gNetwork.Anchor.Enabled", 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            Anchor::Instance->Enable();
        }
    }
    ImGui::EndDisabled();

    if (isEnabled) {
        ImGui::Spacing();
        if (isConnected) {
            ImGui::Text("Connected");

            if (roomState.ownerClientId == ownClientId) {
                if (ImGui::CollapsingHeader("Room Settings")) {
                    static const char* pvpModes[3] = { "Off", "On", "On + Friendly Fire" };
                    if (UIWidgets::CVarCombobox("PvP Mode", "gNetwork.Anchor.RoomSettings.pvpMode", pvpModes)) {
                        SendPacket_UpdateRoomState();
                    }
                    ImGui::SeparatorText("Multi-World");

                    if (roomState.teams.empty()) {
                        std::string teamToRemove;
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Teams:");
                        ImGui::SameLine();
                        if (UIWidgets::Button(ICON_FA_PLUS, { .size = UIWidgets::Sizes::Inline,
                                                              .color = UIWidgets::Colors::Green })) {
                            ImGui::OpenPopup("TeamPopup");
                        }
                        for (auto& team : teams) {
                            ImGui::AlignTextToFramePadding();
                            ImGui::Text("%s", team.c_str());
                            ImGui::SameLine();
                            if (UIWidgets::Button(ICON_FA_TIMES, { .size = UIWidgets::Sizes::Inline,
                                                                   .color = UIWidgets::Colors::Red })) {
                                teamToRemove = team;
                            }
                        }
                        if (!teamToRemove.empty()) {
                            teams.erase(teamToRemove);
                            teamToRemove.clear();
                        }
                        if (ImGui::BeginPopup("TeamPopup")) {
                            ImGui::SeparatorText("Available Spoiler Logs:");
                            for (auto& spoilerFile : Rando::Spoiler::spoilerOptions) {
                                std::string teamName = spoilerFile.substr(0, spoilerFile.size() - 5);
                                if (spoilerFile == "Generate New Seed" || teams.contains(teamName)) {
                                    continue;
                                }

                                if (ImGui::Selectable(teamName.c_str())) {
                                    teams.insert(teamName);
                                }
                            }
                            ImGui::EndPopup();
                        }
                        // { .disabled = teams.size() < 2 || IsSaveLoaded(), .disabledTooltip = "You need at least 2
                        // teams to initialize multi-world, and you must not have a save loaded" }
                        if (UIWidgets::Button("Initialize Multi-World")) {
                            InitializeMultiWorld();
                        }
                    } else {
                        ImGui::Text("Teams:");
                        for (auto& team : roomState.teams) {
                            ImGui::Text("%s", team.c_str());
                        }
                    }
                }
            }

            ImGui::Text("Players in Room:");
            ImGui::Text("[%s] %s%s", CVarGetString("gNetwork.Anchor.TeamId", "default"),
                        CVarGetString("gNetwork.Anchor.Name", ""),
                        IsSaveLoaded() ? (std::string(" - ") + Ship_GetSceneName(gPlayState->sceneId)).c_str() : "");
            for (auto& [clientId, client] : Anchor::clients) {
                ImGui::PushID(clientId);
                if (client.online) {
                    ImGui::TextColored(ImVec4(1, 1, 1, 1), "[%s] %s - %s", client.teamId.c_str(), client.name.c_str(),
                                       Ship_GetSceneName(client.sceneId));
                    ImGui::SameLine();
                    if (UIWidgets::Button(ICON_FA_BUS, { .size = ImVec2(24.f, 24.f) })) {
                        SendPacket_RequestTeleport(client.clientId);
                    }
                    ImGui::AlignTextToFramePadding();
                } else {
                    ImGui::TextColored(ImVec4(1, 1, 1, 0.6f), "[%s] %s - Offline", client.teamId.c_str(),
                                       client.name.c_str());
                }
                ImGui::PopID();
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

    ImGui::PopID();
}

#endif
