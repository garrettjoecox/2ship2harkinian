#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.SkipEntranceCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipEntranceCutscenes() {
    COND_VB_SHOULD(VB_PLAY_ENTRANCE_CS, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterSkipEntranceCutscenes, { CVAR_NAME });
