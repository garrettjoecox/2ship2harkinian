#include "Logic.h"
#include <libultraship/libultraship.h>
#include "2s2h/Rando/Types.h"

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

struct RandoPoolEntry {
    bool shuffled;
    RandoItemId vanillaItemId;
    RandoItemId placedItemId;
    bool itemPlaced;
    bool checkFilled;
    bool inPool;
};

void ApplyGlitchlessLogicToSaveContext() {
    std::unordered_map<RandoCheckId, RandoPoolEntry> randoCheckPool;
    std::set<RandoEvent*> randoEventsTriggered;
    SaveContext copiedSaveContext;
    memcpy(&copiedSaveContext, &gSaveContext, sizeof(SaveContext));

    // #region TODO: This just gives us a bunch of stuff that isn't technically in logic yet, so that generation can
    // happen prior to these items being in logic. Each time an item is logically placed, it should be removed from this
    // section.
    std::vector<RandoItemId> startingItems = {
        RI_SONG_EPONA,  RI_HOOKSHOT,        RI_ARROW_FIRE,    RI_BOW,
        RI_POWDER_KEG,  RI_MASK_GORON,      RI_DEED_MOUNTAIN, RI_MAGIC_BEAN,
        RI_SONG_STORMS, RI_SONG_BOSSA_NOVA, RI_MASK_GARO,     RI_SONG_ELEGY,
        RI_ARROW_LIGHT, RI_ARROW_ICE,       RI_LENS,          RI_BLUE_POTION_REFILL,
    };

    for (RandoItemId randoItemId : startingItems) {
        GiveItem(randoItemId);
    }
    // #endregion

    // First loop through all regions and add checks/items to the pool
    for (auto& [randoRegionId, randoRegion] : Rando::Logic::Regions) {
        for (auto& [randoCheckId, _] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

            if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
                RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_POT && RANDO_SAVE_OPTIONS[RO_SHUFFLE_POTS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO) {
                    continue;
                } else {
                    RANDO_SAVE_CHECKS[randoCheckId].price = Ship_Random(0, 200);
                }
            }

            randoCheckPool[randoCheckId] = { true, randoStaticCheck.randoItemId, RI_UNKNOWN, false, false, false };
        }
    }

    // Recursive lambda for backtracking placement
    std::function<bool(std::set<RandoRegionId>)> PlaceItems = [&](std::set<RandoRegionId> reachableRegions) -> bool {
        // Crawl through all reachable regions and add any new reachable regions
        std::set<RandoRegionId> currentReachableRegions = reachableRegions;
        for (RandoRegionId regionId : reachableRegions) {
            FindReachableRegions(regionId, currentReachableRegions);
        }

        // Track newly accessible checks
        std::vector<RandoCheckId> newChecksInPool;
        for (RandoRegionId regionId : currentReachableRegions) {
            auto& randoRegion = Rando::Logic::Regions[regionId];
            for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
                if (
                    // Check is shuffled
                    randoCheckPool[randoCheckId].shuffled &&
                    // Check is not already in the pool
                    randoCheckPool[randoCheckId].inPool == false) {
                    // Check is accessible
                    if (accessLogicFunc.first()) {
                        randoCheckPool[randoCheckId].inPool = true;
                        newChecksInPool.push_back(randoCheckId);
                    } else {
                        SPDLOG_INFO("Check {} is not accessible", Rando::StaticData::Checks[randoCheckId].name);
                    }
                }
            }
        }

        // Track newly triggered events
        std::vector<RandoEvent*> newEventsTriggered;
        for (RandoRegionId regionId : currentReachableRegions) {
            auto& randoRegion = Rando::Logic::Regions[regionId];
            for (auto& randoEvent : randoRegion.events) {
                if (
                    // Event is not already triggered
                    !randoEventsTriggered.contains(&randoEvent) &&
                    // Event condition is met
                    randoEvent.condition()) {
                    randoEventsTriggered.insert(&randoEvent);
                    newEventsTriggered.push_back(&randoEvent);
                    randoEvent.onApply();
                }
            }
        }

        int checksLeft = 0;
        std::vector<RandoCheckId> currentCheckPool;
        std::vector<std::pair<RandoItemId, RandoCheckId>> currentItemPool;
        for (auto& [randoCheckId, randoPoolEntry] : randoCheckPool) {
            if (randoPoolEntry.inPool) {
                if (!randoPoolEntry.itemPlaced) {
                    currentItemPool.push_back({ randoPoolEntry.vanillaItemId, randoCheckId });
                }
                if (!randoPoolEntry.checkFilled) {
                    currentCheckPool.push_back(randoCheckId);
                }
            }
            if (randoPoolEntry.shuffled && !randoPoolEntry.checkFilled) {
                checksLeft++;
            }
        }
        if (checksLeft == 0) {
            return true; // All items placed
        }

        if (currentItemPool.size() == 0) {
            for (RandoCheckId randoCheckId : newChecksInPool) {
                randoCheckPool[randoCheckId].inPool = false;
            }
            for (RandoEvent* randoEvent : newEventsTriggered) {
                randoEvent->onRemove();
                randoEventsTriggered.erase(randoEvent);
            }
            return false; // No more items to place
        }

        // Shuffle the pool
        if (currentItemPool.size() > 1) {
            for (size_t i = 0; i < currentItemPool.size(); i++) {
                std::swap(currentItemPool[i], currentItemPool[Ship_Random(0, currentItemPool.size() - 1)]);
            }
        }

        for (size_t i = 0; i < currentItemPool.size(); i++) {
            // Place the item in the check
            RandoCheckId randoCheckId = currentCheckPool[i];
            auto [randoItemId, randoCheckIdFromItem] = currentItemPool[i];
            SPDLOG_INFO("Placing item {} in check {}", Rando::StaticData::Items[randoItemId].spoilerName,
                        Rando::StaticData::Checks[randoCheckId].name);
            randoCheckPool[randoCheckId].checkFilled = true;
            randoCheckPool[randoCheckId].placedItemId = randoItemId;
            randoCheckPool[randoCheckIdFromItem].itemPlaced = true;
            RandoItemId convertedItemId = ConvertItem(randoItemId);
            GiveItem(convertedItemId);

            // Recurse to place the next item
            if (PlaceItems(currentReachableRegions)) {
                return true; // Found a solution
            }

            SPDLOG_INFO("Failed to place item {} in check {}", Rando::StaticData::Items[randoItemId].spoilerName,
                        Rando::StaticData::Checks[randoCheckId].name);
            // Backtrack: remove the item and try another check
            randoCheckPool[randoCheckId].checkFilled = false;
            randoCheckPool[randoCheckId].placedItemId = RI_UNKNOWN;
            randoCheckPool[randoCheckIdFromItem].itemPlaced = false;
            RemoveItem(convertedItemId);
        }

        for (RandoCheckId randoCheckId : newChecksInPool) {
            randoCheckPool[randoCheckId].inPool = false;
        }
        for (RandoEvent* randoEvent : newEventsTriggered) {
            randoEvent->onRemove();
            randoEventsTriggered.erase(randoEvent);
        }

        return false; // No valid placements for this item
    };

    if (!PlaceItems({ RR_MAX })) {
        memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));
        throw std::runtime_error("Failed to place all items in glitchless logic");
    } else {
        memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));

        for (auto& [randoCheckId, randoPoolEntry] : randoCheckPool) {
            if (randoPoolEntry.shuffled) {
                RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoPoolEntry.placedItemId;
                RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
            }
        }

        SPDLOG_INFO("Successfully placed all items in glitchless logic");
    }
}

} // namespace Logic

} // namespace Rando
