#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Gk/z_en_gk.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// This is a song tutorial, so the skip is forced on in rando for now
void RegisterSkipLearningGoronLullaby() {
    // Played Lullaby Intro for Baby Goron
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId != 9 || actor == NULL || actor->id != ACTOR_EN_GK) {
            return;
        }

        EnGk* enGk = (EnGk*)actor;

        // Reset cutscene flags state
        enGk->unk_1E4 &= ~0x20;
        *should = false;

        if (IS_RANDO) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_80); // Ensure Goron Elder check is available
            RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].eligible = true;
        } else {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You learned the complete Goron Lullaby!",
                                                                  { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You learned the complete Goron Lullaby!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_SONG_LULLABY);
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_SONG_LULLABY);
                    } });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningGoronLullaby, { CVAR_NAME, "IS_RANDO" });
