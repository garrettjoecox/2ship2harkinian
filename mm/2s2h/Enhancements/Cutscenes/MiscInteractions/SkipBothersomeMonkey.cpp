#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Mnk/z_en_mnk.h"
void EnMnk_Monkey_WaitToRun(EnMnk* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBothersomeMonkey() {
    COND_VB_SHOULD(VB_MONKEY_WAIT_TO_TALK_AFTER_APPROACH, CVAR, {
        EnMnk* monkey = (EnMnk*)va_arg(args, EnMnk*);
        *should = false;
        SET_EVENTINF(EVENTINF_25);
        monkey->actionFunc = EnMnk_Monkey_WaitToRun;
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBothersomeMonkey, { CVAR_NAME });
