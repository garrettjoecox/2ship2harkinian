#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// Registers hooks in both z_obj_um.c and z_en_ma_yto.c
void Rando::ActorBehavior::InitObjUmBehavior() {
    static uint32_t onActorUpdateHookId = 0;
    static uint32_t shouldHookId1 = 0;
    static uint32_t shouldHookId2 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId2);

    shouldHookId1 = 0;
    shouldHookId2 = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHookId1 =
        REGISTER_VB_SHOULD(VB_HAVE_ROMANI_MASK, { *should = RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].obtained; });
    shouldHookId2 = REGISTER_VB_SHOULD(VB_WON_MILK_RUN, {
        if (RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].obtained) {
            return;
        }

        *should = false;
        // Combination of things set between obj_um.c and en_ma_yto.c
        gSaveContext.nextCutsceneIndex = 0;
        gPlayState->nextEntrance = ENTRANCE(TERMINA_FIELD, 13);
        gPlayState->transitionType = TRANS_TYPE_80;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gSaveContext.save.time += CLOCK_TIME(1, 0) + 2;
        if (!RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible) {
            RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible = true;
        }
    });
}
