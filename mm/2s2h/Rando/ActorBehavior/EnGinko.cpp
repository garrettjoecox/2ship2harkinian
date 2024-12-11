#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"
}

void Rando::ActorBehavior::InitEnGinkoBehavior() {
    COND_VB_SHOULD(VB_BANKER_GIVE_REWARD, IS_RANDO, {
        EnGinkoMan* refActor = va_arg(args, EnGinkoMan*);

        if (HS_GET_BANK_RUPEES() >= 200 && !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET].obtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET].eligible = true;
        } else if (HS_GET_BANK_RUPEES() >= 1000 && !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].obtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].eligible = true;
        } else if (HS_GET_BANK_RUPEES() >= 5000 && !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_HP].obtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_HP].eligible = true;
        }
        *should = false;
    });
}