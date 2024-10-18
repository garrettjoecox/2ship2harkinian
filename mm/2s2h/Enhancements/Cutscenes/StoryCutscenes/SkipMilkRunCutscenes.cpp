#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Rando/Rando.h"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

void RegisterSkipMilkRunCutscenes() {

    // Skip initial ride cutscene to Gorman Track
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 11)) {
                gSaveContext.save.cutsceneIndex = 0;
                gSaveContext.save.entrance = ENTRANCE(GORMAN_TRACK, 4);
                // Add time that occurs during skipped cutscenes
                if (gSaveContext.save.timeSpeedOffset == -2) {
                    gSaveContext.save.time += 0x50a; // ~30 min
                } else {
                    gSaveContext.save.time += 0xf1e; // ~1 hour 24 min
                }
            }
        }
    });
}
