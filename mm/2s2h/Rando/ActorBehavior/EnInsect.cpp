#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnInsect is initialized this means the player should now be considered to have access to Bugs. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnInsectBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_INSECT, IS_RANDO, [](Actor* actor) {
        if (!RANDO_ACCESS[RANDO_ACCESS_BUGS]) {
            RANDO_ACCESS[RANDO_ACCESS_BUGS]++;
        }
    });
}
