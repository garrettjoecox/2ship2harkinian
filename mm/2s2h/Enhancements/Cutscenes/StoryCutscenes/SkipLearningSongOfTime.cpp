#include <libultraship/bridge.h>
#include <spdlog/spdlog.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "assets/objects/object_gi_melody/object_gi_melody.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

void RegisterSkipLearningSongOfTime() {
    REGISTER_VB_SHOULD(VB_PLAY_SONG_OF_TIME_CS, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) && *should) {
            *should = false;
            // This typically gets set in the cutscene
            gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
            GameInteractor::Instance->events.emplace_back(
                GIEventGiveItem{ .showGetItemCutscene = true,
                                 .param = GID_MASK_DEKU,
                                 .giveItem =
                                     [](Actor* actor, PlayState* play) {
                                         if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                             CustomMessage::SetActiveCustomMessage("You received the Song of Time!",
                                                                                   { .textboxType = 2 });
                                         } else {
                                             CustomMessage::StartTextbox("You received the Song of Time!\x1C\x02\x10",
                                                                         { .textboxType = 2 });
                                         }
                                         Item_Give(gPlayState, ITEM_SONG_TIME);
                                     },
                                 .drawItem =
                                     [](Actor* actor, PlayState* play) {
                                         Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                                         Rando::DrawItem(RI_SONG_OF_TIME);
                                     } });
        }
    });
}
