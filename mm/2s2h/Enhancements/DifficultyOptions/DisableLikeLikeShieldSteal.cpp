#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.DisableLikeLikeShieldSteal"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDisableLikeLikeShieldSteal() {
    COND_VB_SHOULD(VB_LIKE_LIKE_STEAL_SHIELD, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableLikeLikeShieldSteal, { CVAR_NAME });
