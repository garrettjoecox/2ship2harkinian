#include <libultraship/bridge.h>
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "functions.h"
#include "z64quake.h"
#include "macros.h"

#include "overlays/actors/ovl_En_Bji_01/z_en_bji_01.h"
}

static int frames = 0;

void RegisterSkipFallingMoonsTear() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_EN_BJI_01, [](Actor* actor) {
        EnBji01* bji = (EnBji01*)actor;
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0) &&
            bji->actor.playerHeightRel >= -350.0f && !CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
            // Normally set in the cutscene
            SET_WEEKEVENTREG(WEEKEVENTREG_17_10);
            SET_WEEKEVENTREG(WEEKEVENTREG_74_80);
            SET_WEEKEVENTREG(WEEKEVENTREG_74_20);
            SET_WEEKEVENTREG(WEEKEVENTREG_12_04);
            frames = 1;

            Actor_PlaySfx(&bji->actor, NA_SE_EV_MOONSTONE_FALL);
        }

        if (frames) {
            frames++;
            if (frames == 100) {
                frames = 0;
                Actor_PlaySfx(&bji->actor, NA_SE_EV_GORON_BOUND_1);
                s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(gPlayState), QUAKE_TYPE_3);
                Quake_SetSpeed(quakeIndex, 21536);
                Quake_SetPerturbations(quakeIndex, 4, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 12);
            }
        }
    });
}
