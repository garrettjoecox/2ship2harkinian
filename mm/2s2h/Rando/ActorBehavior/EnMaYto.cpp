#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ma_Yto/z_en_ma_yto.h"

void EnMaYto_PostMilkRunExplainReward(EnMaYto* enMaYto, PlayState* play);
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnMaYtoBehavior() {
    static uint32_t shouldHook1Id = 0;
    static uint32_t shouldHook2Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook2Id);

    shouldHook1Id = 0;
    shouldHook2Id = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHook1Id = REGISTER_VB_SHOULD(VB_GIVE_ROMANI_MASK, {
        EnMaYto* enMaYto = va_arg(args, EnMaYto*);
        *should = false;
        enMaYto->actionFunc = EnMaYto_PostMilkRunExplainReward;
        RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].eligible = true;
    });

    shouldHook2Id =
        REGISTER_VB_SHOULD(VB_HAVE_ROMANI_MASK, { *should = RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].obtained; });
}
