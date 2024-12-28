

#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
void func_808F3D40(EnIn* enIn, PlayState* play);
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

/*
 * This is the same block found for non-scripted actors in OfferGetItem.cpp, with the removal of
 * Player_TalkWithPlayer() and addition of the rando check.
 */
void Rando::ActorBehavior::InitEnInBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_IN) {
            if (*item == GI_MASK_GARO) {
                Player* player = GET_PLAYER(gPlayState);
                *should = false;
                RANDO_SAVE_CHECKS[RC_GORMAN_TRACK_GARO_MASK].eligible = true;
                actor->parent = &player->actor;
                player->talkActor = actor;
                player->talkActorDistance = actor->xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;
            }
        }
    });

    COND_VB_SHOULD(VB_HAVE_GARO_MASK, IS_RANDO, { *should = RANDO_SAVE_CHECKS[RC_GORMAN_TRACK_GARO_MASK].obtained; });
}
