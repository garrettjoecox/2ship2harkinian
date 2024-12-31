#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Fsn/z_en_fsn.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
void EnFsn_ResumeInteraction(EnFsn* enFsn, PlayState* play);
}

void Rando::ActorBehavior::InitEnFsnBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_FSN) { // Curiosity Shop owner
            EnFsn* enFsn = (EnFsn*)actor;
            // Handling for when the Curiosity Shop owner sells something to the player
            if (enFsn->isSelling && enFsn->cursorIndex >= 0 && enFsn->cursorIndex <= 2) {
                EnGirlA* enGirlA = enFsn->items[enFsn->cursorIndex];
                RandoCheckId randoCheckId = (RandoCheckId)enGirlA->actor.world.rot.z;
                // Only handle the two special checks. Leave stolen items as-is.
                if (randoCheckId == RC_CURIOSITY_SHOP_SPECIAL_ITEM ||
                    randoCheckId == RC_BOMB_SHOP_ITEM_OR_CURIOSITY_SHOP_ITEM) {
                    *should = false;
                    Player* player = GET_PLAYER(gPlayState);
                    player->talkActor = actor;
                    player->talkActorDistance = actor->xzDistToPlayer;
                    Player_TalkWithPlayer(gPlayState, actor);
                    actor->flags |= ACTOR_FLAG_TALK_REQUESTED;
                    enFsn->actionFunc = EnFsn_ResumeInteraction;
                    enGirlA->buyFunc(gPlayState, enGirlA);
                    /*
                     * This notebook event must be faked because the randomized item probably won't be the All-Night
                     * Mask. There exists a minor bug where, if the player tries to sell something immediately after
                     * buying the special item, this notebook event will pop as Link pulls out the item to show. The
                     * Curiosity Shop owner's response will then erase that textbox. Not game breaking, but something to
                     * note.
                     */
                    if (randoCheckId == RC_CURIOSITY_SHOP_SPECIAL_ITEM) {
                        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK);
                    }
                }
            }
        }
    });
}
