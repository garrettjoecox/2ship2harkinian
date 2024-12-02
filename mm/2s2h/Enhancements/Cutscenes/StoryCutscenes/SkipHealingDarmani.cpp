#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "functions.h"

void RegisterSkipHealingDarmani() {
    REGISTER_VB_SHOULD(VB_START_CUTSCENE, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            s16* csId = va_arg(args, s16*);
            Actor* csActor = va_arg(args, Actor*);

            // Played Song of Healing for Darmani in Goron Graveyard
            if (gPlayState->sceneId == SCENE_GORON_HAKA && *csId == 9) {
                *should = false;

                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_DARMANI, true)) {
                    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                        .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                        .param = GID_MASK_GORON,
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You received the Goron Mask!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You received the Goron Mask!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                                Item_Give(gPlayState, ITEM_MASK_GORON);
                            },
                    });
                }
                Actor_Kill(csActor);
            }
        }
    });
}
