#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Kujiya/z_en_kujiya.h"
void EnKujiya_Wait(EnKujiya* enKujiya, PlayState* play);
}

void Rando::ActorBehavior::InitEnKujiyaBehavior() {
    COND_VB_SHOULD(VB_GIVE_LOTTERY_WINNINGS, IS_RANDO, {
        EnKujiya* refActor = va_arg(args, EnKujiya*);

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].eligible = true;
        refActor->actionFunc = EnKujiya_Wait;
        *should = false;
    });
}