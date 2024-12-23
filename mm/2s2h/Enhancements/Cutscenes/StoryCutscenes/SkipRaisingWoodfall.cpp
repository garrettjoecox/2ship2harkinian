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

void RegisterSkipRaisingWoodfall() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_21MITURINMAE) {
            if (*csId == 11) {
                *should = false;

                // Need to reload the scene after WEEKEVENTREG_12_20 gets set for the temple to be up.
                Player* player = GET_PLAYER(gPlayState);
                GameInteractor::Instance->events.emplace_back(GIEventRespawn{
                    .entrance = gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance,
                    .roomIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex,
                    .posX = player->actor.world.pos.x,
                    .posY = player->actor.world.pos.y,
                    .posZ = player->actor.world.pos.z,
                    .yaw = player->actor.shape.rot.y,
                });
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipRaisingWoodfall, { CVAR_NAME });
