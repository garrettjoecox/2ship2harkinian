#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "overlays/actors/ovl_En_Kitan/z_en_kitan.h"
void func_80C09518(EnKitan* enKitan, PlayState* play);
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnKitanBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_KITAN) {
            *should = false;
            /*
             * Normally this flag gets set using this syntax, which does not trigger rando's FLAG_WEEK_EVENT_REG
             * handling. Even if we used the equivalent SET_WEEKEVENTREG(WEEKEVENTREG_79_80), that would only work once,
             * meaning nothing would be obtained on repeats. So, we set the WEEKEVENTREG flag for the expected game
             * state but also hard set the eligible flag to true to handle repeats.
             */
            gSaveContext.save.saveInfo.weekEventReg[0x4F] |= 0x80;
            RANDO_SAVE_CHECKS[RC_KEATON_QUIZ].eligible = true;
            ((EnKitan*)actor)->actionFunc = func_80C09518;
            Player_TalkWithPlayer(gPlayState, actor);
        }
    });
}
