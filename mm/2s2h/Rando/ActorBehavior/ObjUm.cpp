#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

void Rando::ActorBehavior::InitObjUmBehavior() {
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
    shouldHookId2 = REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].obtained) {
            return;
        }
        if (!(gSaveContext.save.cutsceneIndex == 0xFFF3 && gSaveContext.save.entrance == ENTRANCE(TERMINA_FIELD, 0))) {
            return;
        }
        gSaveContext.save.cutsceneIndex = 0;
        gSaveContext.save.entrance = ENTRANCE(TERMINA_FIELD, 13);
        if (!RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible) {
            RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible = true;
        }
    });
}
