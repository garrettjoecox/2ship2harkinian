#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_WOODFALL_TEMPLE_BOSS_KEY_ROOM] = RandoRegion{ .name = "Boss Key Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(GEKKO)?
            CHECK(RC_WOODFALL_TEMPLE_BOSS_KEY_CHEST, HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE)),
            CHECK(RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_1, HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE)),
            CHECK(RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_2, HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE)),
            CHECK(RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_3, HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE)),
            CHECK(RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_4, HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE)),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_BOSS_ROOM] = RandoRegion{ .sceneId = SCENE_MITURIN_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Odolwa)?
            CHECK(RC_WOODFALL_TEMPLE_BOSS_CONTAINER, true),
            CHECK(RC_WOODFALL_TEMPLE_BOSS_WARP, true),
            CHECK(RC_GIANTS_CHAMBER_OATH, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 1),                       ONE_WAY_EXIT, true),
        },
        .events = {
            // TODO: CAN_KILL_BOSS(Odolwa)?
            EVENT_WEEKEVENTREG("Clear Woodfall Temple", WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE, true),
        },
        .oneWayEntrances = {
            ENTRANCE(ODOLWAS_LAIR, 0), // From Woodfall Temple Pre-Boss Room
        },
    };
    Regions[RR_WOODFALL_TEMPLE_BOW_ROOM] = RandoRegion{ .name = "Bow Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DINOLFOS)?
            CHECK(RC_WOODFALL_TEMPLE_BOW, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_COMPASS_ROOM] = RandoRegion{ .name = "Compass Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DRAGONFLY)?
            CHECK(RC_WOODFALL_TEMPLE_COMPASS, CAN_BE_DEKU || CAN_USE_EXPLOSIVE),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAZE_ROOM, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_DARK_ROOM] = RandoRegion{ .name = "Dark Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DARK_FOG_DUDE)?
            CHECK(RC_WOODFALL_TEMPLE_DARK_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, CAN_BE_DEKU && CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CONNECTION(RR_WOODFALL_TEMPLE_MAZE_ROOM, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_DEKU_PRINCESS_ROOM] = RandoRegion{ .name = "Deku Princess Room", .sceneId = SCENE_MITURIN,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 3),                     ENTRANCE(WOODFALL_TEMPLE, 2), true),
        },
        .events = {
            EVENT_ACCESS(RANDO_ACCESS_DEKU_PRINCESS, true),
        },
        .oneWayEntrances = {
            ENTRANCE(WOODFALL_TEMPLE, 1), // From boss room
        },
    };
    Regions[RR_WOODFALL_TEMPLE_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_ENTRANCE_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_SF_ENTRANCE, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_ENTRANCE, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 1),                     ENTRANCE(WOODFALL_TEMPLE, 0), true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM, CAN_BE_DEKU),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER] = RandoRegion{ .name = "Main Room Upper", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_1, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_2, true),
            CHECK(RC_WOODFALL_TEMPLE_SF_MAIN_BUBBLE, true),
            CHECK(RC_WOODFALL_TEMPLE_CENTER_CHEST, CAN_BE_DEKU),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, true),
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM, true),
            CONNECTION(RR_WOODFALL_TEMPLE_PRE_BOSS_ROOM, CHECK_WEEKEVENTREG(WEEKEVENTREG_12_01)),
            CONNECTION(RR_WOODFALL_TEMPLE_DARK_ROOM, true),
        },
        .events = {
            EVENT(
                "Light Corner Torch", 
                Flags_GetSceneSwitch(SCENE_MITURIN, 0x09), 
                Flags_SetSceneSwitch(SCENE_MITURIN, 0x09), 
                Flags_ClearSceneSwitch(SCENE_MITURIN, 0x09), 
                HAS_ITEM(ITEM_BOW) && CHECK_WEEKEVENTREG(WEEKEVENTREG_12_01)
            ),
            EVENT_WEEKEVENTREG("Light Middle Torch", WEEKEVENTREG_12_01, HAS_ITEM(ITEM_BOW)),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_MAIN_ROOM] = RandoRegion{ .name = "Main Room", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_1, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_2, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_3, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_4, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_5, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_6, true),
            CHECK(RC_WOODFALL_TEMPLE_SF_MAIN_POT, true),
            // TODO: CAN_KILL_ENEMY(DEKU_BABA)?
            CHECK(RC_WOODFALL_TEMPLE_SF_MAIN_DEKU_BABA, true),
            CHECK(RC_WOODFALL_TEMPLE_SF_MAIN_BUBBLE, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)) && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_ENTRANCE, true),
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM, true), // It's a little tight for goron but possible
            CONNECTION(RR_WOODFALL_TEMPLE_MAZE_ROOM, KEY_COUNT(WOODFALL_TEMPLE) >= 1),
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, Flags_GetSceneSwitch(SCENE_MITURIN, 0x09)),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_MAP_ROOM] = RandoRegion{ .name = "Map Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(SNAPPER)?
            CHECK(RC_WOODFALL_TEMPLE_MAP, CAN_BE_DEKU || CAN_USE_EXPLOSIVE),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_MAZE_ROOM] = RandoRegion{ .name = "Maze Room", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_POT_MAZE_1, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_MAZE_2, true),
            // TODO: Trick for bombs & chus here
            CHECK(RC_WOODFALL_TEMPLE_SF_MAZE_BEEHIVE, CAN_USE_PROJECTILE),
            // TODO: Maybe add a health check here later
            CHECK(RC_WOODFALL_TEMPLE_SF_MAZE_BUBBLE, true),
            // CAN_KILL_ENEMY(SKULLTULA)
            CHECK(RC_WOODFALL_TEMPLE_SF_MAZE_SKULLTULA, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM, KEY_COUNT(WOODFALL_TEMPLE) >= 1),
            CONNECTION(RR_WOODFALL_TEMPLE_DARK_ROOM, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CONNECTION(RR_WOODFALL_TEMPLE_COMPASS_ROOM, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_PRE_BOSS_ROOM] = RandoRegion{ .name = "Pre Boss Room", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_BOTTOM_RIGHT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_LEFT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_TOP_RIGHT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_POT_PRE_BOSS_1, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_POT_PRE_BOSS_2, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_PILLAR, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_01, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_02, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_03, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_04, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_05, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_PRE_BOSS_FREESTANDING_RUPEE_06, CAN_BE_DEKU && CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ODOLWAS_LAIR, 0),                          ONE_WAY_EXIT, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW) && CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_WOODFALL_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER] = RandoRegion{ .name = "Water Room Upper", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_POT_WATER_ROOM_1, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_WATER_ROOM_2, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_WATER_ROOM_3, true),
            CHECK(RC_WOODFALL_TEMPLE_POT_WATER_ROOM_4, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM, true),
            CONNECTION(RR_WOODFALL_TEMPLE_BOW_ROOM, true),
            CONNECTION(RR_WOODFALL_TEMPLE_BOSS_KEY_ROOM, HAS_ITEM(ITEM_BOW) && CAN_BE_DEKU),
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, true),
        },
    };
    Regions[RR_WOODFALL_TEMPLE_WATER_ROOM] = RandoRegion{ .name = "Water Room", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_WATER_CHEST, CAN_BE_DEKU || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_WOODFALL_TEMPLE_SF_WATER_ROOM_BEEHIVE, (
                // Can they break it, leaving the fairy up high? // TODO: Trick for bombs & chus here
                ((HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA) && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)) ||
                // Can they break it, making it drop into the water?
                (HAS_ITEM(ITEM_BOW) || (CAN_BE_DEKU && HAS_MAGIC))
            )),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM, true),
            CONNECTION(RR_WOODFALL_TEMPLE_MAP_ROOM, CAN_BE_DEKU),
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, HAS_ITEM(ITEM_BOW) && CAN_BE_DEKU),
        },
    };
}, {});
// clang-format on
