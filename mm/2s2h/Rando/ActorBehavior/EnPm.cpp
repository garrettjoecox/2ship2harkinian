#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
void func_80837B60(PlayState* play, Player* player);
s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
}

void Rando::ActorBehavior::InitEnPmBehavior() {
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_PM) {    // MSCRIPT_OFFER_ITEM from Postman
            if (cmdId == MSCRIPT_CMD_06) { // MSCRIPT_OFFER_ITEM
                /*
                 * The notebook event is the very last thing that happens in the Postman script. The short of it is
                 * that, if no notebook message actually appears, the func_80832558 call will softlock since there will
                 * be no textbox close to wait on. This check is equivalent to anticipating that at least one of these
                 * two Postman notebook messages will appear at the end of this interaction.
                 */
                if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK) &&
                    !(CHECK_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN) &&
                      CHECK_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMANS_HAT))) {
                    Player* player = GET_PLAYER(gPlayState);
                    func_80832558(gPlayState, player, func_80837B60);
                }
                *should = false;
            } else if (cmdId == MSCRIPT_CMD_BRANCH_ON_ITEM) {
                MsgScript* script = va_arg(args, MsgScript*);
                ItemId itemId = (ItemId)MSCRIPT_GET_16(script, 1);
                /*
                 * The Postman's Hat check does a branch if the player already has the Postman's Hat in their inventory.
                 * In rando, it is possible to already have the Postman's Hat before ever giving him the Express Mail,
                 * so we're just going to always act as if the player does not have the Postman's Hat. This enables
                 * repeat rewards as well, which does slightly differ from vanilla.
                 */
                if (itemId == ITEM_MASK_POSTMAN) {
                    *should = false;
                }
            }
        }
    });
}
