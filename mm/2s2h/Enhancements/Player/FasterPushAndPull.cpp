#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Dblue_Movebg/z_bg_dblue_movebg.h"
}

#define CVAR_NAME "gEnhancements.Player.FasterPushAndPull"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFasterPushAndPull() {
    COND_VB_SHOULD(VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED, CVAR, {
        BgDblueMovebg* bgDblueMovebg = va_arg(args, BgDblueMovebg*);
        *should = false;
        bgDblueMovebg->unk_188 = 20;
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterPushAndPull, { CVAR_NAME });
