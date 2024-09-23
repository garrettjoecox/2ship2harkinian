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
                if (!Flags_GetRandoInf(RANDO_INF_WOODFALL_GREAT_FAIRY)) {
                    Flags_SetRandoInf(RANDO_INF_WOODFALL_GREAT_FAIRY);
                }
                break;
            case ENELFGRP_TYPE_WISDOM:
                if (!Flags_GetRandoInf(RANDO_INF_SNOWHEAD_GREAT_FAIRY)) {
                    Flags_SetRandoInf(RANDO_INF_SNOWHEAD_GREAT_FAIRY);
                }
                break;
            case ENELFGRP_TYPE_COURAGE:
                if (!Flags_GetRandoInf(RANDO_INF_GREAT_BAY_GREAT_FAIRY)) {
                    Flags_SetRandoInf(RANDO_INF_GREAT_BAY_GREAT_FAIRY);
                }
                break;
            case ENELFGRP_TYPE_KINDNESS:
                if (!Flags_GetRandoInf(RANDO_INF_IKANA_GREAT_FAIRY)) {
                    Flags_SetRandoInf(RANDO_INF_IKANA_GREAT_FAIRY);
                }
                break;
            default: // ENELFGRP_TYPE_MAGIC
                if (!Flags_GetRandoInf(RANDO_INF_CLOCK_TOWN_GREAT_FAIRY)) {
                    Flags_SetRandoInf(RANDO_INF_CLOCK_TOWN_GREAT_FAIRY);
                }
                // In the game this uses an `else`, but in rando we are okay with both of these happening at the same
                // time
                if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU &&
                    !Flags_GetRandoInf(RANDO_INF_CLOCK_TOWN_GREAT_FAIRY_ALT)) {
                    Flags_SetRandoInf(RANDO_INF_CLOCK_TOWN_GREAT_FAIRY_ALT);
                }
                break;
        }
    });

    shouldHook2Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_GREAT_FAIRY, { *should = false; });
    shouldHook3Id = REGISTER_VB_SHOULD(GI_VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS, { *should = false; });
}
