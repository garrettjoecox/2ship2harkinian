#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

static bool queued = false;

// This function handles queuing up item gives that the player has been marked as eligible for. If you are looking for
// the behavior of the actual giving itself, the heavy lifting is done by the GameInteractor queue. This function is
// currently called every frame, and loops through the entire list of checks, this works for now but as the check list
// grows we should keep an eye on performance.
//
// Though it seems counter-intuitive, we currently only allow one thing from randommizer to be queued at a time,
// primarily because of the way an item can be converted may change as you queue up multiple items. This is actually
// fine for both the giving/drawing, as we can call ConvertItem() inside the Give/Draw lambda, but the message we
// pass to the queue is static and would need to be updated if we allowed multiple items to be queued at once.
void Rando::MiscBehavior::CheckQueue() {
    if (queued) {
        return;
    }

    for (auto& randoSaveCheck : RANDO_SAVE_CHECKS) {
        if (randoSaveCheck.eligible && !randoSaveCheck.obtained) {
            queued = true;

            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .getItemText = Rando::StaticData::Items[randoItemId].name,
                .drawItem = [randoItemId]() { Rando::DrawItem(randoItemId); },
                .giveItem =
                    [&randoSaveCheck, randoItemId]() {
                        Rando::GiveItem(randoItemId);
                        randoSaveCheck.obtained = true;
                        queued = false;
                    } });
            return;
        }
    }
}

void Rando::MiscBehavior::CheckQueueReset() {
    queued = false;
    GameInteractor::Instance->currentEvent = GIEventNone{};
    GameInteractor::Instance->events.clear();
}
