#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
}

#define IS_AT(xx, zz) (actor->world.pos.x == xx, actor->world.pos.z = zz)

RandoCheckId IdentifyPot(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    switch (gPlayState->sceneId) {
        case SCENE_REDEAD:
            if (IS_AT(1185.0f, -915.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_1;
                break;
            }
            if (IS_AT(1185.0f, -945.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_2;
                break;
            }
            if (IS_AT(1185.0f, -975.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_3;
                break;
            }
            if (IS_AT(1185.0f, -1005.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_4;
                break;
            }
            if (IS_AT(1185.0f, -1035.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_5;
                break;
            }
            if (IS_AT(1185.0f, -1065.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_6;
                break;
            }
            if (IS_AT(1185.0f, -1095.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_7;
                break;
            }
            if (IS_AT(1185.0f, -1125.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_8;
                break;
            }
            if (IS_AT(1185.0f, -1155.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_9;
                break;
            }
            if (IS_AT(1185.0f, -1185.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_10;
                break;
            }
            break;
        default:
            break;
    }
    return randoCheckId;
}

void Rando::ActorBehavior::InitObjTsuboBehavior() {
    static uint32_t shouldHookId1 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);

    shouldHookId1 = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHookId1 = REGISTER_VB_SHOULD(VB_GIVE_ITEM_FROM_POT, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = IdentifyPot(actor);
        if (randoCheckId == RC_UNKNOWN) {
            *should = true;
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (randoSaveCheck.eligible) {
            *should = true;
            return;
        }
        randoSaveCheck.eligible = true;
        *should = false;
    });
}
