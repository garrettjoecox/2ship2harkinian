#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_BENEATH_THE_GRAVEYARD_DAMPE] = RandoRegion{ .sceneId = SCENE_DANPEI2TEST,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_03, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_04, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_05, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_06, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_07, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_08, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_09, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_10, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 4),                       ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(DAMPES_HOUSE, 0), // Day 3 grave
        }
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS] = RandoRegion{ .name = "Night 1 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_TOMBSTONE, CAN_KILL_IRONKNUCKLE),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, CAN_KILL_IRONKNUCKLE),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE] = RandoRegion{ .name = "Night 1 Grave", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_2, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_CHEST, CAN_KILL_BAT),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_2, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 3),              ENTRANCE(BENEATH_THE_GRAVERYARD, 1), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_BOSS] = RandoRegion{ .name = "Night 2 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_HP, CAN_KILL_IRONKNUCKLE),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, true),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT] = RandoRegion{ .name = "Night 2 Grave After Pit", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_AFTER_PIT_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_AFTER_PIT_2, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_AFTER_PIT_3, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_AFTER_PIT_4, true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_BOSS, CAN_USE_EXPLOSIVE),
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_BEFORE_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_BEFORE_PIT] = RandoRegion{ .name = "Night 2 Grave Before Pit", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_03, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_04, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_05, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_06, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_07, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_EARLY, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_BEFORE_PIT_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_BEFORE_PIT_2, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 2),              ENTRANCE(BENEATH_THE_GRAVERYARD, 0), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    };
    Regions[RR_GHOST_HUT] = RandoRegion{ .sceneId = SCENE_TOUGITES,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 1),                 ENTRANCE(GHOST_HUT, 0), true),
        },
    };
    Regions[RR_IKANA_CANYON_CAVE] = RandoRegion{ .name = "Cave", .sceneId = SCENE_IKANA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 13),                ENTRANCE(IKANA_CANYON, 14), true),
        },
        .events = {
            EVENT_WEEKEVENTREG("Break the curse of Sharp, Composer Brother", WEEKEVENTREG_14_04, CAN_PLAY_SONG(STORMS)),
        },
    };
    Regions[RR_IKANA_CANYON_GROTTO] = RandoRegion{ .name = "Ikana Canyon Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_GROTTO, true),
        },
        .connections = {
            CONNECTION(RR_IKANA_CANYON_LOWER, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_IKANA_CANYON_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_SCRUB_HP, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA && CAN_BE_DEKU),
            CHECK(RC_IKANA_CANYON_SCRUB_HUGE_RUPEE, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA),
            CHECK(RC_IKANA_CANYON_SCRUB_POTION_REFILL, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 1),                ENTRANCE(IKANA_CANYON, 0), true), 
            /*              
                TODO : Sakon's Hideout is heavily flag based so we should check for those. Consider what I have in here now to be loose placeholders.
                Must NOT have helped the Bomb Shop old lady on this cycle(Kafei does not show up if you do, as Sakon never visits the shop to be followed.)
                Must have delivered the Letter to Kafei and met Kafei.(Sakon just does not show up otherwise, as odd as that may sound.)
            */
            EXIT(ENTRANCE(SAKONS_HIDEOUT, 0),               ENTRANCE(IKANA_CANYON, 6), Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)),
            EXIT(ENTRANCE(SECRET_SHRINE, 0),                ENTRANCE(IKANA_CANYON, 12), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 9),               ONE_WAY_EXIT, true),
        },
        .connections = {
            CONNECTION(RR_IKANA_CANYON_UPPER, HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_MAGIC_ARROW(ICE)),
            CONNECTION(RR_IKANA_CANYON_GROTTO, true), // TODO: Grotto mapping
        },
        .events = {
            EVENT_ACCESS(RANDO_ACCESS_BLUE_POTION_REFILL, CUR_UPG_VALUE(UPG_WALLET) >= 1),
        },
    };
    Regions[RR_IKANA_CANYON_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_IKANA_CANYON_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_IKANA_CANYON_TINGLE_MAP_2, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GHOST_HUT, 0),                    ENTRANCE(IKANA_CANYON, 1), true),
            EXIT(ENTRANCE(MUSIC_BOX_HOUSE, 0),              ENTRANCE(IKANA_CANYON, 2), CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)),
            EXIT(ENTRANCE(STONE_TOWER, 0),                  ENTRANCE(IKANA_CANYON, 3), true),
            EXIT(ENTRANCE(BENEATH_THE_WELL, 0),             ENTRANCE(IKANA_CANYON, 5), true),
            EXIT(ENTRANCE(IKANA_CASTLE, 1),                 ENTRANCE(IKANA_CANYON, 8), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 4),               ENTRANCE(IKANA_CANYON, 11), true),
            EXIT(ENTRANCE(IKANA_CANYON, 14),                ENTRANCE(IKANA_CANYON, 13), true), // Cave
        },
        .connections = {
            // May consider cutting Deku and Goron from this since getting down as them may be seen as a trick. But its possible and is pretty easy to do.
            CONNECTION(RR_IKANA_CANYON_LOWER, true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_IKANA_CANYON),
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_CANYON, 4), // From Song of Soaring
            ENTRANCE(IKANA_CANYON, 15), // From Stone Tower Temple Blue Warp
        }
    };
    Regions[RR_IKANA_GRAVEYARD_GROTTO] = RandoRegion{ .name = "Ikana Graveyard Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_GROTTO, true),
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_LOWER, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_IKANA_GRAVEYARD_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_BOTI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 2),                ENTRANCE(IKANA_GRAVEYARD, 0), true),
            EXIT(ENTRANCE(DAMPES_HOUSE, 0),                          ONE_WAY_EXIT, HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 3 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 0),       ENTRANCE(IKANA_GRAVEYARD, 2), HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 2 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 1),       ENTRANCE(IKANA_GRAVEYARD, 3), HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 1 hole
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_UPPER, CAN_PLAY_SONG(SONATA)),
            CONNECTION(RR_IKANA_GRAVEYARD_GROTTO, CAN_USE_EXPLOSIVE || CAN_BE_GORON), // TODO: Grotto mapping
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_GRAVEYARD, 4), // Exiting Dampe's house
        }
    };
    Regions[RR_IKANA_GRAVEYARD_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_BOTI,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_CAPTAIN_MASK, true)
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_LOWER, true)
        },
    };
    Regions[RR_IKANA_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Ikana", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_IKANA_GREAT_FAIRY, HAS_ALL_STRAY_FAIRIES(DUNGEON_INDEX_STONE_TOWER_TEMPLE)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 11),                ENTRANCE(FAIRY_FOUNTAIN, 4), true),
        },
    };
    Regions[RR_MUSIC_BOX_HOUSE] = RandoRegion{ .sceneId = SCENE_MUSICHOUSE,
        .checks = {
            CHECK(RC_MUSIC_BOX_HOUSE_FATHER, CAN_PLAY_SONG(HEALING)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 2),                 ENTRANCE(MUSIC_BOX_HOUSE, 0), true),
        },
    };
    Regions[RR_ROAD_TO_IKANA_ABOVE_LEDGE] = RandoRegion{ .name = "Above Ledge", .sceneId = SCENE_IKANAMAE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 0),                 ENTRANCE(ROAD_TO_IKANA, 1), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, true),
        },
    };
    Regions[RR_ROAD_TO_IKANA_BELOW_LEDGE] = RandoRegion{ .name = "Below Ledge", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_POT, CAN_HOOK_SCARECROW),
            // TODO: HAS_ACCESS_TO_POTION_REFILL?
            CHECK(RC_ROAD_TO_IKANA_STONE_MASK, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC /* && (HAS_ITEM(ITEM_POTION_RED) || HAS_ITEM(ITEM_POTION_BLUE)) */),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 0),              ENTRANCE(ROAD_TO_IKANA, 2), true)
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_FIELD_SIDE, CAN_RIDE_EPONA),
            CONNECTION(RR_ROAD_TO_IKANA_ABOVE_LEDGE, HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_MASK_GARO)),
        },
    };
    Regions[RR_ROAD_TO_IKANA_FIELD_SIDE] = RandoRegion{ .name = "Field Side", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 4),                ENTRANCE(ROAD_TO_IKANA, 0), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, CAN_RIDE_EPONA),
            CONNECTION(RR_ROAD_TO_IKANA_GROTTO, CAN_BE_GORON), // TODO: Grotto mapping
        },
    };
    Regions[RR_ROAD_TO_IKANA_GROTTO] = RandoRegion{ .name = "Road to Ikana Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_GROTTO, true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_FIELD_SIDE, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_SAKON_HIDEOUT] = RandoRegion{ .sceneId = SCENE_SECOM,
        .checks = {
            CHECK(RC_SAKON_HIDEOUT_POT_FIRST_ROOM_1,    true),
            CHECK(RC_SAKON_HIDEOUT_POT_FIRST_ROOM_2,    true),
            CHECK(RC_SAKON_HIDEOUT_POT_SECOND_ROOM_1,   true),
            CHECK(RC_SAKON_HIDEOUT_POT_SECOND_ROOM_2,   true),
            CHECK(RC_SAKON_HIDEOUT_POT_THIRD_ROOM,      true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 6),                 ENTRANCE(SAKONS_HIDEOUT, 0), true),
        },
    };
    Regions[RR_SECRET_SHRINE_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_01, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_02, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_03, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_04, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_05, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_06, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_07, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_08, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_09, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_10, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_11, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_12, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_13, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_14, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_15, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_16, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_17, HAS_ITEM(ITEM_MAGIC_BEANS) && CAN_ACCESS(SPRING_WATER)),
            CHECK(RC_SECRET_SHRINE_POT_1, true),
            CHECK(RC_SECRET_SHRINE_POT_2, true),
            CHECK(RC_SECRET_SHRINE_POT_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 12),                ENTRANCE(SECRET_SHRINE, 0), true),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE, HAS_MAGIC && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_ITEM(ITEM_BOW))
        }
    };
    Regions[RR_SECRET_SHRINE] = RandoRegion{ .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_DINALFOS_CHEST, Flags_GetSceneClear(SCENE_RANDOM, 0x02)),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_CHEST, Flags_GetSceneClear(SCENE_RANDOM, 0x03)),
            CHECK(RC_SECRET_SHRINE_WART_CHEST, Flags_GetSceneClear(SCENE_RANDOM, 0x04)),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_CHEST, Flags_GetSceneClear(SCENE_RANDOM, 0x05)),
            CHECK(RC_SECRET_SHRINE_HP_CHEST, Flags_GetSceneClear(SCENE_RANDOM, 0x02) && Flags_GetSceneClear(SCENE_RANDOM, 0x03) && Flags_GetSceneClear(SCENE_RANDOM, 0x04) && Flags_GetSceneClear(SCENE_RANDOM, 0x05)),
            CHECK(RC_SECRET_SHRINE_POT_4, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_5, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_6, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_7, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_8, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_9, CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_ENTRANCE, true),
        },
        .events = {
            // TODO: Allow opting in to health checks
            EVENT("Kill Dinalfos", Flags_GetSceneClear(SCENE_RANDOM, 0x02), Flags_SetSceneClear(SCENE_RANDOM, 0x02), Flags_UnsetSceneClear(SCENE_RANDOM, 0x02), /* CHECK_MAX_HP(4) && */ CAN_KILL_DINALFOS),
            EVENT("Kill Wizzrobe", Flags_GetSceneClear(SCENE_RANDOM, 0x03), Flags_SetSceneClear(SCENE_RANDOM, 0x03), Flags_UnsetSceneClear(SCENE_RANDOM, 0x03), /* CHECK_MAX_HP(8) && */ CAN_KILL_WIZZROBE),
            EVENT("Kill Wart", Flags_GetSceneClear(SCENE_RANDOM, 0x04), Flags_SetSceneClear(SCENE_RANDOM, 0x04), Flags_UnsetSceneClear(SCENE_RANDOM, 0x04), /* CHECK_MAX_HP(12) && */ CAN_KILL_WART),
            EVENT("Kill Garo Master", Flags_GetSceneClear(SCENE_RANDOM, 0x05), Flags_SetSceneClear(SCENE_RANDOM, 0x05), Flags_UnsetSceneClear(SCENE_RANDOM, 0x05), /* CHECK_MAX_HP(16) && */ CAN_KILL_GARO_MASTER),
        },
    };
    Regions[RR_STONE_TOWER_BOTTOM] = RandoRegion{ .name = "Bottom", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_CLIMB_1, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_STONE_TOWER_POT_CLIMB_2, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 3),                 ENTRANCE(STONE_TOWER, 0), true)
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_GORON && CAN_BE_ZORA),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_LOWER] = RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER, 1),                  ENTRANCE(STONE_TOWER_INVERTED, 0), HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_UPPER, CAN_GROW_BEAN_PLANT),
            CONNECTION(RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, true),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_NEAR_TEMPLE] = RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0),  ENTRANCE(STONE_TOWER_INVERTED, 1), (
                // TODO: THIS IS TEMPORARY. Once stone tower is properly split up, this will be replaced with a proper logic check.
                CAN_BE_ZORA && CAN_BE_DEKU && CAN_BE_GORON &&
                HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_HOOKSHOT) && 
                HAS_MAGIC && CAN_LIGHT_TORCH_NEAR_ANOTHER && CAN_USE_SWORD && CAN_USE_EXPLOSIVE && CAN_PLAY_SONG(ELEGY)
                // TODO: We can't really add requirement for key count, as the keys need to be in the pool
                // to be shuffled, and to be in the pool their vanilla location has to be accessible. Once
                // all key locations are logically accessible we can re-add this check.
                /* && KEY_COUNT(STONE_TOWER_TEMPLE) >= 4 */
            )),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_LOWER, true),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_UPPER] = RandoRegion{ .sceneId = SCENE_F41,
        .checks = {
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_1, true),
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_2, true),
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_3, true),
            // These pots don't seem to work, no clue why.
            CHECK(RC_STONE_TOWER_INVERTED_POT_1, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_2, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_3, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_4, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_5, true),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_LOWER, true),
        }
    };
    Regions[RR_STONE_TOWER_MIDDLE] = RandoRegion{ .name = "Middle", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_01, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_02, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_03, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_04, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_05, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_06, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_07, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_08, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_09, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_10, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_11, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_12, CAN_HOOK_SCARECROW),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_BOTTOM, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_GORON && CAN_BE_ZORA),
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
    };
    // TODO: Probably need to split this up to account for entrance rando later
    Regions[RR_STONE_TOWER_TOP] = RandoRegion{ .name = "Top", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_1, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_2, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_3, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 0),         ENTRANCE(STONE_TOWER, 1), CAN_PLAY_SONG(ELEGY) && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE, 0),           ENTRANCE(STONE_TOWER, 2), CAN_BE_ZORA && CAN_BE_GORON && CAN_PLAY_SONG(ELEGY))
            // (
            //     // TODO: THIS IS TEMPORARY. Once stone tower is properly split up, this will be replaced with a proper logic check.
            //     CAN_BE_ZORA && CAN_BE_DEKU && CAN_BE_GORON &&
            //     HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_HOOKSHOT) && 
            //     HAS_MAGIC && CAN_LIGHT_TORCH_NEAR_ANOTHER && CAN_USE_SWORD && CAN_USE_EXPLOSIVE && CAN_PLAY_SONG(ELEGY)
            //     // TODO: We can't really add requirement for key count, as the keys need to be in the pool
            //     // to be shuffled, and to be in the pool their vanilla location has to be accessible. Once
            //     // all key locations are logically accessible we can re-add this check.
            //     /* && KEY_COUNT(STONE_TOWER_TEMPLE) >= 4 */
            // )),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_STONE_TOWER),
        },
        .oneWayEntrances = {
            ENTRANCE(STONE_TOWER, 3), // From Song of Soaring
        }
    };
    Regions[RR_STONE_TOWER_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_1, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_2, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_3, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_4, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_5, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_6, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_7, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_8, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_HIGHER_SCARECROW_9, CAN_HOOK_SCARECROW),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_STONE_TOWER_TOP, HAS_ITEM(ITEM_HOOKSHOT))
        },
    };
}, {});
// clang-format on
