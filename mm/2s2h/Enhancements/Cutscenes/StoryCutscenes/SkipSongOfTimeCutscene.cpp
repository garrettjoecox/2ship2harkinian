#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipSongOfTimeCutscene() {
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.cutsceneIndex == 0xFFF7 && (gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) ||
                                                          gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 1))) {
            *should = false;
            // Override the destination
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
            // Normally happens at the end of EnTest6_StartCutscene
            gSaveContext.save.eventDayCount = 0;
            gSaveContext.save.day = 0;
            gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipSongOfTimeCutscene, { CVAR_NAME });
