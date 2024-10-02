#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnOsnBehavior() {
    static uint32_t shouldHook1Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);

    shouldHook1Id = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHook1Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_OSN, {
        *should = false;

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_DEKU_MASK].eligible = true;
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_SONG_OF_HEALING].eligible = true;
    });
}
