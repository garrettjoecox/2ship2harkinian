#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gCheats.UnbreakableRazorSword"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterUnbreakableRazorSword() {
    COND_VB_SHOULD(VB_LOWER_RAZOR_SWORD_DURABILITY, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterUnbreakableRazorSword, { CVAR_NAME });
