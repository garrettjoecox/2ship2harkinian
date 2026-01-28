#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Logic/EntranceShuffle.h"
#include "2s2h/Rando/EntranceTracker/EntranceTracker.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "z64scene.h"
}

namespace Rando {

namespace EntranceShuffle {

// Hook into scene transitions to apply entrance shuffling
void OnPlayDestroy() {
    if (!IsEntranceShuffleEnabled()) {
        return;
    }

    // Get the shuffled destination entrance
    s32 originalEntrance = gSaveContext.save.entrance;
    s32 shuffledEntrance = GetShuffledEntrance(originalEntrance);

    // Record discovered entrance for the tracker
    EntranceTracker::SetEntranceDiscovered(shuffledEntrance);

    if (shuffledEntrance != originalEntrance) {
        gSaveContext.save.entrance = shuffledEntrance;
    }
}

static RegisterShipInitFunc registerHooks(
    []() {
        // Hook into OnPlayDestroy which is called just before transitioning
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>([]() { OnPlayDestroy(); });
    },
    {});

} // namespace EntranceShuffle

} // namespace Rando
