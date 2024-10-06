#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

void RegisterSkipIntroSequence() {
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        // Intro cutscene
        if (!(gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) && gSaveContext.save.cutsceneIndex == 0))
            return;

        if (CVarGetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 0)) {
            gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
            gSaveContext.save.hasTatl = true;
            gSaveContext.cycleSceneFlags[SCENE_INSIDETOWER].switch0 |= (1 << 0);
            gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 2);
            gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 0);
            gSaveContext.save.playerForm = PLAYER_FORM_DEKU;

            // Mark chest as opened and give the player the Deku Nuts
            gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].chest |= (1 << 0);
            Item_Give(gPlayState, ITEM_DEKU_NUTS_10);
        }
    });
}
