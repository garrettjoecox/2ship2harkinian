#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnTest5 is initialized this means the player should now be considered to have access to Spring Water. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnTest5Behavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_TEST5, IS_RANDO,
                 [](Actor* actor) { Flags_SetRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER); });
}
