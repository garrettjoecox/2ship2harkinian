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
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onPlayerUpdateHook = 0;
    static uint32_t beforeEndOfCycleSaveHook = 0;
    static uint32_t afterEndOfCycleSaveHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::BeforeEndOfCycleSave>(beforeEndOfCycleSaveHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterEndOfCycleSave>(afterEndOfCycleSaveHook);

    onFlagSetHook = 0;
    onSceneFlagSetHook = 0;
    onPlayerUpdateHook = 0;
    beforeEndOfCycleSaveHook = 0;
    afterEndOfCycleSaveHook = 0;

    Rando::MiscBehavior::CheckQueueReset();

    if (!IS_RANDO) {
        return;
    }

    onFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(Rando::MiscBehavior::OnFlagSet);
    onSceneFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(Rando::MiscBehavior::OnSceneFlagSet);
    onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_PLAYER, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });
    beforeEndOfCycleSaveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::BeforeEndOfCycleSave>(
        Rando::MiscBehavior::BeforeEndOfCycleSave);
    afterEndOfCycleSaveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::AfterEndOfCycleSave>(
        Rando::MiscBehavior::AfterEndOfCycleSave);
}
