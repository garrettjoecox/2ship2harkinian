#include <libultraship/bridge.h>
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "variables.h"
}

void RegisterSkipSongOfTimeCutscene() {
    REGISTER_VB_SHOULD(GI_VB_PLAY_TRANSITION_CS, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) &&
            gSaveContext.save.cutsceneIndex == 0xFFF7 &&
            (gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) ||
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
