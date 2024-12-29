#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Kinoko/z_obj_kinoko.h"
}

void Rando::ActorBehavior::InitObjKinokoBehavior() {
    COND_VB_SHOULD(VB_GIVE_MAGIC_MUSHROOM, IS_RANDO, {
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (player->heldItemAction == PLAYER_IA_BOTTLE_EMPTY) {
            Flags_SetCollectible(gPlayState, OBJ_KINOKO_GET_FLAG(refActor));
            Actor_Kill(refActor);
        }

        *should = false;
    });
}