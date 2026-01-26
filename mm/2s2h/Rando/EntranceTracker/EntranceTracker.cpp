#include "EntranceTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/Rando/Logic/EntranceShuffle.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenPort.h"
#include "2s2h/ShipInit.hpp"
#include <cstring>
#include <queue>
#include <algorithm>

extern "C" {
#include "z64scene.h"
s16 Play_GetOriginalSceneId(s16 sceneId);
}

namespace BenGui {
extern std::shared_ptr<Rando::EntranceTracker::EntranceTrackerWindow> mEntranceTrackerWindow;
}

#define WIDGET_COLOR UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5))

// Scene sorting index (same as CheckTracker)
#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, betterMapSelectIndex, _humanName)                               \
    { enumValue, betterMapSelectIndex },
#define DEFINE_SCENE_UNSET(_enumValue)

static std::unordered_map<s32, s32> betterSceneIndex = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

// Visibility modes
typedef enum {
    ENTRANCE_TRACKER_VISIBILITY_MODE_ALWAYS,
    ENTRANCE_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU,
    ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE,
    ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_HOLD,
} EntranceTrackerVisibilityMode;

static std::unordered_map<int32_t, const char*> sVisibilityModes = {
    { ENTRANCE_TRACKER_VISIBILITY_MODE_ALWAYS, "Always" },
    { ENTRANCE_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU, "Only on Pause Menu" },
    { ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, "Button Toggle" },
    { ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_HOLD, "Button Hold" },
};

static bool sEntranceTrackerBtnState = false;

// CVars
#define CVAR_NAME_SHOW_ENTRANCE_TRACKER "gWindows.EntranceTracker"
#define CVAR_NAME_VISIBILITY_MODE "gRando.EntranceTracker.VisibilityMode"
#define CVAR_NAME_VISIBILITY_BTN "gRando.EntranceTracker.VisibilityBtn"
#define CVAR_NAME_TRACKER_OPACITY "gRando.EntranceTracker.Opacity"
#define CVAR_NAME_TRACKER_SCALE "gRando.EntranceTracker.Scale"
#define CVAR_NAME_SPOILER_MODE "gRando.EntranceTracker.SpoilerMode"
#define CVAR_NAME_SHOW_SEARCH "gRando.EntranceTracker.ShowSearch"
#define CVAR_NAME_EXTENDED_ROUTING "gRando.EntranceTracker.ExtendedRouting"

#define CVAR_SHOW_ENTRANCE_TRACKER CVarGetInteger(CVAR_NAME_SHOW_ENTRANCE_TRACKER, 0)
#define CVAR_VISIBILITY_MODE CVarGetInteger(CVAR_NAME_VISIBILITY_MODE, ENTRANCE_TRACKER_VISIBILITY_MODE_ALWAYS)
#define CVAR_VISIBILITY_BTN CVarGetInteger(CVAR_NAME_VISIBILITY_BTN, BTN_CUSTOM_MODIFIER1)
#define CVAR_TRACKER_OPACITY CVarGetFloat(CVAR_NAME_TRACKER_OPACITY, 0.5f)
#define CVAR_TRACKER_SCALE CVarGetFloat(CVAR_NAME_TRACKER_SCALE, 1.0f)
#define CVAR_SPOILER_MODE CVarGetInteger(CVAR_NAME_SPOILER_MODE, 1) // Default: show all
#define CVAR_SHOW_SEARCH CVarGetInteger(CVAR_NAME_SHOW_SEARCH, 1)
#define CVAR_EXTENDED_ROUTING CVarGetInteger(CVAR_NAME_EXTENDED_ROUTING, 0)

static ImGuiTextFilter sEntranceTrackerFilter;
static float trackerScale = 1.0f;
static ImVec4 trackerBG = ImVec4{ 0, 0, 0, 0.5f };

// Entrance display data structures
struct EntranceConnection {
    s32 originalEntrance;
    s32 shuffledEntrance;
    SceneId sourceScene;
    std::string originalName;
    std::string shuffledName;
};

static std::map<SceneId, std::vector<EntranceConnection>> sEntrancesByArea;
static std::vector<SceneId> sSortedSceneIds;

// Route finder state
static int sRouteFromIndex = 0;
static int sRouteToIndex = 0;
static std::string sRouteResult = "";
static std::vector<std::pair<RandoRegionId, std::string>> sRegionList;
static ImGuiTextFilter sRouteFromFilter;
static ImGuiTextFilter sRouteToFilter;

// Entrance name mapping - built from scene names with spawn-specific suffixes
static std::map<s32, std::string> sEntranceNames;

// Helper to extract scene ID from entrance
static SceneId GetSceneFromEntrance(s32 entrance) {
    return static_cast<SceneId>((entrance >> 9) & 0x7F);
}

// Cache of all exits that are in entrance shuffle pools
static std::set<s32> sPoolExits;

// Build the set of exits that are actually in entrance shuffle pools
static void BuildPoolExitSet() {
    sPoolExits.clear();

    std::vector<Rando::EntranceShuffle::EntrancePoolType> pools = {
        Rando::EntranceShuffle::POOL_INTERIOR,
        Rando::EntranceShuffle::POOL_DUNGEON,
        Rando::EntranceShuffle::POOL_OVERWORLD
    };

    for (auto poolType : pools) {
        auto entrancePairs = Rando::EntranceShuffle::GetEntrancePool(poolType);
        for (const auto& pair : entrancePairs) {
            // Add both the entrance and exit from each pair
            sPoolExits.insert(pair.entrance);
            sPoolExits.insert(pair.exit);
        }
    }
}

// Check if an exit is part of any entrance shuffle pool
static bool IsExitInShufflePool(s32 exitId) {
    return sPoolExits.count(exitId) > 0;
}

// Helper to extract spawn number from entrance
static s32 GetSpawnFromEntrance(s32 entrance) {
    return (entrance >> 4) & 0x1F;
}

// Build entrance name from scene name and spawn number
static std::string BuildEntranceName(s32 entrance) {
    SceneId sceneId = GetSceneFromEntrance(entrance);
    s32 spawn = GetSpawnFromEntrance(entrance);

    std::string baseName = Ship_GetSceneName(sceneId);

    // Add spawn suffix for multi-entrance scenes
    if (spawn > 0) {
        baseName += " (" + std::to_string(spawn + 1) + ")";
    }

    return baseName;
}

std::string Rando::EntranceTracker::GetEntranceName(s32 entranceId) {
    if (sEntranceNames.count(entranceId) > 0) {
        return sEntranceNames[entranceId];
    }
    return BuildEntranceName(entranceId);
}

// Discovery tracking - uses save data for persistence
// Maps entrance IDs to compact indices (0-255) for bitfield storage
static std::map<s32, u8> sEntranceToIndex;
static std::map<u8, s32> sIndexToEntrance;
static u8 sNextEntranceIndex = 0;

// Get or assign an index for an entrance ID
static u8 GetEntranceIndex(s32 entranceId) {
    if (sEntranceToIndex.count(entranceId) == 0) {
        if (sNextEntranceIndex >= 255) {
            return 255; // Fallback if we run out of indices
        }
        sEntranceToIndex[entranceId] = sNextEntranceIndex;
        sIndexToEntrance[sNextEntranceIndex] = entranceId;
        sNextEntranceIndex++;
    }
    return sEntranceToIndex[entranceId];
}

// Build the entrance index mapping from known entrance pools
static void BuildEntranceIndexMap() {
    sEntranceToIndex.clear();
    sIndexToEntrance.clear();
    sNextEntranceIndex = 0;

    // Add all entrances from the pools to ensure consistent indexing
    std::vector<Rando::EntranceShuffle::EntrancePoolType> pools = {
        Rando::EntranceShuffle::POOL_INTERIOR,
        Rando::EntranceShuffle::POOL_DUNGEON,
        Rando::EntranceShuffle::POOL_OVERWORLD
    };

    for (auto poolType : pools) {
        auto entrancePairs = Rando::EntranceShuffle::GetEntrancePool(poolType);
        for (const auto& pair : entrancePairs) {
            GetEntranceIndex(pair.entrance);
            GetEntranceIndex(pair.exit);
        }
    }
}

void Rando::EntranceTracker::SetEntranceDiscovered(s32 entranceId) {
    if (!IS_RANDO) {
        return;
    }

    u8 index = GetEntranceIndex(entranceId);
    if (index < 255) {
        u8 byteIndex = index / 8;
        u8 bitIndex = index % 8;
        gSaveContext.save.shipSaveInfo.rando.discoveredEntrances[byteIndex] |= (1 << bitIndex);
    }
}

bool Rando::EntranceTracker::IsEntranceDiscovered(s32 entranceId) {
    if (!IS_RANDO) {
        return false;
    }

    u8 index = GetEntranceIndex(entranceId);
    if (index >= 255) {
        return false;
    }

    u8 byteIndex = index / 8;
    u8 bitIndex = index % 8;
    return (gSaveContext.save.shipSaveInfo.rando.discoveredEntrances[byteIndex] & (1 << bitIndex)) != 0;
}

// List of generic region names that need scene context
static const std::set<std::string> sGenericRegionNames = {
    "Entrance", "Main Room", "Maze Room", "Upper", "Lower", "Back", "Front",
    "Boss Room", "Passage", "Main Room Upper", "Main Room Lower",
    "Night 1 Boss", "Night 2 Boss", "Higher", "Before Great Bay Coast"
};

// Get a display name for a region, using scene name as fallback or prefix for generic names
static std::string GetRegionDisplayName(RandoRegionId regionId) {
    if (Rando::Logic::Regions.count(regionId) == 0) {
        return "";
    }

    const auto& region = Rando::Logic::Regions.at(regionId);
    std::string name = region.name;
    std::string sceneName = Ship_GetSceneName(region.sceneId);

    // If name is empty, use scene name
    if (name.empty()) {
        return sceneName;
    }

    // If name is generic, prefix with scene name
    if (sGenericRegionNames.count(name) > 0) {
        return sceneName + " - " + name;
    }

    return name;
}

// Build the entrance name mapping with better names for specific entrances
static void BuildEntranceNameMap() {
    sEntranceNames.clear();

    // Add custom names for specific entrances (more descriptive than just scene name)
    sEntranceNames[ENTRANCE(STOCK_POT_INN, 0)] = "Stock Pot Inn (Main Entrance)";
    sEntranceNames[ENTRANCE(STOCK_POT_INN, 1)] = "Stock Pot Inn (Upper Floor)";
    sEntranceNames[ENTRANCE(CURIOSITY_SHOP, 0)] = "Curiosity Shop (Front Entrance)";
    sEntranceNames[ENTRANCE(CURIOSITY_SHOP, 1)] = "Curiosity Shop (Back Entrance)";
    sEntranceNames[ENTRANCE(RANCH_HOUSE, 0)] = "Romani Ranch House (Main Building)";
    sEntranceNames[ENTRANCE(RANCH_HOUSE, 1)] = "Romani Ranch House (Barn)";
    sEntranceNames[ENTRANCE(ZORA_HALL_ROOMS, 0)] = "Zora Hall (Evan's Room)";
    sEntranceNames[ENTRANCE(ZORA_HALL_ROOMS, 1)] = "Zora Hall (Lulu's Room)";
    sEntranceNames[ENTRANCE(ZORA_HALL_ROOMS, 2)] = "Zora Hall (Japas' Room)";
    sEntranceNames[ENTRANCE(ZORA_HALL_ROOMS, 3)] = "Zora Hall (Tijo's Room)";
    sEntranceNames[ENTRANCE(ZORA_HALL_ROOMS, 5)] = "Zora Hall (Shop)";
    sEntranceNames[ENTRANCE(FAIRY_FOUNTAIN, 0)] = "Great Fairy (Clock Town)";
    sEntranceNames[ENTRANCE(FAIRY_FOUNTAIN, 1)] = "Great Fairy (Woodfall)";
    sEntranceNames[ENTRANCE(FAIRY_FOUNTAIN, 2)] = "Great Fairy (Snowhead)";
    sEntranceNames[ENTRANCE(FAIRY_FOUNTAIN, 3)] = "Great Fairy (Great Bay)";
    sEntranceNames[ENTRANCE(FAIRY_FOUNTAIN, 4)] = "Great Fairy (Ikana)";
    sEntranceNames[ENTRANCE(CLOCK_TOWER_INTERIOR, 0)] = "Clock Tower Interior (Main)";
    sEntranceNames[ENTRANCE(CLOCK_TOWER_INTERIOR, 1)] = "Clock Tower Interior (Upper)";

    // Temple entrances - be specific about which entrance/exit
    sEntranceNames[ENTRANCE(GREAT_BAY_TEMPLE, 0)] = "Great Bay Temple (Main Entrance)";
    sEntranceNames[ENTRANCE(GREAT_BAY_TEMPLE, 1)] = "Great Bay Temple (Main Exit)";
    sEntranceNames[ENTRANCE(WOODFALL_TEMPLE, 0)] = "Woodfall Temple (Main Entrance)";
    sEntranceNames[ENTRANCE(WOODFALL_TEMPLE, 1)] = "Woodfall Temple (Main Exit)";
    sEntranceNames[ENTRANCE(SNOWHEAD_TEMPLE, 0)] = "Snowhead Temple (Main Entrance)";
    sEntranceNames[ENTRANCE(SNOWHEAD_TEMPLE, 1)] = "Snowhead Temple (Main Exit)";
    sEntranceNames[ENTRANCE(IKANA_CASTLE, 0)] = "Ikana Castle (Main Entrance)";
    sEntranceNames[ENTRANCE(IKANA_CASTLE, 1)] = "Ikana Castle (Main Exit)";
    sEntranceNames[ENTRANCE(STONE_TOWER_TEMPLE, 0)] = "Stone Tower Temple (Main Entrance)";
    sEntranceNames[ENTRANCE(STONE_TOWER_TEMPLE, 1)] = "Stone Tower Temple (Main Exit)";

    // Overworld connections - use more descriptive names
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 0)] = "East Clock Town (to Termina Field)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 1)] = "East Clock Town (Chest Shop)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 2)] = "East Clock Town (Astral Observatory)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 3)] = "East Clock Town (from South)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 4)] = "East Clock Town (Treasure Chest Shop)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 5)] = "East Clock Town (Great Fairy)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 6)] = "East Clock Town (Honey and Darlings)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 7)] = "East Clock Town (Mayor's Residence)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 8)] = "East Clock Town (Shooting Gallery)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 9)] = "East Clock Town (Stock Pot Inn)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 10)] = "East Clock Town (Stock Pot Inn Upstairs)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 11)] = "East Clock Town (Milk Bar)";
    
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 0)] = "South Clock Town (Clock Tower Interior)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 2)] = "South Clock Town (from East)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 3)] = "South Clock Town (from West)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 4)] = "South Clock Town (from North)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 5)] = "South Clock Town (Swordsman)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 6)] = "South Clock Town (Laundry)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 7)] = "South Clock Town (Chest Shop)";
    
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 0)] = "West Clock Town (to Termina Field)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 1)] = "West Clock Town (to South Lower)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 2)] = "West Clock Town (to South Upper)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 3)] = "West Clock Town (Swordsman)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 4)] = "West Clock Town (Curiosity Shop)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 5)] = "West Clock Town (Trading Post)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 6)] = "West Clock Town (Bomb Shop)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 7)] = "West Clock Town (Post Office)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 8)] = "West Clock Town (Lottery Shop)";
    
    sEntranceNames[ENTRANCE(NORTH_CLOCK_TOWN, 1)] = "North Clock Town (Great Fairy)";
    sEntranceNames[ENTRANCE(NORTH_CLOCK_TOWN, 2)] = "North Clock Town (from South)";
    sEntranceNames[ENTRANCE(NORTH_CLOCK_TOWN, 3)] = "North Clock Town (Great Fairy Fountain)";
    
    sEntranceNames[ENTRANCE(LAUNDRY_POOL, 0)] = "Laundry Pool (Entrance)";
    sEntranceNames[ENTRANCE(LAUNDRY_POOL, 1)] = "Laundry Pool (Back Alley)";

    // Additional interior shops/buildings from Central.cpp
    sEntranceNames[ENTRANCE(SWORDMANS_SCHOOL, 0)] = "Swordsman's School";
    sEntranceNames[ENTRANCE(TREASURE_CHEST_SHOP, 0)] = "Treasure Chest Shop";
    sEntranceNames[ENTRANCE(TOWN_SHOOTING_GALLERY, 0)] = "Shooting Gallery";
    sEntranceNames[ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0)] = "Deku Playground";
    sEntranceNames[ENTRANCE(TRADING_POST, 0)] = "Trading Post";
    sEntranceNames[ENTRANCE(POST_OFFICE, 0)] = "Post Office";
    sEntranceNames[ENTRANCE(MILK_BAR, 0)] = "Milk Bar";
    sEntranceNames[ENTRANCE(LOTTERY_SHOP, 0)] = "Lottery Shop";
    sEntranceNames[ENTRANCE(MAYORS_RESIDENCE, 0)] = "Mayor's Residence";
    sEntranceNames[ENTRANCE(BOMB_SHOP, 0)] = "Bomb Shop";
    sEntranceNames[ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0)] = "Honey and Darling's Shop";
    sEntranceNames[ENTRANCE(ASTRAL_OBSERVATORY, 0)] = "Astral Observatory";

    // Termina Field & Roads
    sEntranceNames[ENTRANCE(TOURIST_INFORMATION, 0)] = "Tourist Information";
    sEntranceNames[ENTRANCE(SWAMP_SHOOTING_GALLERY, 0)] = "Swamp Shooting Gallery";
    sEntranceNames[ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0)] = "Magic Hag's Potion Shop";

    // Milk Road
    sEntranceNames[ENTRANCE(CUCCO_SHACK, 0)] = "Cucco Shack";
    sEntranceNames[ENTRANCE(DOGGY_RACETRACK, 0)] = "Doggy Racetrack";
    sEntranceNames[ENTRANCE(RANCH_HOUSE, 0)] = "Ranch House (Main)";
    sEntranceNames[ENTRANCE(RANCH_HOUSE, 1)] = "Ranch House (Barn)";

    // Great Bay
    sEntranceNames[ENTRANCE(FISHERMANS_HUT, 0)] = "Fisherman's Hut";
    sEntranceNames[ENTRANCE(MARINE_RESEARCH_LAB, 0)] = "Marine Research Lab";
    sEntranceNames[ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0)] = "Oceanside Spider House";
    sEntranceNames[ENTRANCE(PINNACLE_ROCK, 0)] = "Pinnacle Rock";

    // Mountain Village
    sEntranceNames[ENTRANCE(GORON_SHOP, 0)] = "Goron Shop";

    // Swamp
    sEntranceNames[ENTRANCE(SWAMP_SPIDER_HOUSE, 0)] = "Swamp Spider House";

    // Stone Tower
    sEntranceNames[ENTRANCE(STONE_TOWER, 0)] = "Stone Tower";
    sEntranceNames[ENTRANCE(STONE_TOWER, 1)] = "Stone Tower (Inverted Entrance)";
    sEntranceNames[ENTRANCE(STONE_TOWER, 2)] = "Stone Tower (to Temple)";

    // Ghost Hut entrances (from Ikana Canyon)
    sEntranceNames[ENTRANCE(GHOST_HUT, 0)] = "Ghost Hut";
    sEntranceNames[ENTRANCE(GHOST_HUT, 1)] = "Ghost Hut (Alternate)";
    sEntranceNames[ENTRANCE(GHOST_HUT, 2)] = "Ghost Hut (Another Route)";

    // Ikana
    sEntranceNames[ENTRANCE(MUSIC_BOX_HOUSE, 0)] = "Music Box House";
}

// Get the source scene for an entrance pair (where you are when you use the exit)
static SceneId GetSourceSceneForEntrancePair(s32 returnEntrance) {
    // returnEntrance is the entrance ID that you return to when exiting a destination
    // We need to find which region has this as a returnEntrance
    // The returnEntrance field is in the value of the exits map
    
    for (const auto& [regionId, region] : Rando::Logic::Regions) {
        for (const auto& [_, regionExit] : region.exits) {
            if (regionExit.returnEntrance == returnEntrance) {
                // Found it! This region has this return entrance
                return region.sceneId;
            }
        }
    }
    
    // Fallback: try to extract scene from the return entrance ID itself
    return GetSceneFromEntrance(returnEntrance);
}

// Build entrance data grouped by source area
static void BuildEntranceData() {
    sEntrancesByArea.clear();
    sSortedSceneIds.clear();

    // Build the set of pool exits for route finding
    BuildPoolExitSet();

    if (!Rando::EntranceShuffle::IsEntranceShuffleEnabled()) {
        return;
    }

    // Get all entrance pools and process each
    std::vector<Rando::EntranceShuffle::EntrancePoolType> pools = {
        Rando::EntranceShuffle::POOL_INTERIOR,
        Rando::EntranceShuffle::POOL_DUNGEON,
        Rando::EntranceShuffle::POOL_OVERWORLD
    };

    for (auto poolType : pools) {
        auto entrancePairs = Rando::EntranceShuffle::GetEntrancePool(poolType);

        for (const auto& pair : entrancePairs) {
            s32 original = pair.entrance;
            s32 shuffled = Rando::EntranceShuffle::GetShuffledEntrance(original);
            s32 exitId = pair.exit;  // This is the exit from the SOURCE region

            // Get source scene using the exit ID from the source region
            SceneId sourceScene = GetSourceSceneForEntrancePair(exitId);

            EntranceConnection conn;
            conn.originalEntrance = original;
            conn.shuffledEntrance = shuffled;
            conn.sourceScene = sourceScene;
            conn.originalName = Rando::EntranceTracker::GetEntranceName(original);
            conn.shuffledName = Rando::EntranceTracker::GetEntranceName(shuffled);

            sEntrancesByArea[sourceScene].push_back(conn);
        }
    }

    // Build sorted scene list
    for (const auto& [sceneId, _] : sEntrancesByArea) {
        sSortedSceneIds.push_back(sceneId);
    }

    // Sort by betterSceneIndex
    std::sort(sSortedSceneIds.begin(), sSortedSceneIds.end(),
              [](SceneId a, SceneId b) { return betterSceneIndex[a] < betterSceneIndex[b]; });
}

// Helper: Find which region a shuffled entrance leads to by checking entrance pools
static RandoRegionId FindRegionFromShuffledEntrance(s32 shuffledEntrance) {
    // First try the direct lookup (for entrances)
    RandoRegionId direct = Rando::Logic::GetRegionIdFromEntrance(shuffledEntrance);
    if (direct != RR_MAX) {
        return direct;
    }
    
    // If direct lookup fails, check all entrance pairs to find which original entrance/exit
    // this shuffled entrance corresponds to, then look up that region
    std::vector<Rando::EntranceShuffle::EntrancePoolType> pools = {
        Rando::EntranceShuffle::POOL_INTERIOR,
        Rando::EntranceShuffle::POOL_DUNGEON,
        Rando::EntranceShuffle::POOL_OVERWORLD
    };
    
    for (auto poolType : pools) {
        auto entrancePairs = Rando::EntranceShuffle::GetEntrancePool(poolType);
        for (const auto& pair : entrancePairs) {
            // Check if shuffled entrance matches shuffled version of the original entrance
            s32 shuffledFromEntrance = Rando::EntranceShuffle::GetShuffledEntrance(pair.entrance);
            if (shuffledFromEntrance == shuffledEntrance) {
                // Found it! The original entrance is pair.entrance
                // Now look up that region
                RandoRegionId region = Rando::Logic::GetRegionIdFromEntrance(pair.entrance);
                if (region != RR_MAX) {
                    return region;
                }
            }
            
            // Also check if shuffled entrance matches shuffled version of the original exit
            s32 shuffledFromExit = Rando::EntranceShuffle::GetShuffledEntrance(pair.exit);
            if (shuffledFromExit == shuffledEntrance) {
                // This is trickier - we have a shuffled exit. Find which region has this exit.
                // The exit points TO a region, so look for the region that has this as an exit
                for (const auto& [rid, rdata] : Rando::Logic::Regions) {
                    for (const auto& [exitId, regionExit] : rdata.exits) {
                        if (Rando::EntranceShuffle::GetShuffledEntrance(exitId) == shuffledEntrance) {
                            return rid;
                        }
                    }
                }
            }
        }
    }
    
    // Fallback for extended routing: try to find ANY region that has this entrance/exit
    // This allows us to trace non-shuffled connections when extended routing is enabled
    if (CVAR_EXTENDED_ROUTING) {
        // Try looking for a region with this entrance
        for (const auto& [rid, rdata] : Rando::Logic::Regions) {
            for (const auto& [exitId, regionExit] : rdata.exits) {
                if (exitId == shuffledEntrance || Rando::EntranceShuffle::GetShuffledEntrance(exitId) == shuffledEntrance) {
                    return rid;
                }
            }
        }
        
        // As a last resort, try by scene ID
        SceneId targetScene = GetSceneFromEntrance(shuffledEntrance);
        for (const auto& [rid, rdata] : Rando::Logic::Regions) {
            if (rdata.sceneId == targetScene) {
                return rid;
            }
        }
    }
    
    return RR_MAX;
}
static void BuildRegionList() {
    sRegionList.clear();

    // Include all regions from the logic system
    for (const auto& [regionId, region] : Rando::Logic::Regions) {
        std::string displayName = GetRegionDisplayName(regionId);
        if (!displayName.empty()) {
            sRegionList.push_back({ regionId, displayName });
        }
    }

    // Sort alphabetically
    std::sort(sRegionList.begin(), sRegionList.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
}

// Route finding using BFS
struct RouteStep {
    RandoRegionId region;
    s32 entranceUsed; // -1 for connections (same-scene transitions)
    std::string description;
};

static std::vector<RouteStep> FindRoute(RandoRegionId from, RandoRegionId to) {
    std::vector<RouteStep> result;

    if (from == to) {
        return result; // Already there
    }

    std::queue<std::vector<RouteStep>> queue;
    std::set<RandoRegionId> visited;

    // Start from the source region
    queue.push({});
    visited.insert(from);

    RandoRegionId current = from;

    while (!queue.empty()) {
        auto path = queue.front();
        queue.pop();

        // Determine current region from path
        current = path.empty() ? from : path.back().region;

        if (Rando::Logic::Regions.count(current) == 0) {
            continue;
        }

        auto& region = Rando::Logic::Regions[current];

        // Check exits - only traverse exits that are in entrance shuffle pools (unless extended routing is enabled)
        for (const auto& [exitId, regionExit] : region.exits) {
            // Only traverse exits that are part of the entrance shuffle pools
            // This prevents the BFS from exploring internal logic connections
            // Extended routing bypasses this check for more complete (but messier) routes
            if (!CVAR_EXTENDED_ROUTING && !IsExitInShufflePool(exitId)) {
                continue;
            }

            s32 actualExit = Rando::EntranceShuffle::GetShuffledEntrance(exitId);

            // Find the region this shuffled entrance leads to
            RandoRegionId targetRegion = FindRegionFromShuffledEntrance(actualExit);

            // If we can't determine where this exit leads:
            // - Normal mode: skip it
            // - Extended routing: try scene-based lookup as fallback
            if (targetRegion == RR_MAX) {
                if (CVAR_EXTENDED_ROUTING) {
                    // Try to find any region in the target scene
                    SceneId targetScene = GetSceneFromEntrance(actualExit);
                    for (const auto& [rid, rdata] : Rando::Logic::Regions) {
                        if (rdata.sceneId == targetScene) {
                            targetRegion = rid;
                            break;
                        }
                    }
                }
                
                // Still no region found, skip this exit
                if (targetRegion == RR_MAX) {
                    continue;
                }
            }

            if (targetRegion == to) {
                // Found the destination
                result = path;
                RouteStep step;
                step.region = to;
                step.entranceUsed = exitId;
                // Use the source entrance name (what you use from current location), not the destination
                step.description = Rando::EntranceTracker::GetEntranceName(exitId);
                result.push_back(step);
                return result;
            }

            if (targetRegion != RR_MAX && visited.find(targetRegion) == visited.end()) {
                visited.insert(targetRegion);
                auto newPath = path;
                RouteStep step;
                step.region = targetRegion;
                step.entranceUsed = exitId;
                // Use the source entrance name (what you use from current location), not the destination
                step.description = Rando::EntranceTracker::GetEntranceName(exitId);
                newPath.push_back(step);
                queue.push(newPath);
            }
        }

        // In extended routing mode, also allow traversing connections (same-scene transitions)
        // Normal mode skips these to avoid garbage routes through internal regions
        if (CVAR_EXTENDED_ROUTING) {
            for (const auto& [connectedRegion, condition] : region.connections) {
                if (connectedRegion == to) {
                    result = path;
                    RouteStep step;
                    step.region = to;
                    step.entranceUsed = -1;
                    step.description = "(internal transition)";
                    result.push_back(step);
                    return result;
                }

                if (visited.find(connectedRegion) == visited.end()) {
                    visited.insert(connectedRegion);
                    auto newPath = path;
                    RouteStep step;
                    step.region = connectedRegion;
                    step.entranceUsed = -1;
                    step.description = "(internal transition)";
                    newPath.push_back(step);
                    queue.push(newPath);
                }
            }
        }
    }

    return result; // Empty if no route found
}

// Format route for display (compact format)
static std::string FormatRoute(RandoRegionId from, const std::vector<RouteStep>& route) {
    if (route.empty()) {
        return "No route found";
    }

    std::string result;
    std::string fromName = GetRegionDisplayName(from);

    if (!fromName.empty()) {
        result = fromName;
    }

    for (const auto& step : route) {
        // Show the entrance you take to get there
        if (!step.description.empty()) {
            result += " -> " + step.description;
        }
    }

    return result.empty() ? "No route found" : result;
}

namespace Rando {

namespace EntranceTracker {

void EntranceTrackerWindow::Draw() {
    if (!CVAR_SHOW_ENTRANCE_TRACKER) {
        return;
    }

    if (CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU &&
        (!gPlayState || !gPlayState->pauseCtx.state)) {
        return;
    }

    if ((CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
         CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) &&
        !sEntranceTrackerBtnState) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::SetNextWindowSize(ImVec2(450.0f, 500.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Entrance Tracker", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing);

    trackerBG.w = ImGui::IsWindowDocked() ? 1.0f : CVAR_TRACKER_OPACITY;
    trackerScale = CVAR_TRACKER_SCALE;
    ImGui::SetWindowFontScale(trackerScale);

    if (!gPlayState || !IS_RANDO) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("No Rando Save Loaded").x) / 2);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10.0f);
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Gray), "No Rando Save Loaded");
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(1);
        return;
    }

    if (!Rando::EntranceShuffle::IsEntranceShuffleEnabled()) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Entrance Shuffle Not Enabled").x) / 2);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10.0f);
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Gray), "Entrance Shuffle Not Enabled");
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(1);
        return;
    }

    // Tab bar for Entrances and Route Finder
    if (ImGui::BeginTabBar("EntranceTrackerTabs")) {
        if (ImGui::BeginTabItem("Entrances")) {
            ImGui::BeginChild("EntranceList");

            // Search bar for entrances tab
            if (CVAR_SHOW_SEARCH) {
                UIWidgets::PushStyleInput();
                sEntranceTrackerFilter.Draw("##filter", ImGui::GetContentRegionAvail().x - 40.0f);
                UIWidgets::PopStyleInput();

                ImGui::SameLine();
                if (!sEntranceTrackerFilter.IsActive()) {
                    ImGui::Text("Search");
                } else {
                    if (UIWidgets::Button(ICON_FA_TIMES, UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                        sEntranceTrackerFilter.Clear();
                    }
                }
            }

            bool spoilerMode = CVAR_SPOILER_MODE;

            for (SceneId sceneId : sSortedSceneIds) {
                auto& entrances = sEntrancesByArea[sceneId];

                // Filter entrances
                std::vector<const EntranceConnection*> filtered;
                for (const auto& conn : entrances) {
                    // Check spoiler mode
                    if (!spoilerMode && !IsEntranceDiscovered(conn.shuffledEntrance)) {
                        continue;
                    }

                    // Check search filter
                    std::string searchStr = conn.originalName + " " + conn.shuffledName;
                    if (!sEntranceTrackerFilter.PassFilter(searchStr.c_str())) {
                        continue;
                    }

                    filtered.push_back(&conn);
                }

                if (filtered.empty()) {
                    continue;
                }

                ImGui::PushID(sceneId);
                ImGui::Separator();

                std::string headerText = Ship_GetSceneName(sceneId);
                headerText += " (" + std::to_string(filtered.size()) + ")";

                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));

                if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent(20.0f);

                    for (const auto* conn : filtered) {
                        std::string line = conn->originalName + " -> " + conn->shuffledName;
                        ImGui::Text("%s", line.c_str());
                    }

                    ImGui::Unindent(20.0f);
                }

                ImGui::PopStyleColor();
                ImGui::PopID();
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Route Finder")) {
            ImGui::BeginChild("RouteFinder");

            // Build region list if empty
            if (sRegionList.empty()) {
                BuildRegionList();
            }

            // From section with search
            ImGui::Text("From:");
            ImGui::SameLine();
            
            // Calculate available width for combo
            float availWidth = ImGui::GetContentRegionAvail().x - 100.0f;
            
            if (ImGui::BeginCombo("##FromRegion", sRouteFromIndex < (int)sRegionList.size()
                                  ? sRegionList[sRouteFromIndex].second.c_str() : "Select...", 
                                  ImGuiComboFlags_HeightLarge)) {
                
                // Search filter for From
                sRouteFromFilter.Draw("##FromSearch", availWidth);
                ImGui::Separator();
                
                int visibleCount = 0;
                for (int i = 0; i < (int)sRegionList.size(); i++) {
                    if (!sRouteFromFilter.PassFilter(sRegionList[i].second.c_str())) {
                        continue;
                    }
                    
                    visibleCount++;
                    ImGui::PushID(i);
                    bool selected = (i == sRouteFromIndex);
                    if (ImGui::Selectable(sRegionList[i].second.c_str(), selected)) {
                        sRouteFromIndex = i;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                    ImGui::PopID();
                }
                
                if (visibleCount == 0) {
                    ImGui::TextDisabled("No matches");
                }
                
                ImGui::EndCombo();
            }

            // "Use Current Location" button
            ImGui::SameLine();
            if (UIWidgets::Button("Current", UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                if (gPlayState) {
                    RandoRegionId currentRegion = Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance);
                    bool found = false;

                    // First try: find exact region match
                    if (currentRegion != RR_MAX) {
                        for (int i = 0; i < (int)sRegionList.size(); i++) {
                            if (sRegionList[i].first == currentRegion) {
                                sRouteFromIndex = i;
                                found = true;
                                break;
                            }
                        }
                    }

                    // Fallback: find a region matching the current scene
                    if (!found) {
                        SceneId currentScene = static_cast<SceneId>(gPlayState->sceneId);
                        for (int i = 0; i < (int)sRegionList.size(); i++) {
                            RandoRegionId regionId = sRegionList[i].first;
                            if (Rando::Logic::Regions.count(regionId) > 0 &&
                                Rando::Logic::Regions.at(regionId).sceneId == currentScene) {
                                sRouteFromIndex = i;
                                break;
                            }
                        }
                    }
                }
            }
            UIWidgets::Tooltip("Use current location");

            // To section with search
            ImGui::Text("To:");
            ImGui::SameLine();
            
            if (ImGui::BeginCombo("##ToRegion", sRouteToIndex < (int)sRegionList.size()
                                  ? sRegionList[sRouteToIndex].second.c_str() : "Select...",
                                  ImGuiComboFlags_HeightLarge)) {
                
                // Search filter for To
                sRouteToFilter.Draw("##ToSearch", availWidth);
                ImGui::Separator();
                
                int visibleCount = 0;
                for (int i = 0; i < (int)sRegionList.size(); i++) {
                    if (!sRouteToFilter.PassFilter(sRegionList[i].second.c_str())) {
                        continue;
                    }
                    
                    visibleCount++;
                    ImGui::PushID(i);
                    bool selected = (i == sRouteToIndex);
                    if (ImGui::Selectable(sRegionList[i].second.c_str(), selected)) {
                        sRouteToIndex = i;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                    ImGui::PopID();
                }
                
                if (visibleCount == 0) {
                    ImGui::TextDisabled("No matches");
                }
                
                ImGui::EndCombo();
            }

            // Swap From/To button
            ImGui::SameLine();
            if (UIWidgets::Button(ICON_FA_EXCHANGE, UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                std::swap(sRouteFromIndex, sRouteToIndex);
            }
            UIWidgets::Tooltip("Swap From and To locations");

            // Find Route button
            if (UIWidgets::Button("Find Route", UIWidgets::ButtonOptions().Color(UIWidgets::Colors::Green))) {
                if (sRouteFromIndex < (int)sRegionList.size() && sRouteToIndex < (int)sRegionList.size()) {
                    RandoRegionId from = sRegionList[sRouteFromIndex].first;
                    RandoRegionId to = sRegionList[sRouteToIndex].first;
                    
                    // Check if spoilers are off and destination is undiscovered
                    bool spoilerMode = CVAR_SPOILER_MODE;
                    bool destinationDiscovered = true;
                    
                    if (!spoilerMode && Rando::Logic::Regions.count(to) > 0) {
                        // Check if any entrance to this region has been discovered
                        SceneId destScene = Rando::Logic::Regions.at(to).sceneId;
                        destinationDiscovered = false;
                        
                        // Iterate through all regions to find entrances leading to the destination
                        for (const auto& [regionId, region] : Rando::Logic::Regions) {
                            for (const auto& [exitId, regionExit] : region.exits) {
                                if (regionExit.returnEntrance == ONE_WAY_EXIT) continue;
                                if (GetSceneFromEntrance(regionExit.returnEntrance) == destScene) {
                                    if (IsEntranceDiscovered(regionExit.returnEntrance)) {
                                        destinationDiscovered = true;
                                        break;
                                    }
                                }
                            }
                            if (destinationDiscovered) break;
                        }
                    }
                    
                    if (!destinationDiscovered) {
                        sRouteResult = "No route could be found";
                    } else {
                        auto route = FindRoute(from, to);
                        sRouteResult = FormatRoute(from, route);
                    }
                }
            }

            // Route result
            ImGui::Separator();
            ImGui::TextWrapped("%s", sRouteResult.c_str());

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
}

void EntranceTrackerSettingsWindow::DrawElement() {
    if (CVarGetInteger("gWindows.EntranceTracker", 0)) {
        UIWidgets::WindowButton("Disable Entrance Tracker", "gWindows.EntranceTracker", BenGui::mEntranceTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red });
    } else {
        UIWidgets::WindowButton("Enable Entrance Tracker", "gWindows.EntranceTracker", BenGui::mEntranceTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green });
    }

    if (ImGui::BeginTable("Settings Table", 2)) {
        ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("col2", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();

        ImGui::SeparatorText("Display Settings");

        UIWidgets::CVarCheckbox("Show All Entrances (Spoilers)", CVAR_NAME_SPOILER_MODE,
                                UIWidgets::CheckboxOptions().DefaultValue(true));
        UIWidgets::Tooltip("When disabled, only shows entrances you have discovered by using them.");

        UIWidgets::CVarCheckbox("Show Search", CVAR_NAME_SHOW_SEARCH, UIWidgets::CheckboxOptions().DefaultValue(true));

        UIWidgets::CVarCheckbox("Extended Routing (Experimental)", CVAR_NAME_EXTENDED_ROUTING);
        UIWidgets::Tooltip(
            "Enables routing through areas not yet in entrance shuffle pools.\n\n"
            "Routes may include internal logic regions that don't correspond to "
            "actual in-game locations. You'll need to use your knowledge of the "
            "vanilla world layout to interpret these steps.");

        ImGui::TableNextColumn();
        ImGui::SeparatorText("Window Settings");

        UIWidgets::CVarCombobox("Visibility", CVAR_NAME_VISIBILITY_MODE, &sVisibilityModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(ENTRANCE_TRACKER_VISIBILITY_MODE_ALWAYS)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));

        if (CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
            CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
            UIWidgets::CVarBtnSelector("Button Combination:", CVAR_NAME_VISIBILITY_BTN,
                                       UIWidgets::BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER1));
        }

        if (UIWidgets::CVarSliderFloat("Opacity", CVAR_NAME_TRACKER_OPACITY,
                                       {
                                           .format = "Opacity: %.1f",
                                           .step = 0.10f,
                                           .min = 0.0f,
                                           .max = 1.0f,
                                           .defaultValue = 0.5f,
                                           .labelPosition = UIWidgets::LabelPosition::None,
                                       })) {
            trackerBG.w = CVAR_TRACKER_OPACITY;
        }

        if (UIWidgets::CVarSliderFloat("Scale", CVAR_NAME_TRACKER_SCALE,
                                       {
                                           .format = "Scale: %.1f",
                                           .step = 0.10f,
                                           .min = 0.7f,
                                           .max = 2.5f,
                                           .defaultValue = 1.0f,
                                           .labelPosition = UIWidgets::LabelPosition::None,
                                       })) {
            trackerScale = CVAR_TRACKER_SCALE;
        }

        ImGui::EndTable();
    }
}

void Init() {
    BuildEntranceNameMap();
    BuildEntranceIndexMap();

    trackerBG = { 0, 0, 0, CVAR_TRACKER_OPACITY };
    trackerScale = CVAR_TRACKER_SCALE;
}

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnGameStateMainStart, CVAR_VISIBILITY_MODE >= ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, []() {
            Input* input = CONTROLLER1(gGameState);

            if (CVAR_VISIBILITY_MODE == ENTRANCE_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN)) {
                    sEntranceTrackerBtnState = true;
                } else {
                    sEntranceTrackerBtnState = false;
                }
            } else {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN) &&
                    CHECK_BTN_ANY(input->press.button, CVAR_VISIBILITY_BTN)) {
                    sEntranceTrackerBtnState = !sEntranceTrackerBtnState;
                }
            }
        });
    },
    { CVAR_NAME_VISIBILITY_MODE });

void OnFileLoad() {
    if (!IS_RANDO) {
        return;
    }

    BuildEntranceIndexMap();
    BuildEntranceData();
    BuildRegionList();
    sRouteResult = "";
}

} // namespace EntranceTracker

} // namespace Rando
