#include "Logic.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNoLogicToSaveContext() {
    std::vector<RandoCheckId> checkPool;
    std::vector<RandoItemId> itemPool;
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            continue;
        }

        if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
            continue;
        }

        if (randoStaticCheck.randoCheckType == RCTYPE_OWL &&
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_OWL_STATUES] == RO_GENERIC_NO) {
            continue;
        }

        if ((randoStaticCheck.randoCheckType == RCTYPE_POT || randoStaticCheck.randoCheckType == RCTYPE_BARREL ||
             randoStaticCheck.randoCheckType == RCTYPE_CRATE ||
             randoStaticCheck.randoCheckType == RCTYPE_FREESTANDING) &&
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_MUNDANE] == RO_GENERIC_NO) {
            continue;
        }

        if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
            if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO &&
                randoCheckId != RC_CURIOSITY_SHOP_SPECIAL_ITEM &&
                randoCheckId != RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) {
                continue;
            } else {
                RANDO_SAVE_CHECKS[randoCheckId].price = Ship_Random(0, 200);
            }
        }

        if (randoStaticCheck.randoCheckType == RCTYPE_REMAINS &&
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_REMAINS] == RO_GENERIC_NO) {
            continue;
        }

        checkPool.push_back(randoCheckId);
        itemPool.push_back(randoStaticCheck.randoItemId);
    }

    // TODO: This should be decided by the player from the UI
    std::vector<RandoItemId> startingItems = {
        RI_SWORD_KOKIRI,
        RI_SHIELD_HERO,
        RI_OCARINA,
        RI_SONG_TIME,
    };

    // Grant the starting items
    for (RandoItemId startingItem : startingItems) {
        GiveItem(ConvertItem(startingItem));
    }

    // Add sword and shield to the pool if they aren't in the starting items
    if (std::find(startingItems.begin(), startingItems.end(), RI_SWORD_KOKIRI) == startingItems.end()) {
        itemPool.push_back(RI_SWORD_KOKIRI);
    }
    if (std::find(startingItems.begin(), startingItems.end(), RI_SHIELD_HERO) == startingItems.end()) {
        itemPool.push_back(RI_SHIELD_HERO);
    }

    // Add other items that don't have a vanilla location like Sun's Song or Song of Double Time

    // Remove starting items from the pool (but only one per entry in startingItems)
    for (RandoItemId startingItem : startingItems) {
        auto it = std::find(itemPool.begin(), itemPool.end(), startingItem);
        if (it != itemPool.end()) {
            itemPool.erase(it);
        }
    }

    // Add/Remove junk items to/from the pool to make the item pool size match the check pool size
    while (checkPool.size() != itemPool.size()) {
        if (checkPool.size() > itemPool.size()) {
            itemPool.push_back(RI_JUNK);
        } else {
            for (int i = 0; i < itemPool.size(); i++) {
                if (Rando::StaticData::Items[itemPool[i]].randoItemType == RITYPE_JUNK) {
                    itemPool.erase(itemPool.begin() + i);
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
    }

    for (size_t i = 0; i < checkPool.size(); i++) {
        RANDO_SAVE_CHECKS[checkPool[i]].shuffled = true;
        RANDO_SAVE_CHECKS[checkPool[i]].randoItemId = itemPool[i];
    }
}

} // namespace Logic

} // namespace Rando
