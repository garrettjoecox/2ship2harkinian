#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Restorations.PowerCrouchStab"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterPowerCrouchStab() {
    COND_VB_SHOULD(VB_PATCH_POWER_CROUCH_STAB, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterPowerCrouchStab, { CVAR_NAME });
