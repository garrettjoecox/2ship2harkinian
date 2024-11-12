#include "MiscBehavior.h"

extern "C" {
#include "variables.h"
}

// Entry point for the module, run once on game boot
void Rando::MiscBehavior::Init() {
    Rando::MiscBehavior::InitFileSelect();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(Rando::MiscBehavior::OnFileCreate);
}

void Rando::MiscBehavior::OnFileLoad() {
    Rando::MiscBehavior::CheckQueueReset();
    Rando::MiscBehavior::InitKaleidoItemPage();
    Rando::MiscBehavior::InitOfferGetItemBehavior();

    COND_HOOK(OnFlagSet, IS_RANDO, Rando::MiscBehavior::OnFlagSet);
    COND_HOOK(OnSceneFlagSet, IS_RANDO, Rando::MiscBehavior::OnSceneFlagSet);
    COND_HOOK(BeforeEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::BeforeEndOfCycleSave);
    COND_HOOK(AfterEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::AfterEndOfCycleSave);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });

    // This overrides the ocarina condition for Termina Field
    COND_VB_SHOULD(VB_TERMINA_FIELD_BE_EMPTY, IS_RANDO, { *should = false; });

    // Override "first-cycle" timespeed if you don't have the Ocarina
    COND_HOOK(AfterRoomSceneCommands, IS_RANDO, [](s16 sceneId, s8 roomNum) { if (R_TIME_SPEED == 5) R_TIME_SPEED = 3; });

    // In Sram_OpenSave (right before this code runs) for non-owl saves, it overwrites the entrance to
    // ENTRANCE(CUTSCENE, 0), we need to override that with our starting location (Harcoded to South Clock Town)
    if (!gSaveContext.save.isOwlSave) {
        gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
    }
}
