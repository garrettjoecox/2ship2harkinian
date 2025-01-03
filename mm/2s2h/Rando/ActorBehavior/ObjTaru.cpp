#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Taru/z_obj_taru.h"
}

std::map<std::pair<float, float>, RandoCheckId> barrelMap = {
    // Great Bay Temple //
    { { -540.0f, 3450.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_01 },
    { { -480.0f, 3450.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_02 },
    { { -540.0f, 3510.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_03 },
    { { -480.0f, 3510.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_04 },
    { { 480.0f, 3510.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_05 },
    { { 540.0f, 3510.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_06 },
    { { 480.0f, 3450.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_07 },
    { { 540.0f, 3450.0f }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_08 },
    { { -360.0f, -1455.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_01 },
    { { -240.0f, -1455.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_02 },
    { { -300.0f, -1590.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_03 },
    { { 2685.0f, -1200.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_BARREL_01 },
    { { 2685.0f, -1260.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_BARREL_02 },
    { { 1785.0f, 2190.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_01 },
    { { 1785.0f, 2070.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_02 },
    { { 1785.0f, 2010.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_03 },
    { { 1305.0f, 2058.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_04 },
    { { 1305.0f, 1998.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_05 },

    // Pirates Fortress //
    { { 3880.0f, -850.0f }, RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_01 },
    { { 3880.0f, -910.0f }, RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_02 },
    { { -80.0f, 1320.0f }, RC_PIRATE_FORTRESS_ENTRANCE_BARREL },
    { { 3740.0f, -1860.0f }, RC_PIRATE_FORTRESS_INTERIOR_GUARDED_BARREL },
    { { 2520.0f, 440.0f }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_01 },
    { { 2580.0f, 400.0f }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_02 },
    { { 2520.0f, 381.0f }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_03 },
    { { 2580.0f, 341.0f }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_04 },
    { { 2520.0f, 321.0f }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_05 },
    { { 1880.0f, -2020.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_01 },
    { { 1880.0f, -2080.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_02 },
    { { 1880.0f, -2140.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_03 },
    { { 2171.0f, -1503.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_04 },
    { { 2171.0f, -1443.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_05 },
    { { 2171.0f, -1383.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_06 },
    { { 2070.0f, -2250.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_07 },
    { { 2070.0f, -2310.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_08 },
    { { 2070.0f, -2370.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_09 },
    { { 2070.0f, -2430.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_10 },
    { { 1570.0f, -2310.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_11 },
    { { 1510.0f, -2310.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_12 },
    { { 1570.0f, -2370.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_13 },
    { { 1570.0f, -2430.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_14 },
    { { 1510.0f, -2370.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_15 },
    { { 1510.0f, -2430.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_16 },
};

void Rando::ActorBehavior::InitObjTaruBehavior() {
    COND_VB_SHOULD(VB_BARREL_OR_CRATE_DROP_COLLECTIBLE, IS_RANDO, {
        ObjTaru* refActor = va_arg(args, ObjTaru*);
        auto randoStaticCheck = Rando::StaticData::Checks[RC_UNKNOWN];

        auto it = barrelMap.find({ refActor->dyna.actor.home.pos.x, refActor->dyna.actor.home.pos.z });
        if (it == barrelMap.end()) {
            return;
        } else {
            randoStaticCheck = Rando::StaticData::Checks[it->second];
        }

        if (!RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled ||
            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].obtained) {
            return;
        }

        *should = false;

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        EnItem00* spawn = CustomItem::Spawn(
            refActor->dyna.actor.world.pos.x, refActor->dyna.actor.world.pos.y, refActor->dyna.actor.world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, randoStaticCheck.randoCheckId,
            [](Actor* actor, PlayState* play) {
                auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
                switch (randoStaticCheck.flagType) {
                    case FLAG_NONE:
                        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
                            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].eligible = true;
                        }
                        break;
                    case FLAG_CYCL_SCENE_COLLECTIBLE:
                        Flags_SetCollectible(play, randoStaticCheck.flag);
                        break;
                    default:
                        break;
                }
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM));
            });
    });
}
