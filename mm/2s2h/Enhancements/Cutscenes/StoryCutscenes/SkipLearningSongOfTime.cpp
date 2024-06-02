#include <libultraship/bridge.h>
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
extern SaveContext gSaveContext;
}

void RegisterSkipLearningSongOfTime() {
    REGISTER_VB_SHOULD(GI_VB_PLAY_TRANSITION_CS, {
        // Cutscene that plays after picking up the Ocarina of Time.
            GameInteractor_ItemGiveQueue_Push(GI_B3, false);
        if (gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) && gSaveContext.save.cutsceneIndex == 0xFFF2 &&
            CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            gSaveContext.save.entrance = ENTRANCE(CLOCK_TOWER_ROOFTOP, 1);
            gSaveContext.save.cutsceneIndex = 0;
            GameInteractor_ItemGiveQueue_Push(GI_B3, true);
        }
    });
}
