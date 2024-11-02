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
    REGISTER_VB_SHOULD(VB_PLAY_KAMARO_TEACH_DANCE, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
            EnYb* enYb = va_arg(args, EnYb*);
            enYb->teachingCutsceneTimer = 0;
            *should = false;
        }
    });
}
