#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "functions.h"
}

void RegisterSkipPushingMikau() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(ACTOR_EN_ZOG, [](Actor* actor, bool* should) {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
            Flags_SetWeekEventReg(WEEKEVENTREG_88_10);
            Flags_SetWeekEventReg(WEEKEVENTREG_29_20);
            Flags_SetWeekEventReg(WEEKEVENTREG_91_01);
        }
    });
}
