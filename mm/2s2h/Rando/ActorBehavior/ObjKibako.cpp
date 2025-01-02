#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
}

std::map<std::pair<float, float>, RandoCheckId> crateMap = {
    { { 1725.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_01 },
    { { 1725.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_02 },
    { { 1755.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_03 },
    { { 1755.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_04 },
    { { 960.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_05 },
    { { 930.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_06 },
    { { 960.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_07 },
    { { 930.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_08 },
    { { 960.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_09 },
    { { 960.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_10 },
    { { 930.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_11 },
    { { 930.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_12 },
    { { 1725.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_13 },
    { { 1755.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_14 },
    { { 1725.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_15 },
    { { 1755.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_16 },
    { { 1845.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_01 },
    { { 1845.0f, 1992.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_02 },
    { { 1845.0f, 2040.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_03 },
    { { 1305.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_04 },
    { { 1245.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_05 },
};

void Rando::ActorBehavior::InitObjKibakoBehavior() {
    COND_VB_SHOULD(VB_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        auto randoStaticCheck = Rando::StaticData::Checks[RC_UNKNOWN];

        if (actor->id != ACTOR_OBJ_KIBAKO && actor->id != ACTOR_OBJ_KIBAKO2) {
            return;
        }

        auto it = crateMap.find({ actor->home.pos.x, actor->home.pos.z });
        if (it == crateMap.end()) {
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
            actor->home.pos.x, actor->home.pos.y, actor->home.pos.z, 0,
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

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO, IS_RANDO, [](Actor* actor) {
        ObjKibako* refActor = (ObjKibako*)actor;

        SPDLOG_INFO("Small Crate In Area");
    });

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO2, IS_RANDO, [](Actor* actor) {
        ObjKibako2* refActor = (ObjKibako2*)actor;

        SPDLOG_INFO("Large Crate In Area");
    });

    COND_ID_HOOK(OnActorKill, ACTOR_OBJ_KIBAKO, IS_RANDO, [](Actor* actor) {
        ObjKibako* refActor = (ObjKibako*)actor;

        SPDLOG_INFO("Small Crate Coords: {}f, {}f", std::to_string(refActor->actor.home.pos.x),
                    std::to_string(refActor->actor.home.pos.z));
    });

    COND_ID_HOOK(OnActorKill, ACTOR_OBJ_KIBAKO2, IS_RANDO, [](Actor* actor) {
        ObjKibako2* refActor = (ObjKibako2*)actor;

        SPDLOG_INFO("Large Crate Coords: {}f, {}f", std::to_string(refActor->dyna.actor.home.pos.x),
                    std::to_string(refActor->dyna.actor.home.pos.z));
    });
}