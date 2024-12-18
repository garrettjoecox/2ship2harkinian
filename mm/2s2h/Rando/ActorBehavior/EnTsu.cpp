#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnTsu is initialized this means the player should now be considered to have access to the Seahorse. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnTsnBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_TSN, IS_RANDO,
                 [](Actor* actor) { Flags_SetRandoInf(RANDO_INF_HAS_ACCESS_SEAHORSE); });
}
