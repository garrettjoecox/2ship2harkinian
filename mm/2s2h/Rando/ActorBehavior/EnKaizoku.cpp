#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnKaizoku is initialized this means the player should now be considered to have access to a Pirate. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnKaizokuBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_KAIZOKU, IS_RANDO,
                 [](Actor* actor) { Flags_SetRandoInf(RANDO_INF_CAN_TAKE_PICTURE_OF_PIRATE); });
}
