#include "Logic.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
void Flags_ClearRandoInf(s32 flag);
s32 Flags_GetRandoInf(s32 flag);
void Flags_SetWeekEventReg(s32 flag);
void Flags_ClearWeekEventReg(s32 flag);
}

namespace Rando {

namespace Logic {

// TODO: MOVE THIS STUFF OR SOMETHING
#define IS_DEKU (GET_PLAYER_FORM == PLAYER_FORM_DEKU)
#define IS_ZORA (GET_PLAYER_FORM == PLAYER_FORM_ZORA)
#define IS_DEITY (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY)
#define IS_GORON (GET_PLAYER_FORM == PLAYER_FORM_GORON)
#define IS_HUMAN (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)
#define HAS_ITEM(item) (INV_CONTENT(item) == item)
#define CAN_BE_DEKU (IS_DEKU || HAS_ITEM(ITEM_MASK_DEKU))
#define CAN_BE_ZORA (IS_ZORA || HAS_ITEM(ITEM_MASK_ZORA))
#define CAN_BE_DEITY (IS_DEITY || HAS_ITEM(ITEM_MASK_FIERCE_DEITY))
#define CAN_BE_GORON (IS_GORON || HAS_ITEM(ITEM_MASK_GORON))
#define CAN_BE_HUMAN                                                                                        \
    (IS_HUMAN || (IS_DEITY && HAS_ITEM(ITEM_MASK_FIERCE_DEITY)) || (IS_ZORA && HAS_ITEM(ITEM_MASK_ZORA)) || \
     (IS_DEKU && HAS_ITEM(ITEM_MASK_DEKU)) || (IS_GORON && HAS_ITEM(ITEM_MASK_GORON)))
#define CHECK_MAX_HP(TARGET_HP) ((TARGET_HP * 16) <= gSaveContext.save.saveInfo.playerData.healthCapacity)
#define HAS_MAGIC (gSaveContext.save.saveInfo.playerData.isMagicAcquired)
#define CAN_HOOK_SCARECROW (CAN_BE_HUMAN && HAS_ITEM(ITEM_OCARINA_OF_TIME) && HAS_ITEM(ITEM_HOOKSHOT))
#define CAN_USE_EXPLOSIVE (CAN_BE_HUMAN && (HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU) || HAS_ITEM(ITEM_MASK_BLAST)))
#define CAN_USE_SWORD                                                                                               \
    ((CAN_BE_HUMAN &&                                                                                               \
      ((GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI) || HAS_ITEM(ITEM_SWORD_GREAT_FAIRY))) || \
     CAN_BE_DEITY)
// Be careful here, as some checks require you to play the song as a specific form
#define CAN_PLAY_SONG(song)                                                   \
    (HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_##song) && \
     (CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA || CAN_BE_GORON))
#define CAN_RIDE_EPONA (CAN_BE_HUMAN && HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA))
#define ONE_WAY_EXIT -1
#define CAN_OWL_WARP(owlId) ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> owlId) & 1)
#define SET_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags |= (1 << owlId))
#define CLEAR_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags &= ~(1 << owlId))
#define HAS_BOTTLE_ITEM(item) (Inventory_HasItemInBottle(item))
// TODO: Maybe not reliable because of theif bird stealing bottle
#define HAS_BOTTLE (INV_CONTENT(ITEM_BOTTLE) != ITEM_NONE)
#define CAN_GET_SPRING_WATER                                                                  \
    (CAN_BE_HUMAN && HAS_BOTTLE && Flags_GetRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER) || \
     Flags_GetRandoInf(RANDO_INF_HAS_ACCESS_TO_HOT_SPRING_WATER))
#define CAN_GROW_BEAN_PLANT \
    (CAN_BE_HUMAN && HAS_ITEM(ITEM_MAGIC_BEANS) && (CAN_PLAY_SONG(STORMS) || CAN_GET_SPRING_WATER))
// TODO: Move these into a seperate file later?
// After thinking about it I decided to cut explosives or "technically possible but annoying" methods from these.
#define CAN_KILL_DINALFOS (CAN_USE_SWORD || CAN_BE_GORON)
#define CAN_KILL_WIZZROBE \
    ((CAN_BE_HUMAN && (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT))) || CAN_USE_SWORD || CAN_BE_GORON)
#define CAN_KILL_WART ((CAN_BE_HUMAN && (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT))) || CAN_BE_ZORA)
#define CAN_KILL_GARO_MASTER ((CAN_BE_HUMAN && HAS_ITEM(ITEM_BOW)) || CAN_BE_GORON || CAN_USE_SWORD)
#define CAN_KILL_IRONKNUCKLE (CAN_USE_SWORD || CAN_BE_GORON)

// TODO: MOVE THIS STUFF OR SOMETHING
void Flags_SetSceneSwitch(s32 scene, s32 flag) {
    gSaveContext.cycleSceneFlags[scene].switch0 |= (1 << flag);
}
void Flags_ClearSceneSwitch(s32 scene, s32 flag) {
    gSaveContext.cycleSceneFlags[scene].switch0 &= ~(1 << flag);
}
bool Flags_GetSceneSwitch(s32 scene, s32 flag) {
    if (gPlayState != NULL && gPlayState->sceneId == scene) {
        return (gPlayState->actorCtx.sceneFlags.switches[0] >> flag) & 1;
    }

    return (gSaveContext.cycleSceneFlags[scene].switch0 >> flag) & 1;
}

std::string LogicString(std::string condition) {
    if (condition == "true")
        return "";

    return condition;
}

#define EVENT(onApply, onRemove, condition)                                                                \
    {                                                                                                      \
        [] { return onApply; }, [] { return onRemove; }, [] { return condition; }, LogicString(#condition) \
    }
#define EXIT(toEntrance, fromEntrance, condition)                           \
    {                                                                       \
        toEntrance, {                                                       \
            fromEntrance, [] { return condition; }, LogicString(#condition) \
        }                                                                   \
    }
#define CONNECTION(region, condition)                         \
    {                                                         \
        region, {                                             \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }
#define CHECK(check, condition)                               \
    {                                                         \
        check, {                                              \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }

// clang-format off
std::unordered_map<RandoRegionId, RandoRegion> Regions = {
    { RR_ASTRAL_OBSERVATORY_OUTSIDE, RandoRegion{ .name = "Outside Astral Observatory", .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1),       ENTRANCE(TERMINA_FIELD, 9), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_GROW_BEAN_PLANT)
        },
    } },
    { RR_ASTRAL_OBSERVATORY_PASSAGE, RandoRegion{ .name = "Passage", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, CAN_BE_HUMAN && CAN_USE_EXPLOSIVE),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_3, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_4, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2),          ENTRANCE(ASTRAL_OBSERVATORY, 0), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY, (CAN_BE_DEKU && HAS_MAGIC) || (CAN_BE_HUMAN && (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT))) || CAN_BE_ZORA),
        },
    } },
    { RR_ASTRAL_OBSERVATORY, RandoRegion{ .name = "Inside Astral Observatory", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_3, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 9),            ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, (CAN_BE_DEKU && HAS_MAGIC) || (CAN_BE_HUMAN && (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT))) || CAN_BE_ZORA), // TODO: Trick to backflip over balloon
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_DAMPE, RandoRegion{ .sceneId = SCENE_DANPEI2TEST,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST, CAN_BE_HUMAN && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_2, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_3, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_4, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_5, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_6, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_7, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_8, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_9, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_10, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 4),                   ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(DAMPES_HOUSE, 0), // Day 3 grave
        }
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS, RandoRegion{ .name = "Night 1 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            // TODO: Song of storms
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, RandoRegion{ .name = "Night 1 Grave", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_2, true),
            // TODO: CAN_KILL_ENEMY(BAT)?
            CHECK(RC_BENEATH_THE_GRAVEYARD_CHEST, CAN_USE_SWORD || (CAN_BE_HUMAN && (HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_BOW) || CAN_USE_EXPLOSIVE)) || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_2, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_3, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 3),    ENTRANCE(BENEATH_THE_GRAVERYARD, 1), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS, (CAN_BE_HUMAN && ((HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE) && HAS_MAGIC) || HAS_ITEM(ITEM_DEKU_STICK)))),
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_2_BOSS, RandoRegion{ .name = "Night 2 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_HP, CAN_KILL_IRONKNUCKLE),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, true),
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, RandoRegion{ .name = "Night 2 Grave After Pit", .sceneId = SCENE_HAKASHITA,
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
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_BEFORE_PIT, RandoRegion{ .name = "Night 2 Grave Before Pit", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_EARLY, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_BEFORE_PIT_1, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_BEFORE_PIT_2, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 2),          ENTRANCE(BENEATH_THE_GRAVERYARD, 0), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    } },
    { RR_BENEATH_THE_WELL, RandoRegion{ .sceneId = SCENE_REDEAD,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 5),             ENTRANCE(BENEATH_THE_WELL, 0), true),
        },
    } },
    { RR_BOMB_SHOP, RandoRegion{ .sceneId = SCENE_BOMYA,
        .checks = {
            // Attempting to buy anything as Deity softlocks the game
            CHECK(RC_BOMB_SHOP_ITEM_1, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
            CHECK(RC_BOMB_SHOP_ITEM_2, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
            CHECK(RC_BOMB_SHOP_ITEM_3, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
            // TODO: Bigger bomb bag
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 6),          ENTRANCE(BOMB_SHOP, 0), true),
        },
    } },
    { RR_CLOCK_TOWER_INTERIOR, RandoRegion{ .sceneId = SCENE_INSIDETOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),         ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
        },
    } },
    { RR_CLOCK_TOWER_ROOF, RandoRegion{ .sceneId = SCENE_OKUJOU,
        .checks = {
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, HAS_MAGIC && CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, HAS_MAGIC && CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_1, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_2, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_3, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_4, true),
        },
        .oneWayEntrances = {
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
    } },
    { RR_CLOCK_TOWN_EAST, RandoRegion{ .sceneId = SCENE_TOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, CAN_BE_DEKU), // Same check in two places, is this okay?
            CHECK(RC_CLOCK_TOWN_EAST_UPPER_CHEST, CAN_BE_ZORA || CAN_BE_HUMAN || CAN_BE_DEITY),
        },
        .exits = { //     TO                                     FROM
            // FD gets stuck when entering Astral Obervatory from here.
            EXIT(ENTRANCE(TERMINA_FIELD, 7),            ENTRANCE(EAST_CLOCK_TOWN, 0), CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7),         ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0),       ENTRANCE(EAST_CLOCK_TOWN, 2), CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA || CAN_BE_GORON), // TODO: Bombers Code req
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2),         ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0),      ENTRANCE(EAST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1),         ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),  ENTRANCE(EAST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0),         ENTRANCE(EAST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0),    ENTRANCE(EAST_CLOCK_TOWN, 8), true),
            EXIT(ENTRANCE(STOCK_POT_INN, 0),            ENTRANCE(EAST_CLOCK_TOWN, 9), true), // To lobby
            EXIT(ENTRANCE(STOCK_POT_INN, 1),            ENTRANCE(EAST_CLOCK_TOWN, 10), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0),                 ENTRANCE(EAST_CLOCK_TOWN, 11), HAS_ITEM(ITEM_MASK_ROMANI)),
        },
    } },
    { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Clock Town", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3),         ENTRANCE(FAIRY_FOUNTAIN, 0), true),
        },
    } },
    { RR_CLOCK_TOWN_LAUNDRY, RandoRegion{ .sceneId = SCENE_ALLEY,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6),         ENTRANCE(LAUNDRY_POOL, 0), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 1),           ENTRANCE(LAUNDRY_POOL, 1), Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI))
        },
    } },
    { RR_CLOCK_TOWN_NORTH, RandoRegion{ .sceneId = SCENE_BACKTOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_HP, CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 8),            ENTRANCE(NORTH_CLOCK_TOWN, 0), CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5),          ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4),         ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0),           ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0),    ENTRANCE(NORTH_CLOCK_TOWN, 4), CAN_BE_DEKU),
        },
    } },
    { RR_CLOCK_TOWN_SOUTH_PLATFORM, RandoRegion{ .name = "Clock Tower Platform", .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0),               ONE_WAY_EXIT, true),
        },
        .connections = {
            CONNECTION(RR_CLOCK_TOWN_SOUTH, true),
        },
    } },
    { RR_CLOCK_TOWN_SOUTH, RandoRegion{ .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || (CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT))),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) && (CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY)) || (CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT))),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1),     ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6),            ENTRANCE(SOUTH_CLOCK_TOWN, 1), CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3),          ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2),          ENTRANCE(SOUTH_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2),         ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1),          ENTRANCE(SOUTH_CLOCK_TOWN, 5), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0),             ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1),          ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
        },
        .connections = {
            CONNECTION(RR_CLOCK_TOWN_SOUTH_PLATFORM, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR))),
            CONNECTION(RR_MAX, true),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_CLOCK_TOWN), CLEAR_OWL_WARP(OWL_WARP_CLOCK_TOWN), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTH_CLOCK_TOWN, 9), // From Song of Soaring
        }
    } },
    { RR_CLOCK_TOWN_WEST, RandoRegion{ .sceneId = SCENE_ICHIBA,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET, true),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_HP, true),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_INTEREST, true),
            CHECK(RC_CLOCK_TOWN_WEST_SISTERS_HP, CAN_BE_HUMAN && HAS_ITEM(ITEM_MASK_KAMARO)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 0),            ENTRANCE(WEST_CLOCK_TOWN, 0), CAN_BE_DEITY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 5),         ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 3),         ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(SWORDMANS_SCHOOL, 0),         ENTRANCE(WEST_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 0),           ENTRANCE(WEST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(TRADING_POST, 0),             ENTRANCE(WEST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(BOMB_SHOP, 0),                ENTRANCE(WEST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(POST_OFFICE, 0),              ENTRANCE(WEST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(LOTTERY_SHOP, 0),             ENTRANCE(WEST_CLOCK_TOWN, 8), true),
        },
    } },
    { RR_CUCCO_SHACK, RandoRegion{ .sceneId = SCENE_F01C,
        .checks = {
            CHECK(RC_ROMANI_RANCH_GROG, CAN_BE_HUMAN && HAS_ITEM(ITEM_MASK_BREMEN)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 4),             ENTRANCE(CUCCO_SHACK, 0), true),
        },
    } },
    { RR_CURIOSITY_SHOP_BACK, RandoRegion{ .name = "Back", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add Keaton Mask/Express Letter to Mama checks
            CHECK(RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES, CAN_BE_HUMAN && Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(LAUNDRY_POOL, 1),             ENTRANCE(CURIOSITY_SHOP, 1), true)
        },
    } },
    { RR_CURIOSITY_SHOP_FRONT, RandoRegion{ .name = "Front", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add the shop checks
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 4),          ENTRANCE(CURIOSITY_SHOP, 0), true)
        },
    } },
    { RR_DEKU_KINGS_CHAMBER_HOLDING_CELL, RandoRegion{ .name = "Holding Cell", .sceneId = SCENE_DEKU_KING,
        .checks = {
            CHECK(RC_DEKU_KINGS_CHAMBER_MONKEY, CAN_BE_DEKU && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(DEKU_PALACE, 3),              ENTRANCE(DEKU_KINGS_CHAMBER, 1), true),
        },
    } },
    { RR_DEKU_KINGS_CHAMBER, RandoRegion{ .sceneId = SCENE_DEKU_KING,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(DEKU_PALACE, 2),              ENTRANCE(DEKU_KINGS_CHAMBER, 0), true),
        },
    } },
    { RR_DEKU_PALACE_INSIDE, RandoRegion{ .name = "Inside", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_DEKU_PALACE_HP, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 0),       ENTRANCE(DEKU_PALACE, 2), true),
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 1),       ENTRANCE(DEKU_PALACE, 3), CAN_BE_DEKU), // Cell TODO: Is there something to do with beans here?
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_OUTSIDE, true),
        },
    } },
    { RR_DEKU_PALACE_OUTSIDE, RandoRegion{ .name = "Outside", .sceneId = SCENE_22DEKUCITY,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 3),  ENTRANCE(DEKU_PALACE, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 4),  ENTRANCE(DEKU_PALACE, 5), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(DEKU_SHRINE, 0),              ENTRANCE(DEKU_PALACE, 4), CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE, CAN_BE_DEKU),
        },
    } },
    { RR_DEKU_SHRINE, RandoRegion{ .sceneId = SCENE_DANPEI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(DEKU_PALACE, 4),              ENTRANCE(DEKU_SHRINE, 0), true),
        },
    } },
    { RR_DOGGY_RACETRACK, RandoRegion{ .sceneId = SCENE_F01_B,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 5),             ENTRANCE(DOGGY_RACETRACK, 0), true),
        },
    } },
    { RR_FISHERMANS_HUT, RandoRegion{ .sceneId = SCENE_FISHERMAN,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 4),             ENTRANCE(FISHERMANS_HUT, 0), true),
        },
    } },
    { RR_GHOST_HUT, RandoRegion{ .sceneId = SCENE_TOUGITES,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 1),             ENTRANCE(GHOST_HUT, 0), true),
        },
    } },
    { RR_GORMAN_TRACK, RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MILK_ROAD, 2),                ENTRANCE(GORMAN_TRACK, 3), HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && CAN_BE_HUMAN),
            EXIT(ENTRANCE(MILK_ROAD, 3),                ENTRANCE(GORMAN_TRACK, 0), true),
        },
    } },
    { RR_GORON_GRAVEYARD, RandoRegion{ .sceneId = SCENE_GORON_HAKA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3),  ENTRANCE(GORON_GRAVERYARD, 0), true),
        },
    } },
    { RR_GREATBAY_COAST_OUTSIDE_LAB, RandoRegion{ .name = "Outside Lab", .sceneId = SCENE_30GYOSON,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_POT_9,         CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_10,        CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_11,        CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_12,        CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MARINE_RESEARCH_LAB, 0),      ENTRANCE(GREAT_BAY_COAST, 7), true),
        },
        .connections = {
            CONNECTION(RR_GREATBAY_COAST, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_GREAT_BAY_COAST), CLEAR_OWL_WARP(OWL_WARP_GREAT_BAY_COAST), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(GREAT_BAY_COAST, 11), // From Song of Soaring
        },
    } },
    { RR_GREATBAY_COAST_OUTSIDE_PIRATES_FORTRESS, RandoRegion{ .name = "Outside Pirate's Fortress", .sceneId = SCENE_30GYOSON,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_POT_1, true),
            CHECK(RC_GREAT_BAY_COAST_POT_2, true),
        },
        .connections = {
            CONNECTION(RR_GREATBAY_COAST_PIRATES_COVE, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
        .oneWayEntrances = {
            ENTRANCE(GREAT_BAY_COAST, 12), // From being captured in Pirate Fortress Moat
        },
    } },
    { RR_GREATBAY_COAST_PIRATES_COVE, RandoRegion{ .name = "Pirate's Cove", .sceneId = SCENE_30GYOSON,
        .checks = {
            // TODO: CAN_PLAY_SCARECROW_SONG?
            CHECK(RC_GREAT_BAY_COAST_HP,            CAN_HOOK_SCARECROW && CAN_GROW_BEAN_PLANT),
            CHECK(RC_GREAT_BAY_COAST_POT_5,         CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_6,         CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_7,         CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_8,         CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_1,   HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_HUMAN),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_2,   HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_HUMAN),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_3,   HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_HUMAN),
        },
        .connections = {
            CONNECTION(RR_GREATBAY_COAST, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CONNECTION(RR_GREATBAY_COAST_OUTSIDE_PIRATES_FORTRESS, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
    } },
    { RR_GREATBAY_COAST, RandoRegion{ .sceneId = SCENE_30GYOSON,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_MIKAU,         CAN_BE_HUMAN && CHECK_QUEST_ITEM(QUEST_SONG_HEALING) && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_GREAT_BAY_COAST_POT_3,         true),
            CHECK(RC_GREAT_BAY_COAST_POT_4,         true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 2),            ENTRANCE(GREAT_BAY_COAST, 0), true),
            EXIT(ENTRANCE(ZORA_CAPE, 0),                ENTRANCE(GREAT_BAY_COAST, 1), true),
            EXIT(ENTRANCE(PINNACLE_ROCK, 0),            ENTRANCE(GREAT_BAY_COAST, 3), CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            EXIT(ENTRANCE(FISHERMANS_HUT, 0),           ENTRANCE(GREAT_BAY_COAST, 4), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 0),ENTRANCE(GREAT_BAY_COAST, 5), CAN_BE_ZORA),
            EXIT(ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0),   ENTRANCE(GREAT_BAY_COAST, 8), true),
        },
        .connections = {
            CONNECTION(RR_GREATBAY_COAST_PIRATES_COVE, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CONNECTION(RR_GREATBAY_COAST_OUTSIDE_LAB, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        }
    } },
    { RR_GREATBAY_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_CAPE, 5),                 ENTRANCE(FAIRY_FOUNTAIN, 3), true),
        },
    } },
    { RR_HONEY_AND_DARLING, RandoRegion{ .sceneId = SCENE_BOWLING,
        .checks = {
            // TODO : Add checks for all 3 days for this shop.
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 6),          ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), true),
        },
    } },
    { RR_IKANA_CANYON_CAVE, RandoRegion{ .name = "Cave", .sceneId = SCENE_IKANA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 13),            ENTRANCE(IKANA_CANYON, 14), true),
        },
        .events = {
            EVENT(SET_WEEKEVENTREG(WEEKEVENTREG_14_04), CLEAR_WEEKEVENTREG(WEEKEVENTREG_14_04), CAN_PLAY_SONG(STORMS))
        },
    } },
    { RR_IKANA_CANYON_LOWER, RandoRegion{ .name = "Lower", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_SCRUB_HP, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA && CAN_BE_DEKU),
            CHECK(RC_IKANA_CANYON_SCRUB_HUGE_RUPEE, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA),
            CHECK(RC_IKANA_CANYON_SCRUB_POTION_REFILL, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_DEITY),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 1),            ENTRANCE(IKANA_CANYON, 0), true), 
            /*              
                TODO : Sakon's Hideout is heavily flag based so we should check for those. Consider what I have in here now to be loose placeholders.
                Must NOT have helped the Bomb Shop old lady on this cycle(Kafei does not show up if you do, as Sakon never visits the shop to be followed.)
                Must have delivered the Letter to Kafei and met Kafei.(Sakon just does not show up otherwise, as odd as that may sound.)
            */
            EXIT(ENTRANCE(SAKONS_HIDEOUT, 0),           ENTRANCE(IKANA_CANYON, 6), CAN_BE_HUMAN && Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 9),  ONE_WAY_EXIT, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA || CAN_BE_DEITY),
        },
        .connections = {
            CONNECTION(RR_IKANA_CANYON_UNDER_WATERFALL, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
            CONNECTION(RR_IKANA_CANYON_UPPER, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_BOW) && HAS_MAGIC && HAS_ITEM(ITEM_ARROW_ICE))
        },
    } },
    { RR_IKANA_CANYON_UNDER_WATERFALL, RandoRegion{ .name = "Waterfall", .sceneId = SCENE_IKANA,
        .checks = {
            // TODO: Grottos
            CHECK(RC_IKANA_CANYON_GROTTO, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SECRET_SHRINE, 0),            ENTRANCE(IKANA_CANYON, 12), true),
        },
        .connections = {
            CONNECTION(RR_IKANA_CANYON_LOWER, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY)
        },
    } },
    { RR_IKANA_CANYON_UPPER, RandoRegion{ .name = "Upper", .sceneId = SCENE_IKANA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GHOST_HUT, 0),                      ENTRANCE(IKANA_CANYON, 1), true),
            EXIT(ENTRANCE(MUSIC_BOX_HOUSE, 0),                ENTRANCE(IKANA_CANYON, 2), CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)),
            EXIT(ENTRANCE(STONE_TOWER, 0),                    ENTRANCE(IKANA_CANYON, 3), true),
            EXIT(ENTRANCE(BENEATH_THE_WELL, 0),               ENTRANCE(IKANA_CANYON, 5), true),
            EXIT(ENTRANCE(IKANA_CASTLE, 1),                   ENTRANCE(IKANA_CANYON, 8), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 4),                 ENTRANCE(IKANA_CANYON, 11), true),
            EXIT(ENTRANCE(IKANA_CANYON, 14),                  ENTRANCE(IKANA_CANYON, 13), true),
        },
        .connections = {
            // May consider cutting Deku and Goron from this since getting down as them may be seen as a trick. But its possible and is pretty easy to do.
            CONNECTION(RR_IKANA_CANYON_LOWER, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA || CAN_BE_DEITY || (CAN_BE_GORON && HAS_MAGIC)),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_IKANA_CANYON), CLEAR_OWL_WARP(OWL_WARP_IKANA_CANYON), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_CANYON, 4), // From Song of Soaring
            // TODO: Do we need one way entrance for blue warp from Stone Tower Temple?
        }
    } },
    { RR_IKANA_CASTLE, RandoRegion{ .sceneId = SCENE_CASTLE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 8),             ENTRANCE(IKANA_CASTLE, 1), true),
        },
    } },
    { RR_IKANA_GRAVEYARD_LOWER, RandoRegion{ .name = "Lower", .sceneId = SCENE_BOTI,
        .checks = {
            // TODO : Grotto
            CHECK(RC_IKANA_GRAVEYARD_GROTTO, (CAN_BE_HUMAN && CAN_USE_EXPLOSIVE) || CAN_BE_GORON)
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 2),             ENTRANCE(IKANA_GRAVEYARD, 0), true),
            EXIT(ENTRANCE(DAMPES_HOUSE, 0),              ONE_WAY_EXIT, HAS_ITEM(ITEM_MASK_CAPTAIN) && CAN_BE_HUMAN), // Day 3 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 0),    ENTRANCE(IKANA_GRAVEYARD, 2), HAS_ITEM(ITEM_MASK_CAPTAIN) && CAN_BE_HUMAN), // Day 2 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 1),    ENTRANCE(IKANA_GRAVEYARD, 3), HAS_ITEM(ITEM_MASK_CAPTAIN) && CAN_BE_HUMAN), // Day 1 hole
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_UPPER, CAN_PLAY_SONG(SONATA)),
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_GRAVEYARD, 4), // Exiting Dampe's house
        }
    } },
    { RR_IKANA_GRAVEYARD_UPPER, RandoRegion{ .name = "Upper", .sceneId = SCENE_BOTI,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_CAPTAIN_MASK, true)
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_LOWER, true)
        },
    } },
    { RR_IKANA_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Ikana", .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 11),            ENTRANCE(FAIRY_FOUNTAIN, 4), true),
        },
    } },
    { RR_INN, RandoRegion{ .sceneId = SCENE_YADOYA,
        .checks = {
            // TODO : Add Couples Mask check here.
            CHECK(RC_STOCK_POT_INN_GRANDMA_LONG_STORY, IS_HUMAN && HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GRANDMA_SHORT_STORY, IS_HUMAN && HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GUEST_ROOM_CHEST, (CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA) && Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            CHECK(RC_STOCK_POT_INN_LETTER_TO_KAFEI, CAN_BE_HUMAN && HAS_ITEM(ITEM_MASK_KAFEIS_MASK)),
            CHECK(RC_STOCK_POT_INN_ROOM_KEY, CAN_BE_HUMAN || CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEITY),
            CHECK(RC_STOCK_POT_INN_STAFF_ROOM_CHEST, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
            CHECK(RC_STOCK_POT_INN_TOILET_HAND, 
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) || Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)
            ),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 9),          ENTRANCE(STOCK_POT_INN, 0), CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA), // From ground floor
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 10),         ENTRANCE(STOCK_POT_INN, 1), CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA), // From upstairs
        },
    } },
    { RR_LOTTERY_SHOP, RandoRegion{ .sceneId = SCENE_TAKARAKUJI,
        .checks = {
            // Are we adding the lotter reward as a check?
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 8),          ENTRANCE(LOTTERY_SHOP, 0), true),
        },
    } },
    { RR_MAGIC_HAGS_POTION_SHOP, RandoRegion{ .sceneId = SCENE_WITCH_SHOP,
        .checks = {
            // TODO: Shop prices vs adult wallet?
            CHECK(RC_HAGS_POTION_SHOP_ITEM_1, true),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_2, true),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_3, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 5),  ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0), true),
        },
    } },
    { RR_MILK_BAR, RandoRegion{ .sceneId = SCENE_MILK_BAR,
        .checks = {
            // TODO : Add shop checks/Circus Leader's Mask checks.
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 11),         ENTRANCE(MILK_BAR, 0), true),
        },
    } },
    { RR_MILK_ROAD, RandoRegion{ .sceneId = SCENE_ROMANYMAE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 5),            ENTRANCE(MILK_ROAD, 0), true),
            EXIT(ENTRANCE(ROMANI_RANCH, 0),             ENTRANCE(MILK_ROAD, 1), true),
            EXIT(ENTRANCE(GORMAN_TRACK, 3),             ENTRANCE(MILK_ROAD, 2), HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && CAN_BE_HUMAN),
            EXIT(ENTRANCE(GORMAN_TRACK, 0),             ENTRANCE(MILK_ROAD, 3), true),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_MILK_ROAD), CLEAR_OWL_WARP(OWL_WARP_MILK_ROAD), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(MILK_ROAD, 4), // From Song of Soaring
        }
    } },
    { RR_MARINE_RESEARCH_LAB, RandoRegion{ .sceneId = SCENE_LABO,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 7),          ENTRANCE(MARINE_RESEARCH_LAB, 0), true),
        },
    } },
    { RR_MAYOR_RESIDENCE, RandoRegion{ .sceneId = SCENE_SONCHONOIE,
        .checks = {
            CHECK(RC_MAYORS_OFFICE_HP, CAN_BE_HUMAN && HAS_ITEM(ITEM_MASK_COUPLE)),
            CHECK(RC_MAYORS_OFFICE_KAFEIS_MASK, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON)
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 7),          ENTRANCE(MAYORS_RESIDENCE, 0), true),
        },
    } },
    { RR_MOUNTAIN_SMITHY, RandoRegion{ .sceneId = SCENE_KAJIYA,
        .checks = {
            CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD, GET_CUR_UPG_VALUE(UPG_WALLET) >= 1),
            // TODO: Access to gold dust
            // CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD, HAS_ITEM(ITEM_GOLD_DUST) && RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD].obtained)
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1),  ENTRANCE(MOUNTAIN_SMITHY, 0), true),
        },
    } },
    { RR_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_10YUKIYAMANOMURA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MOUNTAIN_SMITHY, 0),          ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1), true),
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 2), true),
            // TODO: When it's spring you need goron mask or zora mask instead?
            EXIT(ENTRANCE(GORON_GRAVERYARD, 0),         ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3), HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 0),         ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4), true),
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1), ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6), true),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE), CLEAR_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8), // From Song of Soaring
        }
    } },
    { RR_MUSIC_BOX_HOUSE, RandoRegion{ .sceneId = SCENE_MUSICHOUSE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 2),                 ENTRANCE(MUSIC_BOX_HOUSE, 0), true),
        },
    } },
    { RR_PATH_TO_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_13HUBUKINOMITI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 3),            ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0), true),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6),  ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1), true),
        },
    } },
    { RR_PATH_TO_SNOWHEAD_LOWER, RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4),  ENTRANCE(PATH_TO_SNOWHEAD, 0), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_MIDDLE, CAN_BE_GORON),
        },
    } },
    { RR_PATH_TO_SNOWHEAD_MIDDLE, RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_LOWER, CAN_BE_GORON),
            CONNECTION(RR_PATH_TO_SNOWHEAD_UPPER, CAN_BE_GORON),
        },
    } },
    { RR_PATH_TO_SNOWHEAD_UPPER, RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SNOWHEAD, 0),                 ENTRANCE(PATH_TO_SNOWHEAD, 1), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_MIDDLE, CAN_BE_GORON),
        },
    } },
    { RR_PINNACLE_ROCK, RandoRegion{ .sceneId = SCENE_SINKAI,
        // Maybe we split this up into two areas, one requiring sea horse to get to the other. 
        // (Without seahorse should be considered a trick)
        .checks = {
            CHECK(RC_PINNACLE_ROCK_CHEST_1,     CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_CHEST_2,     CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_1,       CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_2,       CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_3,       CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_4,       CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_5,       CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_6,       CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_7,       CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_8,       CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_9,       CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_10,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_11,      CAN_BE_ZORA),
            // TODO: Missing HP check to add here later.
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 3),          ENTRANCE(PINNACLE_ROCK, 0), CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
    } },
    { RR_POST_OFFICE, RandoRegion{ .sceneId = SCENE_POSTHOUSE,
        .checks = {
            // TODO : Add Heartpiece check
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 7),          ENTRANCE(POST_OFFICE, 0), true),
        },
    } },
    { RR_RANCH_BARN, RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 2),             ENTRANCE(RANCH_HOUSE, 0), true),
        },
    } },
    { RR_RANCH_HOUSE, RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 3),             ENTRANCE(RANCH_HOUSE, 1), true),
        },
    } },
    { RR_ROAD_TO_IKANA_ABOVE_LEDGE, RandoRegion{ .name = "Above Ledge", .sceneId = SCENE_IKANAMAE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 0),             ENTRANCE(ROAD_TO_IKANA, 1), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, true),
        },
    } },
    { RR_ROAD_TO_IKANA_BELOW_LEDGE, RandoRegion{ .name = "Below Ledge", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_POT, CAN_HOOK_SCARECROW),
            // TODO: HAS_ACCESS_TO_POTION_REFILL?
            CHECK(RC_ROAD_TO_IKANA_STONE_MASK, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC /* && (HAS_ITEM(ITEM_POTION_RED) || HAS_ITEM(ITEM_POTION_BLUE)) */),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 0),          ENTRANCE(ROAD_TO_IKANA, 2), CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY)
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_FIELD_SIDE, CAN_RIDE_EPONA),
            CONNECTION(RR_ROAD_TO_IKANA_ABOVE_LEDGE, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_MASK_GARO)),
        },
    } },
    { RR_ROAD_TO_IKANA_FIELD_SIDE, RandoRegion{ .name = "Field Side", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_CHEST, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ROAD_TO_IKANA_GROTTO, CAN_BE_GORON), // TODO: Grotto
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 4),            ENTRANCE(ROAD_TO_IKANA, 0), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, CAN_RIDE_EPONA),
        },
    } },
    { RR_ROAD_TO_SOUTHERN_SWAMP, RandoRegion{ .sceneId = SCENE_24KEMONOMITI,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_HP, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 1),            ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 0),  ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1), true),
            EXIT(ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),   ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2), true),
        },
        .events = {
            EVENT(Flags_SetRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER), Flags_ClearRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER), true),
        },
    } },
    { RR_ROMANI_RANCH, RandoRegion{ .sceneId = SCENE_F01,
        .checks = {
            // TODO:
            // CHECK(RC_ROMANI_RANCH_EPONAS_SONG, CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG) && CAN_BE_HUMAN && HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MILK_ROAD, 1),                ENTRANCE(ROMANI_RANCH, 0), true),
            EXIT(ENTRANCE(RANCH_HOUSE, 0),              ENTRANCE(ROMANI_RANCH, 2), true), // Barn
            EXIT(ENTRANCE(RANCH_HOUSE, 1),              ENTRANCE(ROMANI_RANCH, 3), true), // House
            EXIT(ENTRANCE(CUCCO_SHACK, 0),              ENTRANCE(ROMANI_RANCH, 4), true),
            EXIT(ENTRANCE(DOGGY_RACETRACK, 0),          ENTRANCE(ROMANI_RANCH, 5), true),
        },
    } },
    { RR_SAKON_HIDEOUT, RandoRegion{ .sceneId = SCENE_SECOM,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 6),             ENTRANCE(SAKONS_HIDEOUT, 0), true),
        },
    } },
    { RR_SECRET_SHRINE_12_HEART, RandoRegion{ .name = "12 Heart Room",   .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_WART_CHEST, CAN_KILL_WART),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_MAIN_CENTER, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),
        }
    } },
    { RR_SECRET_SHRINE_16_HEART, RandoRegion{ .name = "16 Heart Room",   .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_CHEST, CAN_KILL_GARO_MASTER),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_MAIN_CENTER, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),
        }
    } },
    { RR_SECRET_SHRINE_4_HEART, RandoRegion{ .name = "4 Heart Room",  .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_DINALFOS_CHEST, CAN_KILL_DINALFOS),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_MAIN_CENTER, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),
        }
    } },
    { RR_SECRET_SHRINE_8_HEART, RandoRegion{ .name = "8 Heart Room",  .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_WIZZROBE_CHEST, CAN_KILL_WIZZROBE),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_MAIN_CENTER, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),
        }
    } },
    { RR_SECRET_SHRINE_MAIN_CENTER, RandoRegion{ .name = "Center Room", .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_HP_CHEST, CAN_KILL_DINALFOS && CAN_KILL_WIZZROBE && CAN_KILL_WART && CAN_KILL_GARO_MASTER),
            CHECK(RC_SECRET_SHRINE_POT_4, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_5, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_6, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_7, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_8, CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_9, CAN_BE_ZORA),
        },
        .connections = {
            // The CHECK_MAX_HP macro sometimes causes seed generaiton fails, I believe this is due to not enough HP being in the pool
            // For now ill just comment out the macro call. Have confirmed lowering the checked count does get around generation however.
            CONNECTION(RR_SECRET_SHRINE_12_HEART, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),// && CHECK_MAX_HP(12)),
            CONNECTION(RR_SECRET_SHRINE_16_HEART, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),// && CHECK_MAX_HP(16)),
            CONNECTION(RR_SECRET_SHRINE_4_HEART, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),// && CHECK_MAX_HP(4)),
            CONNECTION(RR_SECRET_SHRINE_8_HEART, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_ZORA),// && CHECK_MAX_HP(8)),
            CONNECTION(RR_SECRET_SHRINE_ENTRANCE, CAN_BE_HUMAN || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA),
        }
    } },
    { RR_SECRET_SHRINE_ENTRANCE, RandoRegion{ .name = "Entrance", .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_POT_1, true),
            CHECK(RC_SECRET_SHRINE_POT_2, true),
            CHECK(RC_SECRET_SHRINE_POT_3, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 12),          ENTRANCE(SECRET_SHRINE, 0), true),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_MAIN_CENTER, CAN_BE_HUMAN && HAS_MAGIC && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_ITEM(ITEM_BOW))
        }
    } },
    { RR_SNOWHEAD_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SNOWHEAD, 2),                 ENTRANCE(FAIRY_FOUNTAIN, 2), true),
        },
    } },
    { RR_SNOWHEAD_NEAR_PATH, RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 1),         ENTRANCE(SNOWHEAD, 0), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_TEMPLE, CAN_BE_GORON && HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_SNOWHEAD), CLEAR_OWL_WARP(OWL_WARP_SNOWHEAD), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(SNOWHEAD, 3), // From Song of Soaring
        }
    } },
    { RR_SNOWHEAD_NEAR_TEMPLE, RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SNOWHEAD_TEMPLE, 0),          ENTRANCE(SNOWHEAD, 1), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 2),           ENTRANCE(SNOWHEAD, 2), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_PATH, true),
        },
    } },
    { RR_SNOWHEAD_TEMPLE, RandoRegion{ .sceneId = SCENE_HAKUGIN,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SNOWHEAD, 1),                 ENTRANCE(SNOWHEAD_TEMPLE, 0), true),
        },
    } },
    { RR_SOUTHERN_SWAMP_NORTH, RandoRegion{ .name = "North Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_HP, CAN_BE_DEKU && CAN_BE_HUMAN && Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_DEED, CAN_BE_HUMAN && Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1),   ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), true),
            EXIT(ENTRANCE(TOURIST_INFORMATION, 0),      ENTRANCE(SOUTHERN_SWAMP_POISONED, 1), true),
            EXIT(ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),   ENTRANCE(SOUTHERN_SWAMP_POISONED, 5), true),
            EXIT(ENTRANCE(WOODS_OF_MYSTERY, 0),         ENTRANCE(SOUTHERN_SWAMP_POISONED, 7), true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, (Flags_GetSceneSwitch(SCENE_20SICHITAI, 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE))),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP), CLEAR_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP), CAN_BE_HUMAN || CAN_BE_DEITY),
            EVENT(Flags_SetRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER), Flags_ClearRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER), true),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 9), // From river in Ikana
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // From Song of Soaring
        }
    } },
    { RR_STONE_TOWER_BOTTOM, RandoRegion{ .name = "Bottom", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_CLIMB_1, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_STONE_TOWER_POT_CLIMB_2, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(IKANA_CANYON, 3),             ENTRANCE(STONE_TOWER, 0), true)
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_HUMAN && CAN_BE_GORON && CAN_BE_ZORA),
        },
    } },
    { RR_STONE_TOWER_INVERTED_LOWER, RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(STONE_TOWER, 1),              ENTRANCE(STONE_TOWER_INVERTED, 0), HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC && CAN_BE_HUMAN),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_UPPER, CAN_GROW_BEAN_PLANT),
            CONNECTION(RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
    } },
    { RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0), ENTRANCE(STONE_TOWER_INVERTED, 1), true),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_LOWER, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
    } },
    { RR_STONE_TOWER_INVERTED_UPPER, RandoRegion{ .sceneId = SCENE_F41,
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
    } },
    { RR_STONE_TOWER_MIDDLE, RandoRegion{ .name = "Middle", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_1, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_2, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_3, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_4, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_5, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_6, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_7, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_8, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_9, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_10, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_11, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_POT_LOWER_SCARECROW_12, CAN_HOOK_SCARECROW),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_BOTTOM, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_HUMAN && CAN_BE_GORON && CAN_BE_ZORA),
            CONNECTION(RR_STONE_TOWER_UPPER, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
        },
    } },
    { RR_STONE_TOWER_TEMPLE_INVERTED, RandoRegion{ .sceneId = SCENE_INISIE_R,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 1),     ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0), true),
        },
    } },
    { RR_STONE_TOWER_TEMPLE, RandoRegion{ .sceneId = SCENE_INISIE_N,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(STONE_TOWER, 2),              ENTRANCE(STONE_TOWER_TEMPLE, 0), true),
        },
    } },
    // TODO: Probably need to split this up to account for entrance rando later
    { RR_STONE_TOWER_TOP, RandoRegion{ .name = "Top", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_1, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_2, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_3, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_4, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 0),     ENTRANCE(STONE_TOWER, 1), CAN_PLAY_SONG(ELEGY) && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC && CAN_BE_HUMAN),
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE, 0),       ENTRANCE(STONE_TOWER, 2), CAN_PLAY_SONG(ELEGY) && CAN_BE_HUMAN && CAN_BE_GORON && CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_HUMAN),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_STONE_TOWER), CLEAR_OWL_WARP(OWL_WARP_STONE_TOWER), CAN_BE_HUMAN || CAN_BE_DEITY)
        },
        .oneWayEntrances = {
            ENTRANCE(STONE_TOWER, 3), // From Song of Soaring
        }
    } },
    { RR_STONE_TOWER_UPPER, RandoRegion{ .name = "Upper", .sceneId = SCENE_F40,
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
            CONNECTION(RR_STONE_TOWER_MIDDLE, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_STONE_TOWER_TOP, CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT))
        },
    } },
    { RR_SOUTHERN_SWAMP_SOUTH, RandoRegion{ .name = "South Section", .sceneId = SCENE_20SICHITAI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WOODFALL, 0),                 ENTRANCE(SOUTHERN_SWAMP_POISONED, 2), CAN_BE_DEKU || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
            EXIT(ENTRANCE(DEKU_PALACE, 0),              ENTRANCE(SOUTHERN_SWAMP_POISONED, 3), true),
            EXIT(ENTRANCE(DEKU_PALACE, 5),              ENTRANCE(SOUTHERN_SWAMP_POISONED, 4), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(SWAMP_SPIDER_HOUSE, 0),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), HAS_ITEM(ITEM_DEKU_STICK) || (HAS_MAGIC && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE))),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_NORTH, (Flags_GetSceneSwitch(SCENE_20SICHITAI, 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE))),
        },
    } },
    { RR_SWAMP_SHOOTING_GALLERY, RandoRegion{ .sceneId = SCENE_SYATEKI_MORI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2),   ENTRANCE(SWAMP_SHOOTING_GALLERY, 0), true),
        },
    } },
    { RR_SWAMP_SPIDER_HOUSE, RandoRegion{ .sceneId = SCENE_KINSTA1,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 8),  ENTRANCE(SWAMP_SPIDER_HOUSE, 0), true),
        },
    } },
    { RR_SWORDSMAN_SCHOOL, RandoRegion{ .sceneId = SCENE_DOUJOU,
        .checks = {
            // Great Fairy Sword cannot be used here.
            // Deity cannot do the minigame, and is too tall to go to the hidden back area, but can open it up which is a slight complication for Sword rando.
            CHECK(RC_SWORDSMAN_SCHOOL_HP,  CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_1, CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_2, CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_3, CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_4, CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_5, CAN_BE_HUMAN && GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 3),          ENTRANCE(SWORDMANS_SCHOOL, 0), true),
        },
    } },
    { RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0), ENTRANCE(TERMINA_FIELD, 3), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_BE_HUMAN && HAS_ITEM(ITEM_BOW)),
        },
    } },
    { RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST, RandoRegion{ .name = "Before Great Bay Coast", .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 0),          ENTRANCE(TERMINA_FIELD, 2), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_BE_HUMAN && HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)),
        },
    } },
    { RR_TERMINA_FIELD, RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_TERMINA_FIELD_KAMARO, CAN_BE_HUMAN && HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_HEALING)),
            CHECK(RC_TERMINA_FIELD_POT, CAN_GROW_BEAN_PLANT),
            CHECK(RC_TERMINA_FIELD_TALL_GRASS_CHEST, true),
            CHECK(RC_TERMINA_FIELD_TREE_STUMP_CHEST, CAN_GROW_BEAN_PLANT || (CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT))),
            CHECK(RC_TERMINA_FIELD_WATER_CHEST, CAN_BE_ZORA),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 0),          ENTRANCE(TERMINA_FIELD, 0), true),
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0),   ENTRANCE(TERMINA_FIELD, 1), true),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 0),            ENTRANCE(TERMINA_FIELD, 4), true),
            EXIT(ENTRANCE(MILK_ROAD, 0),                ENTRANCE(TERMINA_FIELD, 5), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 1),         ENTRANCE(TERMINA_FIELD, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 0),          ENTRANCE(TERMINA_FIELD, 7), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 0),         ENTRANCE(TERMINA_FIELD, 8), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE, CAN_BE_HUMAN && HAS_ITEM(ITEM_BOW)),
            CONNECTION(RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST, CAN_BE_HUMAN && HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)),
            CONNECTION(RR_ASTRAL_OBSERVATORY_OUTSIDE, CAN_BE_DEKU)
        },
    } },
    { RR_TOURIST_INFORMATION, RandoRegion{ .sceneId = SCENE_MAP_SHOP,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 1),  ENTRANCE(TOURIST_INFORMATION, 0), true),
        },
        .events = {
            EVENT( // Killing Octorok blocking the southern swamp south section
                Flags_SetSceneSwitch(SCENE_20SICHITAI, 1),
                Flags_ClearSceneSwitch(SCENE_20SICHITAI, 1),
                true // TODO: Conditions for starting swamp tour
            ),
        },
    } },
    { RR_TOWN_DEKU_PLAYGROUND, RandoRegion{ .sceneId = SCENE_DEKUTES,
        .checks = {
            // TODO : Add checks for getting best time on all three days/Freestanding items.
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 4),         ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), true),
        },
    } },
    { RR_TOWN_SHOOTING_GALLERY, RandoRegion{ .sceneId = SCENE_SYATEKI_MIZU,
        .checks = {
            // TODO : Add Bow Capacity upgrade/HP checks here.
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 8),          ENTRANCE(TOWN_SHOOTING_GALLERY, 0), true),
        },
    } },
    { RR_TRADING_POST, RandoRegion{ .sceneId = SCENE_8ITEMSHOP,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_TRADING_POST_POT, true), // Note : Goron has to sidehop to get up.
            CHECK(RC_TRADING_POST_SHOP_ITEM_1, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_2, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_3, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_4, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_5, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_6, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_7, true),
            CHECK(RC_TRADING_POST_SHOP_ITEM_8, true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 5),          ENTRANCE(TRADING_POST, 0), true),
        
        },
    } },
    { RR_TREASURE_SHOP, RandoRegion{ .sceneId = SCENE_TAKARAYA,
        .checks = {
            // TODO : Add check for each form(minus FD)
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 4),          ENTRANCE(TREASURE_CHEST_SHOP, 0), true),
        },
    } },
    { RR_WATERFALL_RAPIDS, RandoRegion{ .sceneId = SCENE_35TAKI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_CAPE, 4),                ENTRANCE(WATERFALL_RAPIDS, 0), true),
        },
    } },
    { RR_WOODFALL_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Woodfall", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            // TODO: We can't add this till all stray fairies are in the pool
            // CHECK(RC_WOODFALL_GREAT_FAIRY, gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_WOODFALL_TEMPLE] >= 15),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WOODFALL, 2),                 ENTRANCE(FAIRY_FOUNTAIN, 1), true),
        },
    } },
    { RR_WOODFALL_TEMPLE, RandoRegion{ .sceneId = SCENE_MITURIN,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(WOODFALL, 1),                 ENTRANCE(WOODFALL_TEMPLE, 0), true),
        },
    } },
    { RR_WOODFALL, RandoRegion{ .sceneId = SCENE_21MITURINMAE,
        .checks = {
            CHECK(RC_WOODFALL_ENTRANCE_CHEST, CAN_BE_DEKU || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
            CHECK(RC_WOODFALL_HP_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_NEAR_OWL_CHEST, CAN_BE_DEKU),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 2),  ENTRANCE(WOODFALL, 0), true),
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 0),          ENTRANCE(WOODFALL, 1), CAN_BE_DEKU),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 1),           ENTRANCE(WOODFALL, 2), CAN_BE_DEKU),
        },
    } },
    { RR_WOODS_OF_MYSTERY, RandoRegion{ .sceneId = SCENE_26SARUNOMORI,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 7),  ENTRANCE(WOODS_OF_MYSTERY, 0), true),
        },
    } },   
    { RR_ZORA_CAPE, RandoRegion{ .sceneId = SCENE_31MISAKI,
        .checks = {
            // TODO: Grottos
            CHECK(RC_ZORA_CAPE_GROTTO,                    CAN_BE_HUMAN && (CAN_USE_EXPLOSIVE || CAN_BE_GORON)),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_1,             CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_DEKU),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_2,             CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ZORA_CAPE_UNDERWATER_CHEST,          CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_WATERFALL_HP,              CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_1,        true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_2,        true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 1),          ENTRANCE(ZORA_CAPE, 0), true),
            EXIT(ENTRANCE(ZORA_HALL, 0),                ENTRANCE(ZORA_CAPE, 1), CAN_BE_ZORA),
            EXIT(ENTRANCE(WATERFALL_RAPIDS, 0),         ENTRANCE(ZORA_CAPE, 4), CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT)),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 3),           ENTRANCE(ZORA_CAPE, 5), CAN_BE_HUMAN && HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_EXPLOSIVE),
        },
        .connections = {
            CONNECTION(RR_ZORA_HALL_COAST, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
    } },
    { RR_ZORA_HALL_COAST, RandoRegion{ .name = "Zora Hall Coast", .sceneId = SCENE_31MISAKI,
        .checks = {
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_1,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_2,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_3,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_4,     true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL,        1),         ENTRANCE(ZORA_CAPE, 2), true),
            EXIT(ENTRANCE(GREAT_BAY_TEMPLE, 1),         ENTRANCE(ZORA_CAPE, 7), CAN_BE_HUMAN && CAN_BE_ZORA && CHECK_QUEST_ITEM(QUEST_SONG_BOSSA_NOVA) && HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .connections = {
            CONNECTION(RR_ZORA_CAPE, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DEITY),
        },
        .events = {
            EVENT(SET_OWL_WARP(OWL_WARP_ZORA_CAPE), CLEAR_OWL_WARP(OWL_WARP_ZORA_CAPE), CAN_BE_HUMAN || CAN_BE_DEITY),
        },
        .oneWayEntrances = {
            ENTRANCE(ZORA_CAPE, 6), // From Song of Soaring
        },
    } },
    { RR_ZORA_HALL_EVANS_ROOM, RandoRegion{ .name = "Evan's Room", .sceneId = SCENE_BANDROOM,
        // TODO: Missing HP check to add here later.
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL, 4),                ENTRANCE(ZORA_HALL_ROOMS, 3), true),
        },
    } },
    { RR_ZORA_HALL_JAPAS_ROOM, RandoRegion{ .name = "Japa's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL, 5),                ENTRANCE(ZORA_HALL_ROOMS, 1), true),
        },
    } },
    { RR_ZORA_HALL_LULUS_ROOM, RandoRegion{ .name = "Lulu's Room", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_HALL_SCRUB_DEED,          Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON),
            CHECK(RC_ZORA_HALL_SCRUB_HP,            Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON && CAN_BE_DEKU),
            CHECK(RC_ZORA_HALL_SCRUB_POTION_REFILL, CAN_BE_ZORA),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL, 3),                ENTRANCE(ZORA_HALL_ROOMS, 2), true),
        },
    } },
    { RR_ZORA_HALL_MIKAUS_ROOM, RandoRegion{ .name = "Mikau's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL, 6),                ENTRANCE(ZORA_HALL_ROOMS, 0), true),
        },
    } },
    { RR_ZORA_HALL_SHOP, RandoRegion{ .name = "Shop", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_SHOP_ITEM_1,              true),
            CHECK(RC_ZORA_SHOP_ITEM_2,              true),
            CHECK(RC_ZORA_SHOP_ITEM_3,              true),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_HALL, 2),                ENTRANCE(ZORA_HALL_ROOMS, 5), true),
        },
    } },
    { RR_ZORA_HALL, RandoRegion{ .sceneId = SCENE_33ZORACITY,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(ZORA_CAPE, 1),                ENTRANCE(ZORA_HALL, 0), true),           
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 5),          ENTRANCE(ZORA_HALL, 2), true), // To Shop
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 2),          ENTRANCE(ZORA_HALL, 3), CAN_BE_ZORA), // To Lulu's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 3),          ENTRANCE(ZORA_HALL, 4), CAN_BE_ZORA), // To Evan's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 1),          ENTRANCE(ZORA_HALL, 5), CAN_BE_ZORA), // To Japas's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 0),          ENTRANCE(ZORA_HALL, 6), CAN_BE_ZORA), // To Mikaus's Room
        },
    } },
    { RR_MAX, RandoRegion{ .sceneId = SCENE_MAX,
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),                  ONE_WAY_EXIT, true), // Save warp
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 9),                  ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_CLOCK_TOWN)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 10),          ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP)),
            EXIT(ENTRANCE(MILK_ROAD, 4),                         ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MILK_ROAD)),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),           ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE)),
            EXIT(ENTRANCE(SNOWHEAD, 3),                          ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SNOWHEAD)),
            EXIT(ENTRANCE(GREAT_BAY_COAST, 11),                  ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_GREAT_BAY_COAST)),
            EXIT(ENTRANCE(ZORA_CAPE, 6),                         ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_ZORA_CAPE)),
            EXIT(ENTRANCE(IKANA_CANYON, 4),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_IKANA_CANYON)),
            EXIT(ENTRANCE(STONE_TOWER, 3),                       ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_STONE_TOWER)),
        },
    } },
};
// clang-format on

RandoRegionId GetRegionIdFromEntrance(s32 entrance) {
    static std::unordered_map<s32, RandoRegionId> entranceToRegionId;
    if (entranceToRegionId.empty()) {
        for (auto& [randoRegionId, randoRegion] : Regions) {
            for (auto& [_, regionExit] : randoRegion.exits) {
                if (regionExit.returnEntrance == ONE_WAY_EXIT) {
                    continue;
                }
                entranceToRegionId[regionExit.returnEntrance] = randoRegionId;
            }
            for (auto& entrance : randoRegion.oneWayEntrances) {
                entranceToRegionId[entrance] = randoRegionId;
            }
        }
    }

    if (entranceToRegionId.contains(entrance)) {
        return entranceToRegionId[entrance];
    }

    return RR_MAX;
}

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions) {
    auto& randoRegion = Rando::Logic::Regions[currentRegion];

    for (auto& [connectedRegionId, condition] : randoRegion.connections) {
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && condition.first()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }

    for (auto& [exitId, regionExit] : randoRegion.exits) {
        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && regionExit.condition()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }
}

} // namespace Logic

} // namespace Rando
