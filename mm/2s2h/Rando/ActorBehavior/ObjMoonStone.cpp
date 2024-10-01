#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_Obj_Moon_Stone/z_obj_moon_stone.h"
}

void ObjMoonstone_DrawCustom(Actor* thisx, PlayState* play) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_ASTRAL_OBSERVATORY_MOON_TEAR];

    Matrix_Translate(0.0f, 10.0f, 0.0f, MTXMODE_APPLY);

    thisx->shape.rot.y = thisx->shape.rot.y + 960;

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void Rando::ActorBehavior::InitObjMoonStoneBehavior() {
    static uint32_t shouldHook1Id = 0;
    static uint32_t onActorInit = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInit);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);

    onActorInit = 0;
    shouldHook1Id = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInit = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(
        ACTOR_OBJ_MOON_STONE, [](Actor* actor) {
            actor->world.pos.z += 5.0f;
            actor->draw = ObjMoonstone_DrawCustom;
        });

    shouldHook1Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_MOONS_TEAR, {
        ObjMoonStone* objMoonStone = va_arg(args, ObjMoonStone*);
        if (objMoonStone->actor.xzDistToPlayer < 25.0f) {
            *should = false;
            SET_WEEKEVENTREG(WEEKEVENTREG_74_40);
            Actor_Kill(&objMoonStone->actor);
        }
    });
}
