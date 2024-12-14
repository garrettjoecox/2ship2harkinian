#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Restorations.ConstantFlipsHops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterVariableFlipHop() {
    COND_VB_SHOULD(VB_FLIP_HOP_VARIABLE, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterVariableFlipHop, { CVAR_NAME });
