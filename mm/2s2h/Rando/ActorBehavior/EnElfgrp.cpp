#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Elfgrp/z_en_elfgrp.h"
}

// Handles the Great Fairy checks
void Rando::ActorBehavior::InitEnElfgrpBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, IS_RANDO, {
        *should = false;

        EnElfgrp* elfgrp = va_arg(args, EnElfgrp*);

        switch (elfgrp->type) {
            case ENELFGRP_TYPE_POWER:
                if (!RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].obtained) {
                    RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_WISDOM:
                if (!RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].obtained) {
                    RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_COURAGE:
                if (!RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].obtained) {
                    RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_KINDNESS:
                if (!RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].obtained) {
                    RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].eligible = true;
                }
                break;
            default: // ENELFGRP_TYPE_MAGIC
                if (!RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].obtained) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].eligible = true;
                }
                // In the game this uses an `else`, but in rando we are okay with both of these happening at the same
                // time
                if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU &&
                    !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].obtained) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].eligible = true;
                }
                break;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_GREAT_FAIRY, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS, IS_RANDO, { *should = false; });
}
