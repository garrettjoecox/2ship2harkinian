#include "Sail.h"
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "variables.h"
}

std::unordered_map<std::string, std::unordered_map<int32_t, HOOK_ID>> filteredHookIds;
std::unordered_map<std::string, HOOK_ID> hookIds;

void OnSceneInitHandler(s8 sceneId, s8 spawnNum) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnSceneInit";
    payload["hook"]["sceneNum"] = sceneId;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnItemGiveHandler(u8 item) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnItemGive";
    payload["hook"]["itemId"] = item;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnActorInitHandler(void* refActor) {
    if (!Sail::Instance->isConnected)
        return;

    Actor* actor = (Actor*)refActor;
    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnActorInit";
    payload["hook"]["actorId"] = actor->id;
    payload["hook"]["params"] = actor->params;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnFlagSetHandler(int16_t flagType, int16_t flag) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnFlagSet";
    payload["hook"]["flagType"] = flagType;
    payload["hook"]["flag"] = flag;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnFlagUnsetHandler(int16_t flagType, int16_t flag) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnFlagUnset";
    payload["hook"]["flagType"] = flagType;
    payload["hook"]["flag"] = flag;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnSceneFlagSetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnSceneFlagSet";
    payload["hook"]["flagType"] = flagType;
    payload["hook"]["flag"] = flag;
    payload["hook"]["sceneNum"] = sceneNum;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void OnSceneFlagUnsetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    if (!Sail::Instance->isConnected)
        return;

    nlohmann::json payload;
    payload["id"] = std::rand();
    payload["type"] = "hook";
    payload["hook"]["type"] = "OnSceneFlagUnset";
    payload["hook"]["flagType"] = flagType;
    payload["hook"]["flag"] = flag;
    payload["hook"]["sceneNum"] = sceneNum;

    Sail::Instance->QueueOutgoingPacket(payload);
}

void Sail::Enable() {
    Network::Enable(CVarGetString("gNetwork.Sail.Host", "127.0.0.1"), CVarGetInteger("gNetwork.Sail.Port", 43384));
}

#define HANDLE_ID_SUBSCRIBE(hookType)                                                                                 \
    {                                                                                                                 \
        if (eventName == #hookType) {                                                                                 \
            if (payload.contains("eventIdFilter")) {                                                                  \
                int32_t eventIdFilter = payload["eventIdFilter"].get<int32_t>();                                      \
                if (!filteredHookIds[eventName].contains(eventIdFilter)) {                                            \
                    filteredHookIds[eventName][eventIdFilter] =                                                       \
                        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::hookType>(eventIdFilter,      \
                                                                                                  hookType##Handler); \
                }                                                                                                     \
            } else {                                                                                                  \
                if (!hookIds.contains(eventName)) {                                                                   \
                    hookIds[eventName] =                                                                              \
                        GameInteractor::Instance->RegisterGameHook<GameInteractor::hookType>(hookType##Handler);      \
                }                                                                                                     \
            }                                                                                                         \
            responsePayload["status"] = "success";                                                                    \
        }                                                                                                             \
    }

#define HANDLE_SUBSCRIBE(hookType)                                                                           \
    {                                                                                                        \
        if (eventName == #hookType) {                                                                        \
            if (!hookIds.contains(eventName)) {                                                              \
                hookIds[eventName] =                                                                         \
                    GameInteractor::Instance->RegisterGameHook<GameInteractor::hookType>(hookType##Handler); \
            }                                                                                                \
            responsePayload["status"] = "success";                                                           \
        }                                                                                                    \
    }

#define HANDLE_ID_UNSUBSCRIBE(hookType)                                                                         \
    {                                                                                                           \
        if (eventName == #hookType) {                                                                           \
            if (payload.contains("eventIdFilter")) {                                                            \
                int32_t eventIdFilter = payload["eventIdFilter"].get<int32_t>();                                \
                if (filteredHookIds[eventName].contains(eventIdFilter)) {                                       \
                    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::hookType>(                \
                        filteredHookIds[eventName][eventIdFilter]);                                             \
                    filteredHookIds[eventName].erase(eventIdFilter);                                            \
                }                                                                                               \
            } else {                                                                                            \
                if (hookIds.contains(eventName)) {                                                              \
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::hookType>(hookIds[eventName]); \
                    hookIds.erase(eventName);                                                                   \
                }                                                                                               \
            }                                                                                                   \
            responsePayload["status"] = "success";                                                              \
        }                                                                                                       \
    }

#define HANDLE_UNSUBSCRIBE(hookType)                                                                        \
    {                                                                                                       \
        if (eventName == #hookType) {                                                                       \
            if (hookIds.contains(eventName)) {                                                              \
                GameInteractor::Instance->UnregisterGameHook<GameInteractor::hookType>(hookIds[eventName]); \
                hookIds.erase(eventName);                                                                   \
            }                                                                                               \
            responsePayload["status"] = "success";                                                          \
        }                                                                                                   \
    }

void Sail::OnIncomingJson(nlohmann::json payload) {
    nlohmann::json responsePayload;
    responsePayload["type"] = "result";
    responsePayload["status"] = "failure";

    try {
        if (!payload.contains("id")) {
            SPDLOG_ERROR("[Sail] Received payload without ID");
            QueueOutgoingPacket(responsePayload);
            return;
        }

        responsePayload["id"] = payload["id"];

        if (!payload.contains("type")) {
            SPDLOG_ERROR("[Sail] Received payload without type");
            QueueOutgoingPacket(responsePayload);
            return;
        }

        std::string payloadType = payload["type"].get<std::string>();

        if (payloadType == "command") {
            if (!payload.contains("command")) {
                SPDLOG_ERROR("[Sail] Received command payload without command");
                QueueOutgoingPacket(responsePayload);
                return;
            }

            std::string command = payload["command"].get<std::string>();
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch(command);
            responsePayload["status"] = "success";
            QueueOutgoingPacket(responsePayload);
            return;
        } else if (payloadType == "effect") {
            if (!payload.contains("effect") || !payload["effect"].contains("type")) {
                SPDLOG_ERROR("[Sail] Received effect payload without effect type");
                QueueOutgoingPacket(responsePayload);
                return;
            }

            std::string effectType = payload["effect"]["type"].get<std::string>();

            // Special case for "command" effect, so we can also run commands from the `simple_twitch_sail` script
            if (effectType == "command") {
                if (!payload["effect"].contains("command")) {
                    SPDLOG_ERROR("[Sail] Received command effect payload without command");
                    QueueOutgoingPacket(responsePayload);
                    return;
                }

                std::string command = payload["effect"]["command"].get<std::string>();
                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                    ->Dispatch(command);
                responsePayload["status"] = "success";
                QueueOutgoingPacket(responsePayload);
                return;
            }

            if (effectType == "teleport") {
                if (gPlayState == NULL) {
                    SPDLOG_ERROR("[Sail] Teleport failed, no gPlayState");
                    QueueOutgoingPacket(responsePayload);
                    return;
                }

                if (!payload["effect"].contains("entranceId")) {
                    SPDLOG_ERROR("[Sail] Received teleport effect payload without entranceId");
                    QueueOutgoingPacket(responsePayload);
                    return;
                }

                gPlayState->nextEntrance = payload["effect"]["entranceId"].get<u16>();
                gSaveContext.nextCutsceneIndex = 0;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_INSTANT;
            }

            if (effectType != "apply" && effectType != "remove") {
                SPDLOG_ERROR("[Sail] Received effect payload with unknown effect type: {}", effectType);
                QueueOutgoingPacket(responsePayload);
                return;
            }

            responsePayload["status"] = "success";
            QueueOutgoingPacket(responsePayload);
        } else if (payloadType == "subscribe") {
            if (!payload.contains("eventName")) {
                SPDLOG_ERROR("[Sail] Received subscribe payload without eventName");
                QueueOutgoingPacket(responsePayload);
                return;
            }

            std::string eventName = payload["eventName"].get<std::string>();

            HANDLE_ID_SUBSCRIBE(OnSceneInit);
            HANDLE_ID_SUBSCRIBE(OnItemGive);
            HANDLE_ID_SUBSCRIBE(OnActorInit);
            HANDLE_SUBSCRIBE(OnFlagSet);
            HANDLE_SUBSCRIBE(OnFlagUnset);
            HANDLE_SUBSCRIBE(OnSceneFlagSet);
            HANDLE_SUBSCRIBE(OnSceneFlagUnset);

            responsePayload["status"] = "success";
            QueueOutgoingPacket(responsePayload);
            return;
        } else if (payloadType == "unsubscribe") {
            if (!payload.contains("eventName")) {
                SPDLOG_ERROR("[Sail] Received unsubscribe payload without eventName");
                QueueOutgoingPacket(responsePayload);
                return;
            }

            std::string eventName = payload["eventName"].get<std::string>();

            HANDLE_ID_UNSUBSCRIBE(OnSceneInit);
            HANDLE_ID_UNSUBSCRIBE(OnItemGive);
            HANDLE_ID_UNSUBSCRIBE(OnActorInit);
            HANDLE_UNSUBSCRIBE(OnFlagSet);
            HANDLE_UNSUBSCRIBE(OnFlagUnset);
            HANDLE_UNSUBSCRIBE(OnSceneFlagSet);
            HANDLE_UNSUBSCRIBE(OnSceneFlagUnset);

            QueueOutgoingPacket(responsePayload);
            return;
        } else {
            SPDLOG_ERROR("[Sail] Unknown payload type: {}", payloadType);
            QueueOutgoingPacket(responsePayload);
            return;
        }

        // If we get here, something went wrong, send the failure response
        SPDLOG_ERROR("[Sail] Failed to handle remote JSON, sending failure response");
        QueueOutgoingPacket(responsePayload);
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
    COND_HOOK(OnGameStateMainStart, isConnected, []() {
        std::queue<nlohmann::json> packetQueue;
        Sail::Instance->SwapIncomingPacketQueue(packetQueue);

        while (!packetQueue.empty()) {
            nlohmann::json payload = packetQueue.front();
            packetQueue.pop();
            Sail::Instance->OnIncomingJson(payload);
        }
    });

    if (!isConnected) {
        for (auto& [eventName, hookId] : hookIds) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(hookId);
        }
        hookIds.clear();

        for (auto& [eventName, filteredHookId] : filteredHookIds) {
            for (auto& [eventIdFilter, hookId] : filteredHookId) {
                GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnSceneInit>(hookId);
            }
        }
        filteredHookIds.clear();
    }
}
