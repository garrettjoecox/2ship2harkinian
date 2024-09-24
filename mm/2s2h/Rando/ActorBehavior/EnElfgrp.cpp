#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Elfgrp/z_en_elfgrp.h"
}

void Rando::ActorBehavior::InitEnElfgrpBehavior(bool isRando) {
    static uint32_t shouldHook1Id = 0;
    static uint32_t shouldHook2Id = 0;
    static uint32_t shouldHook3Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook2Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook3Id);

    shouldHook1Id = 0;
    shouldHook2Id = 0;
    shouldHook3Id = 0;

    if (!isRando) {
        return;
    }

    shouldHook1Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, {
        *should = false;

        EnElfgrp* elfgrp = static_cast<EnElfgrp*>(opt);

        switch (elfgrp->type) {
            case ENELFGRP_TYPE_POWER:
                if (!RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].eligible) {
                    RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_WISDOM:
                if (!RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].eligible) {
                    RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_COURAGE:
                if (!RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].eligible) {
                    RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_KINDNESS:
                if (!RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].eligible) {
                    RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].eligible = true;
                }
                break;
            default: // ENELFGRP_TYPE_MAGIC
                if (!RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].eligible) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].eligible = true;
                }
                // In the game this uses an `else`, but in rando we are okay with both of these happening at the same
                // time
                if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU &&
                    !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].eligible) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].eligible = true;
                }
                break;
        }
    });

    shouldHook2Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_GREAT_FAIRY, { *should = false; });
    shouldHook3Id = REGISTER_VB_SHOULD(GI_VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS, { *should = false; });
}
