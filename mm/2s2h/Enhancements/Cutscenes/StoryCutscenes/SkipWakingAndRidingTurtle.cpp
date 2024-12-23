#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipWakingAndRidingTurtle() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_31MISAKI) {
            // 12 is first time waking turtle, 20 is subsequent times
            if (*csId == 12 || *csId == 20) {
                *should = false;

                // Need to reload scene. Position is where Link ends up after CS
                GameInteractor::Instance->events.emplace_back(GIEventRespawn{
                    .entrance = gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance,
                    .roomIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex,
                    .posX = -5525.0f,
                    .posY = 14.0f,
                    .posZ = 1548.0f,
                    .yaw = -16384,
                });
            }
            // 13 is turtle leaving zora cape first time, 15 is subsequent times
            if (*csId == 13 || *csId == 15) {
                *should = false;
                GameInteractor::Instance->events.emplace_back(GIEventTransition{
                    .entrance = ENTRANCE(GREAT_BAY_TEMPLE, 0),
                    .cutsceneIndex = 0,
                    .transitionTrigger = TRANS_TRIGGER_START,
                    .transitionType = TRANS_TYPE_INSTANT,
                });
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipWakingAndRidingTurtle, { CVAR_NAME });
