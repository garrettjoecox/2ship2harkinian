#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "overlays/actors/ovl_En_Yb/z_en_yb.h"
}

void RegisterSkipKamaroTeachDance() {
    /*
     * The game strangely forces you to watch Kamaro dance for 10 seconds before continuing his dialog, so we just set
     * that timer to 0.
     */
    REGISTER_VB_SHOULD(VB_START_CUTSCENE, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
            // Cutscene 41 in Termina Field is the Kamaro dance scene.
            if (gPlayState->sceneId == SCENE_00KEIKOKU) {
                s16* csId = va_arg(args, s16*);
                if (*csId == 41) {
                    EnYb* enYb = va_arg(args, EnYb*);
                    enYb->teachingCutsceneTimer = 0;
                }
            }
        }
    });
}
