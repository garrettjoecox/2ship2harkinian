#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Lift_Nuts/z_en_lift_nuts.h"
#include "overlays/actors/ovl_Obj_Lupygamelift/z_obj_lupygamelift.h"
#include "overlays/actors/ovl_En_Gamelupy/z_en_gamelupy.h"
}

#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

RandoCheckId IdentifyGameLupy(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;

    if (IS_AT(-100.0f, 150.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_1;
    } else if (IS_AT(100.0f, -50.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_2;
    } else if (IS_AT(-200.0f, -250.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_3;
    } else if (IS_AT(200.0f, 350.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_4;
    } else if (IS_AT(-500.0f, 350.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_5;
    } else if (IS_AT(500.0f, -250.0f)) {
        randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_6;
    }

    return randoCheckId;
}

void Gamelupy_RandoDrawFunc(Actor* actor, PlayState* play) {
    EnGamelupy* enGamelupy = (EnGamelupy*)actor;

    auto randoSaveCheck = RANDO_SAVE_CHECKS[IdentifyGameLupy(actor)];

    float x = actor->home.pos.x;
    float z = actor->home.pos.z;

    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void Rando::ActorBehavior::InitObjLupyGameLiftBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GAMELUPY, IS_RANDO, [](Actor* actor) {
        EnGamelupy* enGamelupy = (EnGamelupy*)actor;

        if (gPlayState->sceneId != SCENE_DEKUTES) {
            return;
        }

        enGamelupy->actor.draw = Gamelupy_RandoDrawFunc;
    });
}
