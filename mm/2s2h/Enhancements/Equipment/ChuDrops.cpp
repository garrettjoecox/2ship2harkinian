#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "objects/gameplay_keep/gameplay_keep.h"
}

#define CVAR_NAME "gEnhancements.Equipment.ChuDrops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void ChuDrop_Give(Actor* actor, PlayState* play) {
    Item_Give(play, ITEM_BOMBCHUS_5);
}

static void ChuDrop_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    if (CVarGetInteger("gEnhancements.Graphics.3DItemDrops", 0)) {
        Matrix_Scale(16.0f, 16.0f, 16.0f, MTXMODE_APPLY);
        GetItem_Draw(play, GID_BOMBCHU);
    } else {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

        POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gDropBombchuTex);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);

        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gItemDropDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void RegisterChuDrops() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, CVAR, [](Actor* actor, bool* should) {
        if (actor->params == ITEM00_BOMBS_0 || actor->params == ITEM00_BOMBS_A || actor->params == ITEM00_BOMBS_B) {
            if (rand() % 100 < 50) {
                *should = false;
                CustomItem::Spawn(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
                                  CustomItem::GIVE_OVERHEAD | CustomItem::TOSS_ON_SPAWN, GID_BOMBCHU, ChuDrop_Give,
                                  ChuDrop_Draw);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterChuDrops, { CVAR_NAME });
