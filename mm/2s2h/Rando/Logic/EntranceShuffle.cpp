#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "EntranceShuffle.h"
#include "Logic.h"

extern "C" {
#include "z64scene.h"
}

namespace Rando {

namespace EntranceShuffle {

// Entrance mapping storage
std::map<s32, s32> sEntranceMap;

// Optimally, these lists are dynamically built from the maps we already have built for logic...

std::set<s32> interiorEntrances = {
    // Clock Town
    ENTRANCE(ASTRAL_OBSERVATORY, 0),
    ENTRANCE(TREASURE_CHEST_SHOP, 0),
    ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),
    ENTRANCE(MAYORS_RESIDENCE, 0),
    ENTRANCE(TOWN_SHOOTING_GALLERY, 0),
    ENTRANCE(STOCK_POT_INN, 0),
    ENTRANCE(STOCK_POT_INN, 1),
    ENTRANCE(MILK_BAR, 0),
    ENTRANCE(CURIOSITY_SHOP, 1),
    ENTRANCE(FAIRY_FOUNTAIN, 0),
    ENTRANCE(CLOCK_TOWER_INTERIOR, 1),
    ENTRANCE(SWORDMANS_SCHOOL, 0),
    ENTRANCE(CURIOSITY_SHOP, 0),
    ENTRANCE(TRADING_POST, 0),
    ENTRANCE(BOMB_SHOP, 0),
    ENTRANCE(POST_OFFICE, 0),
    ENTRANCE(LOTTERY_SHOP, 0),
    // Termina Field & Roads
    ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),
    ENTRANCE(TOURIST_INFORMATION, 0),
    ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),
    // Milk Road
    ENTRANCE(CUCCO_SHACK, 0),
    ENTRANCE(DOGGY_RACETRACK, 0),
    ENTRANCE(RANCH_HOUSE, 0),
    ENTRANCE(RANCH_HOUSE, 1),
    // Mountain Village
    ENTRANCE(GORON_SHOP, 0),
    ENTRANCE(MOUNTAIN_SMITHY, 0),
    // Great Bay
    ENTRANCE(FISHERMANS_HUT, 0),
    ENTRANCE(MARINE_RESEARCH_LAB, 0),
    ENTRANCE(ZORA_HALL_ROOMS, 0),
    ENTRANCE(ZORA_HALL_ROOMS, 1),
    ENTRANCE(ZORA_HALL_ROOMS, 2),
    ENTRANCE(ZORA_HALL_ROOMS, 3),
    ENTRANCE(ZORA_HALL_ROOMS, 5),
    ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0),
    // Ikana
    ENTRANCE(GHOST_HUT, 0),
    ENTRANCE(MUSIC_BOX_HOUSE, 0),
    // Great Fairy Fountains
    ENTRANCE(FAIRY_FOUNTAIN, 1),
    ENTRANCE(FAIRY_FOUNTAIN, 2),
    ENTRANCE(FAIRY_FOUNTAIN, 3),
    ENTRANCE(FAIRY_FOUNTAIN, 4),
    // Other
    ENTRANCE(SWAMP_SPIDER_HOUSE, 0),
};

std::set<s32> dungeonEntrances = {
    ENTRANCE(WOODFALL_TEMPLE, 0),
    ENTRANCE(SNOWHEAD_TEMPLE, 0),
    ENTRANCE(GREAT_BAY_TEMPLE, 0),
};

std::vector<EntrancePair> ConvertSetToEntrancePairs(const std::set<s32>& entranceSet) {
    std::vector<EntrancePair> entrancePairs;
    for (s32 entrance : entranceSet) {
        auto randoRegionId = Rando::Logic::GetRegionIdFromEntrance(entrance);
        for (const auto& [exitId, regionExit] : Rando::Logic::Regions[randoRegionId].exits) {
            if (regionExit.returnEntrance == entrance) {
                entrancePairs.push_back({ entrance, exitId });
                break;
            }
        }
    }

    return entrancePairs;
}

std::vector<EntrancePair> GetInteriorEntrances() {
    return ConvertSetToEntrancePairs(interiorEntrances);
}

std::vector<EntrancePair> GetGrottoEntrances() {
    return {};
}

std::vector<EntrancePair> GetDungeonEntrances() {
    return ConvertSetToEntrancePairs(dungeonEntrances);
}

std::vector<EntrancePair> GetOverworldEntrances() {
    return {
        { ENTRANCE(EAST_CLOCK_TOWN, 3), ENTRANCE(SOUTH_CLOCK_TOWN, 2) },
        { ENTRANCE(WEST_CLOCK_TOWN, 2), ENTRANCE(SOUTH_CLOCK_TOWN, 3) },
        { ENTRANCE(NORTH_CLOCK_TOWN, 2), ENTRANCE(SOUTH_CLOCK_TOWN, 4) },
        { ENTRANCE(WEST_CLOCK_TOWN, 1), ENTRANCE(SOUTH_CLOCK_TOWN, 5) },
        { ENTRANCE(LAUNDRY_POOL, 0), ENTRANCE(SOUTH_CLOCK_TOWN, 6) },
        { ENTRANCE(EAST_CLOCK_TOWN, 1), ENTRANCE(SOUTH_CLOCK_TOWN, 7) },
        { ENTRANCE(NORTH_CLOCK_TOWN, 1), ENTRANCE(EAST_CLOCK_TOWN, 5) },
    };
}

std::vector<EntrancePair> GetEntrancePool(EntrancePoolType poolType) {
    switch (poolType) {
        case POOL_INTERIOR:
            return GetInteriorEntrances();
        case POOL_GROTTO:
            return GetGrottoEntrances();
        case POOL_DUNGEON:
            return GetDungeonEntrances();
        case POOL_OVERWORLD:
            return GetOverworldEntrances();
        default:
            return {};
    }
}

bool IsEntranceShuffleEnabled() {
    return IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES] != RO_ENTRANCE_SHUFFLE_OFF;
}

// Called on file load and in file creation prior to logic calculation
void ShuffleEntrances() {
    sEntranceMap.clear();

    if (!IsEntranceShuffleEnabled()) {
        return;
    }

    Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed);

    auto shuffleType = RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES];

    std::vector<EntrancePoolType> poolsToShuffle;

    switch (shuffleType) {
        case RO_ENTRANCE_SHUFFLE_INTERIORS_ONLY:
            poolsToShuffle = { POOL_INTERIOR };
            break;
        case RO_ENTRANCE_SHUFFLE_DUNGEONS_ONLY:
            poolsToShuffle = { POOL_DUNGEON };
            break;
        case RO_ENTRANCE_SHUFFLE_FULL:
            poolsToShuffle = { POOL_INTERIOR, POOL_DUNGEON, POOL_OVERWORLD };
            break;
        default:
            return;
    }

    // Shuffle each pool independently
    for (auto poolType : poolsToShuffle) {
        std::vector<EntrancePair> originalPool = GetEntrancePool(poolType);
        auto pool = originalPool; // Make a copy to shuffle

        if (pool.size() < 2) {
            continue;
        }

        // Shuffle pool
        for (size_t i = 0; i < pool.size(); i++) {
            size_t j = Ship_Random(0, pool.size() - 1);
            std::swap(pool[i], pool[j]);
        }

        for (size_t i = 0; i < pool.size(); ++i) {
            const auto& from = originalPool[i];
            const auto& to = pool[i];

            if (from.entrance != to.entrance) {
                sEntranceMap[from.entrance] = to.entrance;
            }

            // if (from.isReversible && to.isReversible) {
            if (to.exit != from.exit) {
                sEntranceMap[to.exit] = from.exit;
            }
            // }
        }
    }
}

s32 GetShuffledEntrance(s32 originalEntrance) {
    if (!IsEntranceShuffleEnabled()) {
        return originalEntrance;
    }

    if (sEntranceMap.count(originalEntrance) > 0) {
        return sEntranceMap[originalEntrance];
    }

    return originalEntrance;
}

s32 GetOriginalEntrance(s32 shuffledEntrance) {
    if (!IsEntranceShuffleEnabled()) {
        return shuffledEntrance;
    }

    for (const auto& [orig, mapped] : sEntranceMap) {
        if (mapped == shuffledEntrance) {
            return orig;
        }
    }

    return shuffledEntrance;
}

} // namespace EntranceShuffle

} // namespace Rando
