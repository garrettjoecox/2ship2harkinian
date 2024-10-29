#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Baba/z_en_baba.h"
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
void EnBaba_GaveBlastMask(EnBaba* enBaba, PlayState* play);
}

#define BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK 64

// The Bomb Shop Lady's item give is non-scripted, but the catch-all for VB_GIVE_ITEM_FROM_OFFER does not work for
// this case, as Link can move freely once the next textbox appears. This code fixes that.
void Rando::ActorBehavior::InitEnBabaBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        *should = false;

        GetItemId* item = va_arg(args, GetItemId*);
        EnBaba* enBaba = va_arg(args, EnBaba*);
        Player* player = GET_PLAYER(gPlayState);
        enBaba->stateFlags |= BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK;
        enBaba->actionFunc = EnBaba_GaveBlastMask;
        enBaba->actor.parent = &player->actor;
        player->talkActor = &enBaba->actor;
        player->talkActorDistance = enBaba->actor.xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_TalkWithPlayer(gPlayState, &enBaba->actor);
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_NORTH_BOMB_LADY].eligible = true;
    });
}
