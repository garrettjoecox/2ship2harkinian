#ifdef ENABLE_NETWORKING

#include "Sail.h"
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "ShipUtils.h"

void Sail::Enable() {
    Network::Enable(CVarGetString("gNetwork.Sail.Host", "127.0.0.1"), CVarGetInteger("gNetwork.Sail.Port", 43385));
}

void Sail::OnIncomingJson(nlohmann::json payload) {
    nlohmann::json responsePayload;
    responsePayload["type"] = "result";
    responsePayload["status"] = "failure";

    try {
        if (!payload.contains("id")) {
            SPDLOG_ERROR("[Sail] Received payload without ID");
            SendJsonToRemote(responsePayload);
            return;
        }

        responsePayload["id"] = payload["id"];

        if (!payload.contains("type")) {
            SPDLOG_ERROR("[Sail] Received payload without type");
            SendJsonToRemote(responsePayload);
            return;
        }

        std::string payloadType = payload["type"].get<std::string>();

        if (payloadType == "command") {
            if (!payload.contains("command")) {
                SPDLOG_ERROR("[Sail] Received command payload without command");
                SendJsonToRemote(responsePayload);
                return;
            }

            std::string command = payload["command"].get<std::string>();
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch(command);
            responsePayload["status"] = "success";
            SendJsonToRemote(responsePayload);
            return;
        } else if (payloadType == "effect") {
            if (!payload.contains("effect") || !payload["effect"].contains("type")) {
                SPDLOG_ERROR("[Sail] Received effect payload without effect type");
                SendJsonToRemote(responsePayload);
                return;
            }

            std::string effectType = payload["effect"]["type"].get<std::string>();

            // Special case for "command" effect, so we can also run commands from the `simple_twitch_sail` script
            if (effectType == "command") {
                if (!payload["effect"].contains("command")) {
                    SPDLOG_ERROR("[Sail] Received command effect payload without command");
                    SendJsonToRemote(responsePayload);
                    return;
                }

                std::string command = payload["effect"]["command"].get<std::string>();
                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                    ->Dispatch(command);
                responsePayload["status"] = "success";
                SendJsonToRemote(responsePayload);
                return;
            }

            if (effectType != "apply" && effectType != "remove") {
                SPDLOG_ERROR("[Sail] Received effect payload with unknown effect type: {}", effectType);
                SendJsonToRemote(responsePayload);
                return;
            }

            responsePayload["status"] = "success";
            SendJsonToRemote(responsePayload);
        } else {
            SPDLOG_ERROR("[Sail] Unknown payload type: {}", payloadType);
            SendJsonToRemote(responsePayload);
            return;
        }

        // If we get here, something went wrong, send the failure response
        SPDLOG_ERROR("[Sail] Failed to handle remote JSON, sending failure response");
        SendJsonToRemote(responsePayload);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[Sail] Exception handling remote JSON: {}", e.what());
    } catch (...) { SPDLOG_ERROR("[Sail] Unknown exception handling remote JSON"); }
}

void Sail::OnConnected() {
    RegisterHooks();
}

void Sail::OnDisconnected() {
    RegisterHooks();
}

void Sail::RegisterHooks() {
    static HOOK_ID onSceneInitHook = 0;
    static HOOK_ID onItemGiveHook = 0;
    static HOOK_ID onActorInitHook = 0;
    static HOOK_ID onFlagSetHook = 0;
    static HOOK_ID onFlagUnsetHook = 0;
    static HOOK_ID onSceneFlagSetHook = 0;
    static HOOK_ID onSceneFlagUnsetHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemGive>(onItemGiveHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagUnset>(onFlagUnsetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagUnset>(onSceneFlagUnsetHook);

    if (!isConnected) {
        return;
    }

    onSceneInitHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([&](s8 sceneId, s8 spawnNum) {
            if (!isConnected)
                return;

            nlohmann::json payload;
            payload["id"] = std::rand();
            payload["type"] = "hook";
            payload["hook"]["type"] = "OnSceneInit";
            payload["hook"]["sceneNum"] = sceneId;

            SendJsonToRemote(payload);
        });
    onItemGiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemGive>([&](u8 item) {
        if (!isConnected)
            return;

        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnItemGive";
        payload["hook"]["itemId"] = item;

        SendJsonToRemote(payload);
    });
    onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([&](void* refActor) {
        if (!isConnected)
            return;

        Actor* actor = (Actor*)refActor;
        nlohmann::json payload;
        payload["id"] = std::rand();
        payload["type"] = "hook";
        payload["hook"]["type"] = "OnActorInit";
        payload["hook"]["actorId"] = actor->id;
        payload["hook"]["params"] = actor->params;

        SendJsonToRemote(payload);
    });
    onFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([&](int16_t flagType, int16_t flag) {
            if (!isConnected)
                return;

            nlohmann::json payload;
            payload["id"] = std::rand();
            payload["type"] = "hook";
            payload["hook"]["type"] = "OnFlagSet";
            payload["hook"]["flagType"] = flagType;
            payload["hook"]["flag"] = flag;

            SendJsonToRemote(payload);
        });
    onFlagUnsetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([&](int16_t flagType, int16_t flag) {
            if (!isConnected)
                return;

            nlohmann::json payload;
            payload["id"] = std::rand();
            payload["type"] = "hook";
            payload["hook"]["type"] = "OnFlagUnset";
            payload["hook"]["flagType"] = flagType;
            payload["hook"]["flag"] = flag;

            SendJsonToRemote(payload);
        });
    onSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(
        [&](int16_t sceneNum, int16_t flagType, int16_t flag) {
            if (!isConnected)
                return;

            nlohmann::json payload;
            payload["id"] = std::rand();
            payload["type"] = "hook";
            payload["hook"]["type"] = "OnSceneFlagSet";
            payload["hook"]["flagType"] = flagType;
            payload["hook"]["flag"] = flag;
            payload["hook"]["sceneNum"] = sceneNum;

            SendJsonToRemote(payload);
        });
    onSceneFlagUnsetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>(
        [&](int16_t sceneNum, int16_t flagType, int16_t flag) {
            if (!isConnected)
                return;

            nlohmann::json payload;
            payload["id"] = std::rand();
            payload["type"] = "hook";
            payload["hook"]["type"] = "OnSceneFlagUnset";
            payload["hook"]["flagType"] = flagType;
            payload["hook"]["flag"] = flag;
            payload["hook"]["sceneNum"] = sceneNum;

            SendJsonToRemote(payload);
        });
}

#endif
