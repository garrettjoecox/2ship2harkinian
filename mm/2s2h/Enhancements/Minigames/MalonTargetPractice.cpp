#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Ma4/z_en_ma4.h"
}

#define CVAR_NAME "gEnhancements.Minigames.MalonTargetPractice"
#define CVAR CVarGetInteger(CVAR_NAME, 10)

void RegisterMalonTargetPractice() {
    COND_VB_SHOULD(VB_WIN_MALON_PRACTICE, CVAR != 10, {
        EnMa4* enMa4 = va_arg(args, EnMa4*);

        if (enMa4->poppedBalloonCounter >= CVAR) {
            enMa4->poppedBalloonCounter = 10;
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMalonTargetPractice, { CVAR_NAME });
