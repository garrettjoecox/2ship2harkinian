#include "Logic.h"

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNoLogicToSaveContext(std::unordered_map<RandoCheckId, bool>& checkPool, std::vector<RandoItemId>& itemPool) {
    // Check if dungeon items should be restricted to their own dungeons
    bool restrictDungeonItems = RANDO_SAVE_OPTIONS[RO_RESTRICT_DUNGEON_ITEMS] == RO_GENERIC_ON;
    
    if (restrictDungeonItems) {
        // Separate dungeon items from general items
        std::vector<RandoItemId> generalItems;
        std::vector<std::vector<RandoItemId>> dungeonItems(4); // 4 dungeons
        
        for (auto it = itemPool.begin(); it != itemPool.end();) {
            if (IsDungeonItem(*it)) {
                DungeonIndex dungeonIndex = GetDungeonIndexFromItem(*it);
                dungeonItems[dungeonIndex].push_back(*it);
                it = itemPool.erase(it);
            } else {
                generalItems.push_back(*it);
                ++it;
            }
        }
        
        // Shuffle each dungeon's items separately
        for (auto& dungeonItemList : dungeonItems) {
            for (size_t i = 0; i < dungeonItemList.size(); i++) {
                std::swap(dungeonItemList[i], dungeonItemList[Ship_Random(0, dungeonItemList.size() - 1)]);
            }
        }
        
        // Shuffle general items
        for (size_t i = 0; i < generalItems.size(); i++) {
            std::swap(generalItems[i], generalItems[Ship_Random(0, generalItems.size() - 1)]);
        }
        
        // Separate checks by dungeon and general
        std::vector<RandoCheckId> generalChecks;
        std::vector<std::vector<RandoCheckId>> dungeonChecks(4); // 4 dungeons
        
        for (auto& [randoCheckId, _] : checkPool) {
            if (randoCheckId == RC_UNKNOWN) {
                continue;
            }
            
            if (IsCheckInDungeon(randoCheckId)) {
                DungeonIndex dungeonIndex = GetDungeonIndexFromCheck(randoCheckId);
                dungeonChecks[dungeonIndex].push_back(randoCheckId);
            } else {
                generalChecks.push_back(randoCheckId);
            }
        }
        
        // Place dungeon items in their respective dungeons
        for (size_t dungeonIndex = 0; dungeonIndex < 4; dungeonIndex++) {
            auto& checks = dungeonChecks[dungeonIndex];
            auto& items = dungeonItems[dungeonIndex];
            
            // Place dungeon items first in their own dungeon
            size_t itemIndex = 0;
            for (auto checkId : checks) {
                if (itemIndex < items.size()) {
                    RANDO_SAVE_CHECKS[checkId].shuffled = true;
                    RANDO_SAVE_CHECKS[checkId].randoItemId = items[itemIndex];
                    itemIndex++;
                } else {
                    // Fill remaining checks with general items
                    if (!generalItems.empty()) {
                        RANDO_SAVE_CHECKS[checkId].shuffled = true;
                        RANDO_SAVE_CHECKS[checkId].randoItemId = generalItems.back();
                        generalItems.pop_back();
                    }
                }
            }
        }
        
        // Place remaining general items in general checks
        for (auto checkId : generalChecks) {
            if (!generalItems.empty()) {
                RANDO_SAVE_CHECKS[checkId].shuffled = true;
                RANDO_SAVE_CHECKS[checkId].randoItemId = generalItems.back();
                generalItems.pop_back();
            }
        }
    } else {
        // Original logic - shuffle everything together
        for (size_t i = 0; i < itemPool.size(); i++) {
            std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
        }

        for (auto& [randoCheckId, _] : checkPool) {
            if (randoCheckId == RC_UNKNOWN) {
                continue;
            }

            RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
            itemPool.pop_back();
        }
    }
}

} // namespace Logic

} // namespace Rando
