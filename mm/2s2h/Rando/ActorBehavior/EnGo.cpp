#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Go/z_en_go.h"

void Player_TalkWithPlayer(PlayState* play, Actor* actor);
void func_80837B60(PlayState* play, Player* player);
s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
}

static std::vector<u8> skipCmds = {};

void Rando::ActorBehavior::InitEnGoBehavior() {

    COND_ID_HOOK(OnActorInit, ACTOR_EN_GO, IS_RANDO, [](Actor* actor) { skipCmds.clear(); });

    // Scripted Actors
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        MsgScript* script = va_arg(args, MsgScript*);
        Player* player = GET_PLAYER(gPlayState);

        if (actor->id != ACTOR_EN_GO || ENGO_GET_TYPE(actor) != ENGO_MEDIGORON) {
            return;
        }

        // We no longer want Medigoron to set the Powder Keg privileges flag, we set this when getting RI_POWDER_KEG
        if (cmdId == MSCRIPT_CMD_17 && script[1] == 18 && script[2] == 0x80) {
            *should = false;
            return;
        }

        if (cmdId == MSCRIPT_CMD_06) { // MSCRIPT_OFFER_ITEM
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES)) {
                func_80832558(gPlayState, player, func_80837B60);
                *should = false;
                skipCmds.clear();
                skipCmds.push_back(MSCRIPT_CMD_07);
                skipCmds.push_back(MSCRIPT_CMD_12);
                return;
            }
        }

        if (skipCmds.empty()) {
            return;
        }

        if (cmdId == skipCmds[0]) {
            skipCmds.erase(skipCmds.begin());
            *should = false;
        }
    });
}
