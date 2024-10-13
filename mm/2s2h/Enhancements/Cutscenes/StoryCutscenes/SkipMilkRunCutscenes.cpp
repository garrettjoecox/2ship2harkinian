#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Rando/Rando.h"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
// extern s32 D_801BDAA0;
}

void RegisterSkipMilkRunCutscenes() {

    // Skip initial ride cutscene to Gorman Track, always on for Rando at the moment
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) || IS_RANDO) {
            Player* player = GET_PLAYER(gPlayState);
            if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 11)) {
                // This block crashes
                // if (player->stateFlags1 & PLAYER_STATE1_800000) {
                //     D_801BDAA0 = true;
                // }
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

    // Skip post milk ride cutscene in Rando, only if check hasn't happened yet
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (IS_RANDO) {
            if (!GameInteractor_Should(VB_HAVE_ROMANI_MASK, (INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI))) {
                if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(MILK_ROAD, 6)) {
                    gSaveContext.save.cutsceneIndex = 0;
                    gSaveContext.save.entrance = ENTRANCE(TERMINA_FIELD, 13);
                    gSaveContext.save.time += CLOCK_TIME(1, 0) + 2;
                }
            }
        }
    });
}
