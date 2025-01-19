#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Trt/z_en_trt.h"
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
void EnTrt_ItemGiven(EnTrt* enTrt, PlayState* play);
}

void Rando::ActorBehavior::InitEnTrtBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        EnTrt* refActor = va_arg(args, EnTrt*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->actor.id != ACTOR_EN_TRT && refActor->actor.id != ACTOR_EN_TRT2) {
            return;
        }
        if (!RANDO_SAVE_CHECKS[RC_HAGS_POTION_SHOP_KOTAKE].shuffled) {
            return;
        }

        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE);

        *should = false;
        refActor->actor.parent = &player->actor;
        player->talkActor = &refActor->actor;
        player->talkActorDistance = refActor->actor.xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;

        if (refActor->actor.id == ACTOR_EN_TRT) {
            Player_TalkWithPlayer(gPlayState, &refActor->actor);
            refActor->actionFunc = EnTrt_ItemGiven;
        }
    });
}
