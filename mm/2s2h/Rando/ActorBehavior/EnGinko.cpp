#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"

void EnGinkoMan_SetupIdle(EnGinkoMan* enGinkoMan);
}

void Rando::ActorBehavior::InitEnGinkoBehavior() {
    COND_VB_SHOULD(VB_BANKER_GIVE_REWARD, IS_RANDO, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        EnGinkoMan_SetupIdle(enGinkoMan);

        if (GameInteractor_Should(VB_PASS_FIRST_BANK_THRESHOLD,
                                  (HS_GET_BANK_RUPEES() >= 200) && (enGinkoMan->previousBankValue < 200), enGinkoMan)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_10_08);
        }

        if (GameInteractor_Should(VB_PASS_INTEREST_BANK_THRESHOLD,
                                  (HS_GET_BANK_RUPEES() >= 1000) && (enGinkoMan->previousBankValue < 1000),
                                  enGinkoMan) &&
            !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].obtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].eligible = true;
        }

        if (GameInteractor_Should(VB_PASS_SECOND_BANK_THRESHOLD, HS_GET_BANK_RUPEES() >= 5000, enGinkoMan)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_59_08);
        }

        *should = false;
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_GINKO_MAN, IS_RANDO, [](Actor* actor) {
        EnGinkoMan* refActor = (EnGinkoMan*)actor;

        // Set New Account
        refActor->isNewAccount = false;
        if (HS_GET_BANK_RUPEES() == 0) {
            HS_SET_BANK_RUPEES(1);
        }
    });

    COND_VB_SHOULD(VB_CONTINUE_BANKER_DIALOGUE, IS_RANDO, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        // Initial Banter
        if (enGinkoMan->curTextId == 0 || enGinkoMan->curTextId == 0x44c || enGinkoMan->curTextId == 0x457) {
            Message_StartTextbox(gPlayState, 0x466, &enGinkoMan->actor);
            enGinkoMan->curTextId = 0x466;
            *should = false;
        }

        // Deposit Dialogue
        if (enGinkoMan->curTextId == 0x469 && enGinkoMan->choiceDepositWithdrawl == GINKOMAN_CHOICE_DEPOSIT) {
            enGinkoMan->curTextId = 0x44f;
            *should = false;
        }

        if (enGinkoMan->curTextId == 0x454 || enGinkoMan->curTextId == 0x453) {
            enGinkoMan->curTextId = 0x455;
            *should = false;
        }
    });
}