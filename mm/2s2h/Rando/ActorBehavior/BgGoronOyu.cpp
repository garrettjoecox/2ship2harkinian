#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When BgGoronOyu is initialized this means the player should now be considered to have access to Hot Spring Water. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitGbGoronOyuBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_BG_GORON_OYU, IS_RANDO, [](Actor* actor) {
        if (!RANDO_ACCESS[RANDO_ACCESS_HOT_SPRING_WATER]) {
            RANDO_ACCESS[RANDO_ACCESS_HOT_SPRING_WATER]++;
        }
    });
}
