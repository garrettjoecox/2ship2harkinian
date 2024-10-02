#include "GameInteractor.h"
#include "spdlog/spdlog.h"
#include <libultraship/bridge.h>
#include <variant>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
void func_800A6A40(EnItem00* thisx, PlayState* play);
extern Vec3f sPlayerGetItemRefPos;
}

void GameInteractor_ExecuteOnGameStateMainFinish() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateMainFinish>();
}

void GameInteractor_ExecuteOnGameStateDrawFinish() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateDrawFinish>();
}

void GameInteractor_ExecuteOnGameStateUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateUpdate>();
}

void GameInteractor_ExecuteOnConsoleLogoUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnConsoleLogoUpdate>();
}

void GameInteractor_ExecuteOnKaleidoUpdate(PauseContext* pauseCtx) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnKaleidoUpdate>(pauseCtx);
}

void GameInteractor_ExecuteBeforeKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeKaleidoDrawPage>(pauseCtx, pauseIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::BeforeKaleidoDrawPage>(pauseIndex, pauseCtx,
                                                                                       pauseIndex);
}

void GameInteractor_ExecuteAfterKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterKaleidoDrawPage>(pauseCtx, pauseIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterKaleidoDrawPage>(pauseIndex, pauseCtx, pauseIndex);
}

void GameInteractor_ExecuteOnSaveInit(s16 fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveInit>(fileNum);
}

void GameInteractor_ExecuteOnSaveLoad(s16 fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveLoad>(fileNum);
}

void GameInteractor_ExecuteOnFileSelectSaveLoad(s16 fileNum, bool isOwlSave, SaveContext* saveContext) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFileSelectSaveLoad>(fileNum, isOwlSave, saveContext);
}

void GameInteractor_ExecuteBeforeEndOfCycleSave() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeEndOfCycleSave>();
}

void GameInteractor_ExecuteAfterEndOfCycleSave() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterEndOfCycleSave>();
}

void GameInteractor_ExecuteBeforeMoonCrashSaveReset() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeMoonCrashSaveReset>();
}

void GameInteractor_ExecuteOnSceneInit(s16 sceneId, s8 spawnNum) {
    SPDLOG_DEBUG("OnSceneInit: sceneId: {}, spawnNum: {}", sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnSceneInit>(sceneId, sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneInit>(sceneId, spawnNum);
}

void GameInteractor_ExecuteOnRoomInit(s16 sceneId, s8 roomNum) {
    SPDLOG_DEBUG("OnRoomInit: sceneId: {}, roomNum: {}", sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnRoomInit>(sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnRoomInit>(sceneId, sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnRoomInit>(sceneId, roomNum);
}

void GameInteractor_ExecuteAfterRoomSceneCommands(s16 sceneId, s8 roomNum) {
    SPDLOG_DEBUG("AfterRoomSceneCommands: sceneId: {}, roomNum: {}", sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterRoomSceneCommands>(sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterRoomSceneCommands>(sceneId, sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::AfterRoomSceneCommands>(sceneId, roomNum);
}

void GameInteractor_ExecuteOnPlayDrawWorldEnd() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDrawWorldEnd>();
}

void GameInteractor_ExecuteOnPlayDestroy() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDestroy>();
}

bool GameInteractor_ShouldActorInit(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorInit>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorInit>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorInit>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorInit>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorInit(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorInit>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorInit>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorInit>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorInit>(actor);
}

bool GameInteractor_ShouldActorUpdate(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorUpdate>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorUpdate>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorUpdate>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorUpdate>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorUpdate(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorUpdate>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorUpdate>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorUpdate>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorUpdate>(actor);
}

bool GameInteractor_ShouldActorDraw(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorDraw>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorDraw>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorDraw>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorDraw>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorDraw(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorDraw>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorDraw>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorDraw>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorDraw>(actor);
}

void GameInteractor_ExecuteOnActorKill(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorKill>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorKill>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorKill>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorKill>(actor);
}

void GameInteractor_ExecuteOnActorDestroy(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorDestroy>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorDestroy>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorDestroy>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorDestroy>(actor);
}

void GameInteractor_ExecuteOnPlayerPostLimbDraw(Player* player, s32 limbIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerPostLimbDraw>(player, limbIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnPlayerPostLimbDraw>(limbIndex, player, limbIndex);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnPlayerPostLimbDraw>(player, limbIndex);
}

void GameInteractor_ExecuteOnSceneFlagSet(s16 sceneId, FlagType flagType, u32 flag) {
    SPDLOG_DEBUG("OnSceneFlagSet: sceneId: {}, flagType: {}, flag: {}", sceneId, (u32)flagType, flag);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagSet>(sceneId, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagSet>(sceneId, flagType, flag);
}

void GameInteractor_ExecuteOnSceneFlagUnset(s16 sceneId, FlagType flagType, u32 flag) {
    SPDLOG_DEBUG("OnSceneFlagUnset: sceneId: {}, flagType: {}, flag: {}", sceneId, (u32)flagType, flag);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagUnset>(sceneId, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagUnset>(sceneId, flagType, flag);
}

void GameInteractor_ExecuteOnFlagSet(FlagType flagType, u32 flag) {
    // This flag in particular is very spammy, so we'll suppress it
    if (!(flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80)) {
        SPDLOG_DEBUG("OnFlagSet: flagType: {}, flag: {}", (u32)flagType, flag);
    }
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagSet>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagSet>(flagType, flag);
}

void GameInteractor_ExecuteOnFlagUnset(FlagType flagType, u32 flag) {
    // This flag in particular is very spammy, so we'll suppress it
    if (!(flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80)) {
        SPDLOG_DEBUG("OnFlagUnset: flagType: {}, flag: {}", (u32)flagType, flag);
    }
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagUnset>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagUnset>(flagType, flag);
}

void GameInteractor_ExecuteOnCameraChangeModeFlags(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnCameraChangeModeFlags>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnCameraChangeModeFlags>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnCameraChangeModeFlags>((uintptr_t)camera, camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnCameraChangeModeFlags>(camera);
}

void GameInteractor_ExecuteAfterCameraUpdate(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterCameraUpdate>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterCameraUpdate>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::AfterCameraUpdate>((uintptr_t)camera, camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::AfterCameraUpdate>(camera);
}

void GameInteractor_ExecuteOnCameraChangeSettingsFlags(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnCameraChangeSettingsFlags>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnCameraChangeSettingsFlags>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnCameraChangeSettingsFlags>((uintptr_t)camera,
                                                                                              camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnCameraChangeSettingsFlags>(camera);
}

void GameInteractor_ExecuteOnPassPlayerInputs(Input* input) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPassPlayerInputs>(input);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnPassPlayerInputs>(input);
}

void GameInteractor_ExecuteOnOpenText(u16* textId) {
    SPDLOG_DEBUG("OnOpenText: textId: {}", *textId);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnOpenText>(textId);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnOpenText>(*textId, textId);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnOpenText>(textId);
}

void GameInteractor_ExecuteOnHandleCustomMessage(s32 modId, s32 textId, std::string* msg) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnHandleCustomMessage>(modId, textId, msg);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnHandleCustomMessage>(modId, textId, msg);
}

bool GameInteractor_ShouldItemGive(u8 item) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldItemGive>(item, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldItemGive>(item, item, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldItemGive>(item, &result);
    return result;
}

void GameInteractor_ExecuteOnItemGive(u8 item) {
    SPDLOG_DEBUG("OnItemGive: item: {}", item);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnItemGive>(item);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnItemGive>(item, item);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnItemGive>(item);
}

bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...) {
    // Only the external function can use the Variadic Function syntax
    // To pass the va args to the next caller must be done using va_list and reading the args into it
    // Because there can be N subscribers registered to each template call, the subscribers will be responsible for
    // creating a copy of this va_list to avoid incrementing the original pointer between calls
    va_list args;
    va_start(args, result);

    // Because of default argument promotion, even though our incoming "result" is just a bool, it needs to be typed as
    // an int to be permitted to be used in `va_start`, otherwise it is undefined behavior.
    // Here we downcast back to a bool for our actual hook handlers
    bool boolResult = static_cast<bool>(result);

    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldVanillaBehavior>(flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldVanillaBehavior>(flag, flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldVanillaBehavior>(flag, &boolResult, args);

    va_end(args);
    return boolResult;
}

// Returns 1 or -1 based on a number of factors like CVars or other game states.
int GameInteractor_InvertControl(GIInvertType type) {
    int result = 1;

    switch (type) {
        case GI_INVERT_CAMERA_RIGHT_STICK_X:
            if (CVarGetInteger("gEnhancements.Camera.RightStick.InvertXAxis", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_CAMERA_RIGHT_STICK_Y:
            if (CVarGetInteger("gEnhancements.Camera.RightStick.InvertYAxis", 1)) {
                result *= -1;
            }
            break;
    }

    /*
    // Invert all X axis inputs if the Mirrored World mode is enabled
    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        switch (type) {
            case GI_INVERT_CAMERA_RIGHT_STICK_X:
                result *= -1;
                break;
        }
    }
    */

    /*
    if (CrowdControl::State::InvertedInputs) {
        result *= -1;
    }
    */

    return result;
}

uint32_t GameInteractor_Dpad(GIDpadType type, uint32_t buttonCombo) {
    uint32_t result = 0;

    switch (type) {
        case GI_DPAD_OCARINA:
            if (CVarGetInteger("gEnhancements.Playback.DpadOcarina", 0)) {
                result = buttonCombo;
            }
            break;
        case GI_DPAD_EQUIP:
            if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                result = buttonCombo;
            }
            break;
    }

    return result;
}

void EnItem00_DrawCustomForOverHead(Actor* thisx, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)thisx;
    thisx->shape.rot.y += 0x3C0;
    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
    f32 height = -30.0f;
    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_DEKU:
            height = -50.0f;
            break;
        case PLAYER_FORM_FIERCE_DEITY:
            height = 120.0f;
            break;
        case PLAYER_FORM_GORON:
            height = 45.0f;
            break;
        case PLAYER_FORM_ZORA:
            height = 30.0f;
            break;
    }
    Matrix_Translate(0.0f, height, 0.0f, MTXMODE_APPLY);
    GetItem_Draw(play, GID_SHIP);
}

void GameInteractor_ProcessEvents(Actor* actor) {
    Player* player = GET_PLAYER(gPlayState);

    // If the player has a message active, stop
    if (gPlayState->msgCtx.msgMode != 0) {
        return;
    }

    // If the player is in a blocking cutscene, stop
    if (Player_InBlockingCsMode(gPlayState, player)) {
        return;
    }

    // If player is dead or in first person, stop
    if (player->stateFlags1 & (PLAYER_STATE1_80 | PLAYER_STATE1_100000)) {
        return;
    }

    // If holding an item, stop
    if ((Player_GetExplosiveHeld(player) > PLAYER_EXPLOSIVE_NONE)) {
        return;
    }

    // If there is an event active, stop
    const auto& currentEvent = GameInteractor::Instance->currentEvent;
    if (auto e = std::get_if<GIEventNone>(&currentEvent)) {
        // no-op
    } else {
        // if the event is a give item event with a cutscene and the player is no longer receiving the item, clear it
        if (auto e = std::get_if<GIEventGiveItem>(&currentEvent)) {
            if (e->showGetItemCutscene && player->getItemId == GI_NONE) {
                GameInteractor::Instance->currentEvent = GIEventNone{};
            }
        }
        return;
    }

    // If there are no events that need to happen, stop
    if (GameInteractor::Instance->events.empty()) {
        return;
    }

    GameInteractor::Instance->currentEvent = GameInteractor::Instance->events.front();
    const auto& nextEvent = GameInteractor::Instance->currentEvent;

    if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
        // If the player is climbing or in the air, deliver the item without a cutscene but freeze the player
        if (!e->showGetItemCutscene ||
            (player->stateFlags1 & (PLAYER_STATE1_1000 | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_40000 |
                                    PLAYER_STATE1_80000 | PLAYER_STATE1_200000))) {
            EnItem00* enItem00 =
                (EnItem00*)Item_DropCollectible(gPlayState, &actor->world.pos, ITEM00_NOTHING | 0x8000);
            Audio_PlaySfx(NA_SE_SY_GET_ITEM);
            CustomMessage_StartTextbox(gPlayState, MOD_ID_SHIP, SHIP_TEXT_GIVE_ITEM_NO_STOP, &enItem00->actor);
            e->giveItem();
            enItem00->actor.draw = EnItem00_DrawCustomForOverHead;
            if (e->showGetItemCutscene) {
                player->actor.freezeTimer = 30;
            }
        } else {
            EnItem00* enItem00 = (EnItem00*)Item_DropCollectible(gPlayState, &actor->world.pos, ITEM00_NOTHING);
            enItem00->getItemId = GI_SHIP;
            enItem00->actionFunc = func_800A6A40;
            CustomMessage_SetActiveMessage(MOD_ID_SHIP, SHIP_TEXT_GIVE_ITEM);
            Actor_OfferGetItem(&enItem00->actor, gPlayState, GI_SHIP, 50.0f, 80.0f);
            enItem00->actionFunc(enItem00, gPlayState);
            e->giveItem();
        }
    } else if (auto e = std::get_if<GIEventTransition>(&nextEvent)) {
        gPlayState->nextEntrance = e->entrance;
        gSaveContext.nextCutsceneIndex = e->cutsceneIndex;
        gPlayState->transitionTrigger = e->transitionTrigger;
        gPlayState->transitionType = e->transitionType;
    }

    GameInteractor::Instance->events.erase(GameInteractor::Instance->events.begin());
}

void GameInteractor::Init() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER,
                                                                                   GameInteractor_ProcessEvents);

    REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_ITEM00, {
        EnItem00* item00 = va_arg(args, EnItem00*);
        if (item00->actor.params == ITEM00_NOTHING || item00->actor.params == (ITEM00_NOTHING | 0x8000)) {
            *should = false;
        }
    });

    // If it's a give item event without a cutscene, clear the current event
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorKill>(ACTOR_EN_ITEM00, [](Actor* actor) {
        if (actor->params == 24) {
            const auto& currentEvent = GameInteractor::Instance->currentEvent;
            if (auto e = std::get_if<GIEventGiveItem>(&currentEvent)) {
                if (!e->showGetItemCutscene) {
                    GameInteractor::Instance->currentEvent = GIEventNone{};
                }
            }
        }
    });

    // If there is a transition event active, clear it
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s8 sceneId, s8 spawnNum) {
        const auto& currentEvent = GameInteractor::Instance->currentEvent;
        if (auto e = std::get_if<GIEventTransition>(&currentEvent)) {
            GameInteractor::Instance->currentEvent = GIEventNone{};
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnHandleCustomMessage>(
        [](s32 modId, s32 textId, std::string* msg) {
            // Replaces {{item}} in msg arg with currentEvent getItemText
            const auto& currentEvent = GameInteractor::Instance->currentEvent;
            if (auto e = std::get_if<GIEventGiveItem>(&currentEvent)) {
                CustomMessage_Replace(msg, "{{item}}", e->getItemText);
            }
        });
}

void GameInteractor_GetItemDraw(PlayState* play, s16 drawId) {
    const auto& currentEvent = GameInteractor::Instance->currentEvent;
    if (auto e = std::get_if<GIEventGiveItem>(&currentEvent)) {
        e->drawItem();
    }
}
