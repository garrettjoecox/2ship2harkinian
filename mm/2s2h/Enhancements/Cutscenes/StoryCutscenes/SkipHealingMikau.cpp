#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "functions.h"

void RegisterSkipHealingMikau() {
    REGISTER_VB_SHOULD(VB_START_CUTSCENE, {
        // Forced on in rando for now. The Zora Mask item get processing, as well as others, is handled by DmChar05.
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) || IS_RANDO) {
            s16* csId = va_arg(args, s16*);
            // Played Song of Healing for Mikau at Great Bay Coast
            if (gPlayState->sceneId == SCENE_30GYOSON && *csId == 13) {
                if (IS_RANDO) {
                    RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].eligible = true;
                } else {
                    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                        .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                        .param = GID_MASK_ZORA,
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You received the Zora Mask!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You received the Zora Mask!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                            },
                    });
                    // Immediately give the Zora Mask instead of queuing it, so that the gravestone spawn below works
                    Item_Give(gPlayState, ITEM_MASK_ZORA);
                }
                /*
                 * Mikau's gravestone exists in the Great Bay Coast scene but gets killed on init if the Mikau check has
                 * not already been processed. Normally, there are cutscene transitions after Link obtains the Zora Mask
                 * that allow the gravestone to spawn. We are skipping those, so we manually spawn the gravestone. These
                 * values are based on N64 US 1.0. This only matters for the current scene load; subsequent loads will
                 * just operate as normal.
                 */
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_SEKIHI, 838.0f, 80.0f, 3588.0, 0, -16749, 0, 4);
                Actor_Kill(va_arg(args, Actor*));
                *should = false;
            }
        }
    });
}
