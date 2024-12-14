#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gModes.TimeMovesWhenYouMove"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterTimeMovesWhenYouMove() {
    gSaveContext.save.timeSpeedOffset = 0;

    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        Player* player = GET_PLAYER(gPlayState);

        gSaveContext.save.timeSpeedOffset = -R_TIME_SPEED;
        if (player->linearVelocity != 0) {
            gSaveContext.save.timeSpeedOffset = CLAMP(player->linearVelocity / 2 - R_TIME_SPEED, -R_TIME_SPEED, 10);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTimeMovesWhenYouMove, { CVAR_NAME });
