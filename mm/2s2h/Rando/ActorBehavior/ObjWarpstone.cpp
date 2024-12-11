#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/Rando/ActorBehavior/ObjWarpstone.h"

extern "C" {
#include "variables.h"
}

void ObjWarpstone_ActivateStatue(u8 refId) {
    gSaveContext.save.saveInfo.playerData.owlActivationFlags =
        ((void)0, gSaveContext.save.saveInfo.playerData.owlActivationFlags) | (u16)gBitFlags[refId];
}

RandoCheckId Identify_Statue(u8 refId) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    switch (refId) {
        case 0:
            randoCheckId = RC_GREAT_BAY_COAST_OWL_STATUE;
            break;
        case 1:
            randoCheckId = RC_ZORA_CAPE_OWL_STATUE;
            break;
        case 2:
            randoCheckId = RC_SNOWHEAD_OWL_STATUE;
            break;
        case 3:
            randoCheckId = RC_MOUNTAIN_VILLAGE_OWL_STATUE;
            break;
        case 4:
            randoCheckId = RC_CLOCK_TOWN_SOUTH_OWL_STATUE;
            break;
        case 5:
            randoCheckId = RC_MILK_ROAD_OWL_STATUE;
            break;
        case 6:
            randoCheckId = RC_WOODFALL_OWL_STATUE;
            break;
        case 7:
            randoCheckId = RC_SOUTHERN_SWAMP_OWL_STATUE;
            break;
        case 8:
            randoCheckId = RC_IKANA_CANYON_OWL_STATUE;
            break;
        case 9:
            randoCheckId = RC_STONE_TOWER_OWL_STATUE;
            break;
        default:
            break;
    }
    return randoCheckId;
}

void Rando::ActorBehavior::InitObjWarpstoneBehavior() {
    COND_VB_SHOULD(VB_OWL_STATUE_ACTIVATE, IS_RANDO, {
        u8 warpstoneId = va_arg(args, u8);
        if (!RANDO_SAVE_CHECKS[Identify_Statue(warpstoneId)].obtained) {
            RANDO_SAVE_CHECKS[Identify_Statue(warpstoneId)].eligible = true;
        }
        *should = false;
    });
}