#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "overlays/actors/ovl_En_Gk/z_en_gk.h"

extern "C" {
#include "variables.h"

void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnGKBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        switch (actor->id) {
            case ACTOR_EN_GK:
                if (RANDO_SAVE_CHECKS[RC_GORON_RACETRACK_GOLD_DUST].obtained) {
                    return;
                }

                *should = false;
                actor->parent = &player->actor;
                player->talkActor = actor;
                player->talkActorDistance = actor->xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;
                Player_TalkWithPlayer(gPlayState, actor);
                break;
        }
    });

    COND_VB_SHOULD(VB_TEACH_GORON_LULLABY, IS_RANDO, {
        EnGk* ObjActor = va_arg(args, EnGk*);
        PlayState* play = va_arg(args, PlayState*);
        Player* player = GET_PLAYER(gPlayState);
        if (!RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].obtained) {
            if (ObjActor->actor.xzDistToPlayer < 100.0f) {
                if ((player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
                    (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY_INTRO)) {
                    RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].eligible = true;
                }
            }
        }
        *should = false;
    });

    COND_VB_SHOULD(VB_GIVE_LULLABY_CHECK_IF_ALREADY_KNOWN, IS_RANDO, {
        if (!RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].obtained) {
            RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].eligible = true;
        }
        *should = true;
    });
}