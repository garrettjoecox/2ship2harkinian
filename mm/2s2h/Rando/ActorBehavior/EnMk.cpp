#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

// This interaction is skipped by the SkipLearningNewWaveBossaNova and forced on for rando for now, 
// this file simply handles queuing up the check to be given.
void Rando::ActorBehavior::InitEnMkBehavior() {
    REGISTER_VB_SHOULD(VB_GIVE_NEW_WAVE_BOSSA_NOVA, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) || IS_RANDO) {
            *should = true;
            // TO-DO: uncomment after shuffle is implemented.
            //if (IS_RANDO && RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA].shuffled) {
            //    if (!RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA].obtained) {
            //        RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA].eligible = true;
            //    }
            //}
        }
    });
}