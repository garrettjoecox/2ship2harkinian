#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Jg/z_en_jg.h"
}

void Rando::ActorBehavior::InitEnJgBehavior() {
    COND_VB_SHOULD(VB_JG_THINK_YOU_KNOW_LULLABY, IS_RANDO, {
        EnJg* ObjectActor = va_arg(args, EnJg*);
        Player* player = GET_PLAYER(gPlayState);

        // Always consider lullaby known so we don't go into the cutscene to learn it
        *should = true;

        if (!RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].obtained) {
            RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].eligible = true;
        }
    });
}
