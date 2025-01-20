#include "Logic.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNearlyNoLogicToSaveContext() {
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

    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
    }

    std::unordered_map<RandoItemId, RandoCheckId> importantItems;
    std::vector<RandoCheckId> safeChecks;

    std::unordered_map<RandoItemId, std::vector<SceneId>> itemToSceneBlacklist = {
        { RI_MASK_DEKU,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_SONATA,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_MASK_ZORA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_SONG_NOVA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_REMAINS_ODOLWA,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GOHT,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GYORG,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_TWINMOLD,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_OATH,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
    };

    for (size_t i = 0; i < checkPool.size(); i++) {
        RANDO_SAVE_CHECKS[checkPool[i]].shuffled = true;
        RANDO_SAVE_CHECKS[checkPool[i]].randoItemId = itemPool[i];

        auto randoStaticCheck = Rando::StaticData::Checks[checkPool[i]];

        if (itemToSceneBlacklist.find(itemPool[i]) != itemToSceneBlacklist.end()) {
            importantItems[itemPool[i]] = checkPool[i];
        } else if (randoStaticCheck.sceneId != SCENE_MITURIN &&    // Woodfall Temple
                   randoStaticCheck.sceneId != SCENE_MITURIN_BS && // Woodfall Temple Boss
                   randoStaticCheck.sceneId != SCENE_SEA &&        // Great Bay Temple
                   randoStaticCheck.sceneId != SCENE_SEA_BS &&     // Great Bay Temple Boss
                   randoStaticCheck.sceneId != SCENE_LAST_DEKU &&  // Moon Deku
                   randoStaticCheck.sceneId != SCENE_LAST_GORON && // Moon Goron
                   randoStaticCheck.sceneId != SCENE_LAST_ZORA &&  // Moon Goron
                   randoStaticCheck.sceneId != SCENE_LAST_LINK &&  // Moon Human
                   randoStaticCheck.sceneId != SCENE_SOUGEN &&     // Moon
                   randoStaticCheck.sceneId != SCENE_LAST_BS       // Moon Boss
        ) {
            safeChecks.push_back(checkPool[i]);
        }
    }

    for (auto& [randoItemId, blacklistedScenes] : itemToSceneBlacklist) {
        if (importantItems.find(randoItemId) != importantItems.end()) {
            auto randoStaticCheck = Rando::StaticData::Checks[importantItems.at(randoItemId)];
            const auto& blacklist = itemToSceneBlacklist.at(randoItemId);

            if (std::find(blacklist.begin(), blacklist.end(), randoStaticCheck.sceneId) != blacklist.end()) {
                auto otherCheckIndex = Ship_Random(0, safeChecks.size() - 1);
                RANDO_SAVE_CHECKS[importantItems[randoItemId]].randoItemId =
                    RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId;
                RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId = randoItemId;
                safeChecks.erase(safeChecks.begin() + otherCheckIndex);
            }
        }
    }
}

} // namespace Logic

} // namespace Rando
