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

        if ((randoStaticCheck.randoCheckType == RCTYPE_POT || randoStaticCheck.randoCheckType == RCTYPE_RUPEE ||
             randoStaticCheck.randoCheckType == RCTYPE_BARREL || randoStaticCheck.randoCheckType == RCTYPE_CRATE) &&
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_MUNDANE] == RO_GENERIC_NO) {
            continue;
        }

        if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
            if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO &&
                randoCheckId != RC_CURIOSITY_SHOP_SPECIAL_ITEM &&
                randoCheckId != RC_BOMB_SHOP_ITEM_OR_CURIOSITY_SHOP_ITEM) {
                continue;
            } else {
                RANDO_SAVE_CHECKS[randoCheckId].price = Ship_Random(0, 200);
            }
        }

        checkPool.push_back(randoCheckId);
        itemPool.push_back(randoStaticCheck.randoItemId);
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
