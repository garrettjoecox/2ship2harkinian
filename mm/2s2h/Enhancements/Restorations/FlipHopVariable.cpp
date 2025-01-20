#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Restorations.ConstantFlipsHops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterVariableFlipHop() {
    COND_VB_SHOULD(VB_FLIP_HOP_VARIABLE, CVAR, {
        Player* player = GET_PLAYER(gPlayState);

        if (player->stateFlags2 & PLAYER_STATE2_80000) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterVariableFlipHop, { CVAR_NAME });
