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
#define CAN_HOOK_SCARECROW (HAS_ITEM(ITEM_OCARINA_OF_TIME) && HAS_ITEM(ITEM_HOOKSHOT))
#define CAN_USE_EXPLOSIVE ((HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU) || HAS_ITEM(ITEM_MASK_BLAST)))
#define CAN_USE_HUMAN_SWORD (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI)
#define CAN_USE_SWORD (CAN_USE_HUMAN_SWORD || HAS_ITEM(ITEM_SWORD_GREAT_FAIRY) || CAN_BE_DEITY)
// Be careful here, as some checks require you to play the song as a specific form
#define CAN_PLAY_SONG(song) (HAS_ITEM(ITEM_OCARINA_OF_TIME) && CHECK_QUEST_ITEM(QUEST_SONG_##song))
#define CAN_RIDE_EPONA (CAN_PLAY_SONG(EPONA))
#define ONE_WAY_EXIT -1
#define CAN_OWL_WARP(owlId) ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> owlId) & 1)
#define SET_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags |= (1 << owlId))
#define CLEAR_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags &= ~(1 << owlId))
#define HAS_BOTTLE_ITEM(item) (Inventory_HasItemInBottle(item))
// TODO: Maybe not reliable because of theif bird stealing bottle
#define HAS_BOTTLE (INV_CONTENT(ITEM_BOTTLE) != ITEM_NONE)
#define CAN_USE_PROJECTILE (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_ZORA)
#define CAN_GET_SPRING_WATER                                                  \
    (HAS_BOTTLE && Flags_GetRandoInf(RANDO_INF_HAS_ACCESS_TO_SPRING_WATER) || \
     Flags_GetRandoInf(RANDO_INF_HAS_ACCESS_TO_HOT_SPRING_WATER))
#define CAN_GROW_BEAN_PLANT (HAS_ITEM(ITEM_MAGIC_BEANS) && (CAN_PLAY_SONG(STORMS) || CAN_GET_SPRING_WATER))
// TODO: Move these into a seperate file later?
// After thinking about it I decided to cut explosives or "technically possible but annoying" methods from these.
#define CAN_KILL_DINALFOS (CAN_USE_SWORD || CAN_BE_GORON)
#define CAN_KILL_WIZZROBE (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_SWORD || CAN_BE_GORON)
#define CAN_KILL_WART (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)
#define CAN_KILL_GARO_MASTER (HAS_ITEM(ITEM_BOW) || CAN_BE_GORON || CAN_USE_SWORD)
#define CAN_KILL_IRONKNUCKLE (CAN_USE_SWORD || CAN_BE_GORON)
#define CAN_KILL_BAT \
    (CAN_USE_SWORD || HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_BOW) || CAN_USE_EXPLOSIVE || CAN_BE_GORON || CAN_BE_ZORA)
#define CAN_LIGHT_TORCH_NEAR_ANOTHER \
    (HAS_ITEM(ITEM_DEKU_STICK) || (HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE) && HAS_MAGIC))
#define KEY_COUNT(dungeon) (gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_##dungeon])

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
bool Flags_GetSceneClear(s32 scene, s32 roomNumber) {
    if (gPlayState != NULL && gPlayState->sceneId == scene) {
        return (gPlayState->actorCtx.sceneFlags.clearedRoom & (1 << roomNumber));
    }

    return (gSaveContext.cycleSceneFlags[scene].clearedRoom & (1 << roomNumber));
}
void Flags_SetSceneClear(s32 scene, s32 roomNumber) {
    gSaveContext.cycleSceneFlags[scene].clearedRoom |= (1 << roomNumber);
}
void Flags_UnsetSceneClear(s32 scene, s32 roomNumber) {
    gSaveContext.cycleSceneFlags[scene].clearedRoom &= ~(1 << roomNumber);
}

s32 Cutscene_CountNormalMasks(void) {
    s32 count = 0;

    if (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAFEIS_MASK) == ITEM_MASK_KAFEIS_MASK) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ALL_NIGHT) == ITEM_MASK_ALL_NIGHT) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CIRCUS_LEADER) == ITEM_MASK_CIRCUS_LEADER) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_POSTMAN) == ITEM_MASK_POSTMAN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_COUPLE) == ITEM_MASK_COUPLE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIBDO) == ITEM_MASK_GIBDO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_DON_GERO) == ITEM_MASK_DON_GERO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CAPTAIN) == ITEM_MASK_CAPTAIN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_STONE) == ITEM_MASK_STONE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BREMEN) == ITEM_MASK_BREMEN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_SCENTS) == ITEM_MASK_SCENTS) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIANT) == ITEM_MASK_GIANT) {
        count++;
    }

    return count;
}

std::string LogicString(std::string condition) {
    if (condition == "true")
        return "";

    return condition;
}

#define EVENT(name, isApplied, onApply, onRemove, condition)                                                       \
    {                                                                                                              \
        name, [] { return isApplied; }, [] { return onApply; }, [] { return onRemove; }, [] { return condition; }, \
            LogicString(#condition)                                                                                \
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
#define EVENT_OWL_WARP(owlId)                                                                                         \
    {                                                                                                                 \
        "Owl Statue", [] { return CAN_OWL_WARP(owlId); }, [] { SET_OWL_WARP(owlId); }, [] { CLEAR_OWL_WARP(owlId); }, \
            [] { return RANDO_SAVE_OPTIONS[RO_SHUFFLE_OWL_STATUES] == RO_GENERIC_NO && CAN_USE_SWORD; },              \
            "CAN_USE_SWORD"                                                                                           \
    }
#define EVENT_WEEKEVENTREG(name, flag, condition)                                               \
    {                                                                                           \
        name, [] { return CHECK_WEEKEVENTREG(flag); }, [] { SET_WEEKEVENTREG(flag); },          \
            [] { CLEAR_WEEKEVENTREG(flag); }, [] { return condition; }, LogicString(#condition) \
    }
#define EVENT_RANDOINF(name, flag, condition)                                                    \
    {                                                                                            \
        name, [] { return Flags_GetRandoInf(flag); }, [] { Flags_SetRandoInf(flag); },           \
            [] { Flags_ClearRandoInf(flag); }, [] { return condition; }, LogicString(#condition) \
    }

// clang-format off
std::unordered_map<RandoRegionId, RandoRegion> Regions = {
    { RR_ASTRAL_OBSERVATORY_OUTSIDE, RandoRegion{ .name = "Outside Astral Observatory", .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1),           ENTRANCE(TERMINA_FIELD, 9), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_GROW_BEAN_PLANT)
        },
    } },
    { RR_ASTRAL_OBSERVATORY_PASSAGE, RandoRegion{ .name = "Passage", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, CAN_USE_EXPLOSIVE),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_3, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2),              ENTRANCE(ASTRAL_OBSERVATORY, 0), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY, (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA),
        },
    } },
    { RR_ASTRAL_OBSERVATORY, RandoRegion{ .name = "Inside Astral Observatory", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 9),                ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA), // TODO: Trick to backflip over balloon
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_DAMPE, RandoRegion{ .sceneId = SCENE_DANPEI2TEST,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST, HAS_ITEM(ITEM_BOW)),
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 4),                       ONE_WAY_EXIT, true),
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
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, CAN_KILL_IRONKNUCKLE),
        },
    } },
    { RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, RandoRegion{ .name = "Night 1 Grave", .sceneId = SCENE_HAKASHITA,
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 2),              ENTRANCE(BENEATH_THE_GRAVERYARD, 0), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    } },
    { RR_BENEATH_THE_WELL, RandoRegion{ .sceneId = SCENE_REDEAD,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 5),                 ENTRANCE(BENEATH_THE_WELL, 0), true),
        },
    } },
    { RR_BOMB_SHOP, RandoRegion{ .sceneId = SCENE_BOMYA,
        .checks = {
            CHECK(RC_BOMB_SHOP_ITEM_1, true),
            CHECK(RC_BOMB_SHOP_ITEM_2, true),
            CHECK(RC_BOMB_SHOP_ITEM_3, true),
            // TODO: Bigger bomb bag
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 6),              ENTRANCE(BOMB_SHOP, 0), true),
        },
    } },
    { RR_CLOCK_TOWER_INTERIOR, RandoRegion{ .sceneId = SCENE_INSIDETOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),             ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
        },
    } },
    { RR_CLOCK_TOWER_ROOF, RandoRegion{ .sceneId = SCENE_OKUJOU,
        .checks = {
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, (HAS_MAGIC && CAN_BE_DEKU) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, (HAS_MAGIC && CAN_BE_DEKU) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_1, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_2, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_3, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_4, true),
        },
        .exits = {
            EXIT(ENTRANCE(THE_MOON, 0),                 ONE_WAY_EXIT, CAN_PLAY_SONG(OATH)),
        },
        .oneWayEntrances = {
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
    } },
    { RR_CLOCK_TOWN_EAST, RandoRegion{ .sceneId = SCENE_TOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWN_EAST_UPPER_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 7),                ENTRANCE(EAST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7),             ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0),           ENTRANCE(EAST_CLOCK_TOWN, 2), true), // TODO: Bombers Code req
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2),             ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0),          ENTRANCE(EAST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1),             ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),      ENTRANCE(EAST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0),             ENTRANCE(EAST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0),        ENTRANCE(EAST_CLOCK_TOWN, 8), true),
            EXIT(ENTRANCE(STOCK_POT_INN, 0),                ENTRANCE(EAST_CLOCK_TOWN, 9), true), // To lobby
            EXIT(ENTRANCE(STOCK_POT_INN, 1),                ENTRANCE(EAST_CLOCK_TOWN, 10), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0),                     ENTRANCE(EAST_CLOCK_TOWN, 11), HAS_ITEM(ITEM_MASK_ROMANI)),
        },
    } },
    { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Clock Town", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            // TODO: Do we want there to be two checks here? In vanilla you can get one right away but the other requires human form. In rando you start as human. Maybe the other should require any form?
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3),             ENTRANCE(FAIRY_FOUNTAIN, 0), true),
        },
    } },
    { RR_CLOCK_TOWN_LAUNDRY, RandoRegion{ .sceneId = SCENE_ALLEY,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6),             ENTRANCE(LAUNDRY_POOL, 0), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 1),               ENTRANCE(LAUNDRY_POOL, 1), Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI))
        },
    } },
    { RR_CLOCK_TOWN_NORTH, RandoRegion{ .sceneId = SCENE_BACKTOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_2, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_HP, true),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, CAN_USE_SWORD || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 8),                ENTRANCE(NORTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5),              ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4),             ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0),               ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0),        ENTRANCE(NORTH_CLOCK_TOWN, 4), CAN_BE_DEKU),
        },
    } },
    { RR_CLOCK_TOWN_SOUTH, RandoRegion{ .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, true),
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWN_SOUTH_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1),         ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6),                ENTRANCE(SOUTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3),              ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2),              ENTRANCE(SOUTH_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2),             ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 5), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0),                 ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0),                   ONE_WAY_EXIT, true),
        },
        .connections = {
            CONNECTION(RR_MAX, true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_CLOCK_TOWN),
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
            CHECK(RC_CLOCK_TOWN_WEST_SISTERS_HP, HAS_ITEM(ITEM_MASK_KAMARO)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 0),                ENTRANCE(WEST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 5),             ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 3),             ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(SWORDMANS_SCHOOL, 0),             ENTRANCE(WEST_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 0),               ENTRANCE(WEST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(TRADING_POST, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(BOMB_SHOP, 0),                    ENTRANCE(WEST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(POST_OFFICE, 0),                  ENTRANCE(WEST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(LOTTERY_SHOP, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 8), true),
        },
    } },
    { RR_CUCCO_SHACK, RandoRegion{ .sceneId = SCENE_F01C,
        .checks = {
            CHECK(RC_ROMANI_RANCH_GROG, HAS_ITEM(ITEM_MASK_BREMEN)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 4),                 ENTRANCE(CUCCO_SHACK, 0), true),
        },
    } },
    { RR_CURIOSITY_SHOP_BACK, RandoRegion{ .name = "Back", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add Keaton Mask/Express Letter to Mama checks
            CHECK(RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(LAUNDRY_POOL, 1),                 ENTRANCE(CURIOSITY_SHOP, 1), true)
        },
    } },
    { RR_CURIOSITY_SHOP_FRONT, RandoRegion{ .name = "Front", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add the shop checks
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 4),              ENTRANCE(CURIOSITY_SHOP, 0), true)
        },
    } },
    { RR_DEKU_KINGS_CHAMBER_HOLDING_CELL, RandoRegion{ .name = "Holding Cell", .sceneId = SCENE_DEKU_KING,
        .checks = {
            CHECK(RC_DEKU_KINGS_CHAMBER_MONKEY, CAN_BE_DEKU && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 3),                  ENTRANCE(DEKU_KINGS_CHAMBER, 1), true),
        },
    } },
    { RR_DEKU_KINGS_CHAMBER, RandoRegion{ .sceneId = SCENE_DEKU_KING,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 2),                  ENTRANCE(DEKU_KINGS_CHAMBER, 0), true),
        },
    } },
    { RR_DEKU_PALACE_INSIDE, RandoRegion{ .name = "Inside", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_DEKU_PALACE_HP, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 0),           ENTRANCE(DEKU_PALACE, 2), true),
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 1),           ENTRANCE(DEKU_PALACE, 3), CAN_BE_DEKU), // Cell TODO: Is there something to do with beans here?
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_OUTSIDE, true),
        },
    } },
    { RR_DEKU_PALACE_OUTSIDE, RandoRegion{ .name = "Outside", .sceneId = SCENE_22DEKUCITY,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 3),      ENTRANCE(DEKU_PALACE, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 4),      ENTRANCE(DEKU_PALACE, 5), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(DEKU_SHRINE, 0),                  ENTRANCE(DEKU_PALACE, 4), CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE, CAN_BE_DEKU),
        },
    } },
    { RR_DEKU_SHRINE, RandoRegion{ .sceneId = SCENE_DANPEI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 4),                  ENTRANCE(DEKU_SHRINE, 0), true),
        },
    } },
    { RR_DOGGY_RACETRACK, RandoRegion{ .sceneId = SCENE_F01_B,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 5),                 ENTRANCE(DOGGY_RACETRACK, 0), true),
        },
    } },
    { RR_FISHERMANS_HUT, RandoRegion{ .sceneId = SCENE_FISHERMAN,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 4),              ENTRANCE(FISHERMANS_HUT, 0), true),
        },
    } },
    { RR_GHOST_HUT, RandoRegion{ .sceneId = SCENE_TOUGITES,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 1),                 ENTRANCE(GHOST_HUT, 0), true),
        },
    } },
    { RR_GORMAN_TRACK, RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 2),                    ENTRANCE(GORMAN_TRACK, 3), CAN_PLAY_SONG(EPONA)),
            EXIT(ENTRANCE(MILK_ROAD, 3),                    ENTRANCE(GORMAN_TRACK, 0), true),
        },
    } },
    { RR_GORON_GRAVEYARD, RandoRegion{ .sceneId = SCENE_GORON_HAKA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3),      ENTRANCE(GORON_GRAVERYARD, 0), true),
        },
    } },
    { RR_GREAT_BAY_COAST, RandoRegion{ .sceneId = SCENE_30GYOSON,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_GREAT_BAY_COAST_MIKAU, CAN_PLAY_SONG(HEALING)),
            CHECK(RC_GREAT_BAY_COAST_HP, CAN_HOOK_SCARECROW && CAN_GROW_BEAN_PLANT),
            CHECK(RC_GREAT_BAY_COAST_POT_1, true),
            CHECK(RC_GREAT_BAY_COAST_POT_2, true),
            CHECK(RC_GREAT_BAY_COAST_POT_3, true),
            CHECK(RC_GREAT_BAY_COAST_POT_4, true),
            CHECK(RC_GREAT_BAY_COAST_POT_5, true),
            CHECK(RC_GREAT_BAY_COAST_POT_6, true),
            CHECK(RC_GREAT_BAY_COAST_POT_7, true),
            CHECK(RC_GREAT_BAY_COAST_POT_8, true),
            CHECK(RC_GREAT_BAY_COAST_POT_9, true),
            CHECK(RC_GREAT_BAY_COAST_POT_10, true),
            CHECK(RC_GREAT_BAY_COAST_POT_11, true),
            CHECK(RC_GREAT_BAY_COAST_POT_12, true),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_1, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_2, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_3, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_GREAT_BAY_COAST_TINGLE_MAP_2, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 2),                ENTRANCE(GREAT_BAY_COAST, 0), true),
            EXIT(ENTRANCE(ZORA_CAPE, 0),                    ENTRANCE(GREAT_BAY_COAST, 1), true),
            EXIT(ENTRANCE(PINNACLE_ROCK, 0),                ENTRANCE(GREAT_BAY_COAST, 3), true),
            EXIT(ENTRANCE(FISHERMANS_HUT, 0),               ENTRANCE(GREAT_BAY_COAST, 4), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 0),    ENTRANCE(GREAT_BAY_COAST, 5), CAN_BE_ZORA),
            EXIT(ENTRANCE(MARINE_RESEARCH_LAB, 0),          ENTRANCE(GREAT_BAY_COAST, 7), true),
            EXIT(ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0),       ENTRANCE(GREAT_BAY_COAST, 8), true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_GREAT_BAY_COAST),
        },
        .oneWayEntrances = {
            ENTRANCE(GREAT_BAY_COAST, 11), // From Song of Soaring
            ENTRANCE(GREAT_BAY_COAST, 12), // From being captured in Pirate Fortress Moat
        },
    } },
    { RR_GREAT_BAY_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 5),                    ENTRANCE(FAIRY_FOUNTAIN, 3), true),
        },
    } },
    { RR_GREAT_BAY_TEMPLE_BOSS_ROOM, RandoRegion{ .sceneId = SCENE_SEA_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Gyorg)?
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_HC, CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_WARP, CAN_BE_ZORA && HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 9),                             ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(GYORGS_LAIR, 0), // Great Bay Temple Pre Boss Room
        },
    } },
    // TODO: Split this up into regions
    { RR_GREAT_BAY_TEMPLE, RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_BABA_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_KEY, true),
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_UNDERWATER, true),
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS, true),
            CHECK(RC_GREAT_BAY_TEMPLE_ENTRANCE_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_LOWER_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_UPPER_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_CHEST, true),
            CHECK(RC_GREAT_BAY_TEMPLE_ICE_ARROW, true),
            CHECK(RC_GREAT_BAY_TEMPLE_MAP, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_BARREL, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_UNDERWATER_POT, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_COMPASS_ROOM_POT, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_GREEN_PIPE_3_BARREL, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_MAP_ROOM_POT, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_ABOVE_WATER, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_UNDERWATER, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_PLATFORM, true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_SKULLTULA, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_WARP, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_10, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_11, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_12, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_8, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_9, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_8, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_LOWER, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_8, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_8, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_8, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 7),                    ENTRANCE(GREAT_BAY_TEMPLE, 1), true),
            EXIT(ENTRANCE(GYORGS_LAIR, 0),                           ONE_WAY_EXIT, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_GREAT_BAY_TEMPLE)),
        },
    } },
    { RR_HONEY_AND_DARLING, RandoRegion{ .sceneId = SCENE_BOWLING,
        .checks = {
            // TODO : Add checks for all 3 days for this shop.
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 6),              ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), true),
        },
    } },
    { RR_IKANA_CANYON_CAVE, RandoRegion{ .name = "Cave", .sceneId = SCENE_IKANA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 13),                ENTRANCE(IKANA_CANYON, 14), true),
        },
        .events = {
            EVENT_WEEKEVENTREG("Free Ghost Dude", WEEKEVENTREG_14_04, CAN_PLAY_SONG(STORMS)),
        },
    } },
    { RR_IKANA_CANYON_LOWER, RandoRegion{ .name = "Lower", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_SCRUB_HP, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA && CAN_BE_DEKU),
            CHECK(RC_IKANA_CANYON_SCRUB_HUGE_RUPEE, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA),
            CHECK(RC_IKANA_CANYON_SCRUB_POTION_REFILL, true),
            // TODO: Grottos
            CHECK(RC_IKANA_CANYON_GROTTO, true),
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
            CONNECTION(RR_IKANA_CANYON_UPPER, HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_BOW) && HAS_MAGIC && HAS_ITEM(ITEM_ARROW_ICE))
        },
    } },
    { RR_IKANA_CANYON_UPPER, RandoRegion{ .name = "Upper", .sceneId = SCENE_IKANA,
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
    } },
    { RR_IKANA_CASTLE, RandoRegion{ .sceneId = SCENE_CASTLE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 8),                 ENTRANCE(IKANA_CASTLE, 1), true),
        },
    } },
    { RR_IKANA_GRAVEYARD_LOWER, RandoRegion{ .name = "Lower", .sceneId = SCENE_BOTI,
        .checks = {
            // TODO : Grotto
            CHECK(RC_IKANA_GRAVEYARD_GROTTO, CAN_USE_EXPLOSIVE || CAN_BE_GORON)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 2),                ENTRANCE(IKANA_GRAVEYARD, 0), true),
            EXIT(ENTRANCE(DAMPES_HOUSE, 0),                          ONE_WAY_EXIT, HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 3 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 0),       ENTRANCE(IKANA_GRAVEYARD, 2), HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 2 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 1),       ENTRANCE(IKANA_GRAVEYARD, 3), HAS_ITEM(ITEM_MASK_CAPTAIN)), // Day 1 hole
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 11),                ENTRANCE(FAIRY_FOUNTAIN, 4), true),
        },
    } },
    { RR_INN, RandoRegion{ .sceneId = SCENE_YADOYA,
        .checks = {
            // TODO : Add Couples Mask check here.
            CHECK(RC_STOCK_POT_INN_GRANDMA_LONG_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GRANDMA_SHORT_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GUEST_ROOM_CHEST,Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            CHECK(RC_STOCK_POT_INN_LETTER_TO_KAFEI, HAS_ITEM(ITEM_MASK_KAFEIS_MASK)),
            CHECK(RC_STOCK_POT_INN_ROOM_KEY, true),
            CHECK(RC_STOCK_POT_INN_STAFF_ROOM_CHEST, true),
            CHECK(RC_STOCK_POT_INN_TOILET_HAND, 
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) || Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)
            ),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 9),              ENTRANCE(STOCK_POT_INN, 0), true), // From ground floor
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 10),             ENTRANCE(STOCK_POT_INN, 1), true), // From upstairs
        },
    } },
    { RR_LOTTERY_SHOP, RandoRegion{ .sceneId = SCENE_TAKARAKUJI,
        .checks = {
            // Are we adding the lotter reward as a check?
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 8),              ENTRANCE(LOTTERY_SHOP, 0), true),
        },
    } },
    { RR_MAGIC_HAGS_POTION_SHOP, RandoRegion{ .sceneId = SCENE_WITCH_SHOP,
        .checks = {
            // TODO: Shop prices vs adult wallet?
            CHECK(RC_HAGS_POTION_SHOP_ITEM_1, true),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_2, true),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 5),      ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0), true),
        },
    } },
    { RR_MARINE_RESEARCH_LAB, RandoRegion{ .sceneId = SCENE_LABO,
        .checks = {
            // TODO: eggs
            CHECK(RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA, CAN_BE_ZORA && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 7),              ENTRANCE(MARINE_RESEARCH_LAB, 0), true),
        },
    } },
    { RR_MAYOR_RESIDENCE, RandoRegion{ .sceneId = SCENE_SONCHONOIE,
        .checks = {
            CHECK(RC_MAYORS_OFFICE_HP, HAS_ITEM(ITEM_MASK_COUPLE)),
            CHECK(RC_MAYORS_OFFICE_KAFEIS_MASK, true)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 7),              ENTRANCE(MAYORS_RESIDENCE, 0), true),
        },
    } },
    { RR_MILK_BAR, RandoRegion{ .sceneId = SCENE_MILK_BAR,
        .checks = {
            // TODO : Add shop checks/Circus Leader's Mask checks.
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 11),             ENTRANCE(MILK_BAR, 0), true),
        },
    } },
    { RR_MILK_ROAD, RandoRegion{ .sceneId = SCENE_ROMANYMAE,
        .checks = {
            CHECK(RC_MILK_ROAD_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_2, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 5),                ENTRANCE(MILK_ROAD, 0), true),
            EXIT(ENTRANCE(ROMANI_RANCH, 0),                 ENTRANCE(MILK_ROAD, 1), true),
            EXIT(ENTRANCE(GORMAN_TRACK, 3),                 ENTRANCE(MILK_ROAD, 2), CAN_PLAY_SONG(EPONA)),
            EXIT(ENTRANCE(GORMAN_TRACK, 0),                 ENTRANCE(MILK_ROAD, 3), true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_MILK_ROAD),
        },
        .oneWayEntrances = {
            ENTRANCE(MILK_ROAD, 4), // From Song of Soaring
        }
    } },
    { RR_MOON, RandoRegion{ .sceneId = SCENE_SOUGEN,
    // TO-DO: Mask requirements per Trial [Deku:1/1 | Goron:2/2 | Zora:3/3 | Link:4/4]
        .checks = {
            CHECK(RC_MOON_FIERCE_DEITY_MASK, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0),                   ONE_WAY_EXIT, true),
            EXIT(ENTRANCE(MOON_DEKU_TRIAL, 0),              ENTRANCE(THE_MOON, 0), true),
            EXIT(ENTRANCE(MOON_GORON_TRIAL, 0),             ENTRANCE(THE_MOON, 0), true),
            EXIT(ENTRANCE(MOON_LINK_TRIAL, 0),              ENTRANCE(THE_MOON, 0), true),
            EXIT(ENTRANCE(MOON_ZORA_TRIAL, 0),              ENTRANCE(THE_MOON, 0), true),
            EXIT(ENTRANCE(MAJORAS_LAIR, 0),                          ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(THE_MOON, 0),
        },
    } },
    { RR_MOON_DEKU_TRIAL, RandoRegion{ .sceneId = SCENE_LAST_DEKU,
        .checks = {
            CHECK(RC_MOON_TRIAL_DEKU_HP, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_DEKU_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                     ONE_WAY_EXIT, CAN_BE_DEKU), // Exit from Mask NPC
        },
    } },
    { RR_MOON_GORON_TRIAL, RandoRegion{ .sceneId = SCENE_LAST_GORON,
        .checks = {
            CHECK(RC_MOON_TRIAL_GORON_HP, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_EARLY_1, true),
            CHECK(RC_MOON_TRIAL_GORON_POT_EARLY_2, true),
            CHECK(RC_MOON_TRIAL_GORON_POT_EARLY_3, true),
            CHECK(RC_MOON_TRIAL_GORON_POT_EARLY_4, true),
            CHECK(RC_MOON_TRIAL_GORON_POT_1, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_10, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_11, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_2, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_3, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_4, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_5, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_6, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_7, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_8, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_9, CAN_BE_GORON && HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_GORON_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                     ONE_WAY_EXIT, CAN_BE_GORON && HAS_MAGIC), // Exit from Mask NPC
        },
    } },
    { RR_MOON_LINK_TRIAL, RandoRegion{ .sceneId = SCENE_LAST_LINK,
        .checks = {
            CHECK(RC_MOON_TRIAL_LINK_GARO_MASTER_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_MOON_TRIAL_LINK_HP, HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_BOMBCHU) && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_MOON_TRIAL_LINK_IRON_KNUCKLE_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_MOON_TRIAL_LINK_POT_1, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_2, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_3, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_4, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_5, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_6, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_7, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_8, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_LINK_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                     ONE_WAY_EXIT, HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_BOMBCHU) && HAS_ITEM(ITEM_BOW)), // Exit from Mask NPC
        },
    } },
    { RR_MOON_MAJORAS_LAIR, RandoRegion{ .sceneId = SCENE_LAST_BS,
        .oneWayEntrances = {
            ENTRANCE(MAJORAS_LAIR, 0),
        },
    } },
    { RR_MOON_ZORA_TRIAL, RandoRegion{ .sceneId = SCENE_LAST_ZORA,
        .checks = {
            CHECK(RC_MOON_TRIAL_ZORA_HP, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_ZORA_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                     ONE_WAY_EXIT, true), // Exit from Mask NPC
        },
    } },
    { RR_MOUNTAIN_SMITHY, RandoRegion{ .sceneId = SCENE_KAJIYA,
        .checks = {
            CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD, GET_CUR_UPG_VALUE(UPG_WALLET) >= 1),
            // TODO: Access to gold dust
            // CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD, HAS_ITEM(ITEM_GOLD_DUST) && RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD].obtained)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1),      ENTRANCE(MOUNTAIN_SMITHY, 0), true),
        },
    } },
    { RR_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_10YUKIYAMANOMURA,
        .checks = {
            CHECK(RC_MOUNTAIN_VILLAGE_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_SMITHY, 0),              ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1), true),
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 2), true),
            // TODO: When it's spring you need goron mask or zora mask instead?
            EXIT(ENTRANCE(GORON_GRAVERYARD, 0),             ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3), HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 0),             ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4), true),
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1),     ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6), true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE),
        },
        .oneWayEntrances = {
            ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8), // From Song of Soaring
        }
    } },
    { RR_MUSIC_BOX_HOUSE, RandoRegion{ .sceneId = SCENE_MUSICHOUSE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 2),                 ENTRANCE(MUSIC_BOX_HOUSE, 0), true),
        },
    } },
    { RR_PATH_TO_GORON_VILLAGE, RandoRegion{ .sceneId = SCENE_17SETUGEN,
        .checks = {
            CHECK(RC_TWIN_ISLANDS_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_TWIN_ISLANDS_TINGLE_MAP_2, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 2),  ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), true),
            EXIT(ENTRANCE(GORON_VILLAGE_WINTER, 0),     ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 1), true),
            EXIT(ENTRANCE(GORON_RACETRACK, 0),          ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 2), HAS_ITEM(ITEM_POWDER_KEG) && CAN_BE_GORON),
        },
    } },
    { RR_PATH_TO_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_13HUBUKINOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 3),                ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0), true),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6),      ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1), true),
        },
    } },
    { RR_PATH_TO_SNOWHEAD_LOWER, RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4),      ENTRANCE(PATH_TO_SNOWHEAD, 0), true),
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD, 0),                     ENTRANCE(PATH_TO_SNOWHEAD, 1), true),
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 3),              ENTRANCE(PINNACLE_ROCK, 0), true),
        },
    } },
    { RR_PIRATES_FORTRESS_CAPTAIN_ROOM_UPPER, RandoRegion{ .name = "Captain Room Upper", .sceneId = SCENE_PIRATE,
        // TODO : If NTSC JP 1.0 support is added we should add a connection here to RR_PIRATES_FORTRESS_CAPTAIN_ROOM and a unique flag to check for it...or ignore it and let the player go the long way around
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_BEEHIVE_1, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_BEEHIVE_2, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 2),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 1), true),
        },
        .events = {
            EVENT_WEEKEVENTREG("Hit Beehive", WEEKEVENTREG_83_02, HAS_ITEM(ITEM_BOW)),
        },
    } },
    { RR_PIRATES_FORTRESS_CAPTAIN_ROOM, RandoRegion{ .name = "Captain Room", .sceneId = SCENE_PIRATE,
        .checks = {
            // TODO: Zora Egg Here
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_HOOKSHOT, CHECK_WEEKEVENTREG(WEEKEVENTREG_83_02))
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 1),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 0), true),
        },
    } },
    { RR_PIRATES_FORTRESS_INSIDE_3_GUARD_ROOM, RandoRegion{ .name = "3 Guard Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SILVER_RUPEE_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 3),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 2), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_PURPLE_GUARD, true),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_CHEST_EGG_ROOM, RandoRegion{ .name = "Chest Egg Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_AQUARIUM, HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_1, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_2, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 8),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD, true),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_GREEN_GUARD, RandoRegion{ .name = "Green Guard Room", .sceneId = SCENE_PIRATE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 5),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD, CAN_USE_SWORD || CAN_BE_ZORA),
            CONNECTION(RR_PIRATES_FORTRESS_RIGHT_CLAM_EGG_ROOM, CAN_USE_SWORD || CAN_BE_ZORA),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_LINE_GUARD, RandoRegion{ .name = "Line Guard Room", .sceneId = SCENE_PIRATE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 7),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD, true),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD, RandoRegion{ .name = "Maze Room", .sceneId = SCENE_PIRATE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 5),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_GREEN_GUARD, true),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD, RandoRegion{ .name = "Orange Guard Room", .sceneId = SCENE_PIRATE,
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_LINE_GUARD, (CAN_USE_SWORD || CAN_BE_ZORA)),
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_CHEST_EGG_ROOM, (CAN_USE_SWORD || CAN_BE_ZORA)),
        }
    } },
    { RR_PIRATES_FORTRESS_INSIDE_PURPLE_GUARD, RandoRegion{ .name = "Purple Guard Room", .sceneId = SCENE_PIRATE,
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_3_GUARD_ROOM, (CAN_USE_SWORD || CAN_BE_ZORA)),
            CONNECTION(RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM, (CAN_USE_SWORD || CAN_BE_ZORA)),
        }
    } },
    { RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM, RandoRegion{ .name = "Left Clam Room", .sceneId = SCENE_PIRATE,
        .checks = {
            // Zora Egg Here
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_GUARDED_1, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_GUARDED_2, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 4),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 3), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM, true),
        }
    } },
    { RR_PIRATES_FORTRESS_LEFT_PLATFORM, RandoRegion{ .name = "Left Platform", .sceneId = SCENE_KAIZOKU,
        // The drop down from the LEFT_CLAM_EGG_ROOM
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 3),    ENTRANCE(PIRATES_FORTRESS, 4), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
        }
    } },
    { RR_PIRATES_FORTRESS_MOAT_HIGHER, RandoRegion{ .name = "Higher", .sceneId = SCENE_TORIDE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 0),             ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 1), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 10),   ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 6), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_LOWER, true),
        },
        .oneWayEntrances = {
            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 4), // From being captured in the inner part of Pirate Fortress
        },
    } },
    { RR_PIRATES_FORTRESS_MOAT_LOWER, RandoRegion{ .name = "Lower", .sceneId = SCENE_TORIDE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_1, CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_2, CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_3, CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 5),              ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 0), CAN_BE_ZORA),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 9),    ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 2), CAN_BE_ZORA && CAN_BE_GORON),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_HIGHER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .oneWayEntrances = {
            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3), // Two steams in "RR_PIRATES_FORTRESS_SEWERS_PREGATE" and "RR_PIRATES_FORTRESS_SEWERS_POSTGATE"
        }
    } },
    { RR_PIRATES_FORTRESS_MOAT_PLATFORM, RandoRegion{ .name = "Platform", .sceneId = SCENE_TORIDE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 12),            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 5), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_LOWER, true),
        },
    } },
    { RR_PIRATES_FORTRESS_PALAZA_LEFT_EXIT, RandoRegion{ .name = "Left Side Exit", .sceneId = SCENE_KAIZOKU,
        // The doorway when exiting the CHEST_EGG_ROOM, one way jump down to PALAZA
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7),    ENTRANCE(PIRATES_FORTRESS, 8), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA_LEFT_LOWER, RandoRegion{ .name = "Palaza Left Lower", .sceneId = SCENE_KAIZOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7),    ENTRANCE(PIRATES_FORTRESS, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA_LEFT_UPPER, HAS_ITEM(ITEM_HOOKSHOT))
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA_LEFT_UPPER, RandoRegion{ .name = "Palaza Left Upper", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_HEART_1, true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_HEART_2, true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_HEART_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 2),    ENTRANCE(PIRATES_FORTRESS, 3), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA_LEFT_LOWER, HAS_ITEM(ITEM_HOOKSHOT))
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA_RIGHT_EXIT, RandoRegion{ .name = "Right Side Exit", .sceneId = SCENE_KAIZOKU,
        // The doorway when exiting the RIGHT_CLAM_EGG_ROOM, one way jump down to PALAZA
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 5),    ENTRANCE(PIRATES_FORTRESS, 6), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA_RIGHT, RandoRegion{ .name = "Right Side", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_UPPER_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 5),    ENTRANCE(PIRATES_FORTRESS, 12), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4),    ENTRANCE(PIRATES_FORTRESS, 5), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA_TOWER, RandoRegion{ .name = "Palaza Tower", .sceneId = SCENE_KAIZOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 1),    ENTRANCE(PIRATES_FORTRESS, 2), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA, true),
        }
    } },
    { RR_PIRATES_FORTRESS_PALAZA, RandoRegion{ .name = "Palaza", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_LOWER_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 0),    ENTRANCE(PIRATES_FORTRESS, 1), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 1),    ENTRANCE(PIRATES_FORTRESS, 0), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA_LEFT_LOWER, HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA_RIGHT, HAS_ITEM(ITEM_HOOKSHOT)),
            // Outside of using Stones Mask you have to deal with this guard in some way.
            CONNECTION(RR_PIRATES_FORTRESS_PALAZA_TOWER, (
                (HAS_ITEM(ITEM_DEKU_NUT) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_MASK_STONE)) ||
                (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_ZORA
            )),
        }
    } },
    { RR_PIRATES_FORTRESS_RIGHT_CLAM_EGG_ROOM, RandoRegion{ .name = "Right Clam Room", .sceneId = SCENE_PIRATE,
        .checks = {
            // Zora Egg Here
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_1, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_2, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 6),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 5), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD, true),
        }
    } },
    { RR_PIRATES_FORTRESS_SEWERS_POSTGATE, RandoRegion{ .name = "Sewers Postgate", .sceneId = SCENE_PIRATE,
        .checks = {
            // SEWERS_POT_WATERWAY_1 incorrectly triggers INTERIOR_POT_GUARDED_1, commenting it out for now.
            //CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_WATERWAY_1, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_WATERWAY_2, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3),             ONE_WAY_EXIT, true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_TELESCOPE_ROOM, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)
        }
    } },
    { RR_PIRATES_FORTRESS_SEWERS_PREGATE, RandoRegion{ .name = "Sewers Pregate", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_1, CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_2, CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_3, CAN_BE_ZORA),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_HEART_PIECE_ROOM_1, true),
            // RC_PIRATE_FORTRESS_SEWERS_POT_HEART_PIECE_ROOM_2 seems to be missing, the pot next to the above pot.
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_HP, true)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 2),    ENTRANCE(PIRATES_FORTRESS_INTERIOR, 9), CAN_BE_ZORA),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3),             ONE_WAY_EXIT, true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_SEWERS_POSTGATE, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)
        }
    } },
    { RR_PIRATES_FORTRESS_TELESCOPE_ROOM, RandoRegion{ .name = "Telescope Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_END_1, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_END_2, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_POT_END_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 6),    ENTRANCE(PIRATES_FORTRESS_INTERIOR, 10), CAN_USE_PROJECTILE)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_SEWERS_POSTGATE, true)
        }
    } },
    { RR_POST_OFFICE, RandoRegion{ .sceneId = SCENE_POSTHOUSE,
        .checks = {
            // TODO : Add Heartpiece check
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 7),              ENTRANCE(POST_OFFICE, 0), true),
        },
    } },
    { RR_RANCH_BARN, RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 2),                 ENTRANCE(RANCH_HOUSE, 0), true),
        },
    } },
    { RR_RANCH_HOUSE, RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 3),                 ENTRANCE(RANCH_HOUSE, 1), true),
        },
    } },
    { RR_ROAD_TO_IKANA_ABOVE_LEDGE, RandoRegion{ .name = "Above Ledge", .sceneId = SCENE_IKANAMAE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 0),                 ENTRANCE(ROAD_TO_IKANA, 1), true),
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 0),              ENTRANCE(ROAD_TO_IKANA, 2), true)
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_FIELD_SIDE, CAN_RIDE_EPONA),
            CONNECTION(RR_ROAD_TO_IKANA_ABOVE_LEDGE, HAS_ITEM(ITEM_HOOKSHOT) && HAS_ITEM(ITEM_MASK_GARO)),
        },
    } },
    { RR_ROAD_TO_IKANA_FIELD_SIDE, RandoRegion{ .name = "Field Side", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ROAD_TO_IKANA_GROTTO, CAN_BE_GORON), // TODO: Grotto
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 4),                ENTRANCE(ROAD_TO_IKANA, 0), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, CAN_RIDE_EPONA),
        },
    } },
    { RR_ROAD_TO_SOUTHERN_SWAMP, RandoRegion{ .sceneId = SCENE_24KEMONOMITI,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_HP, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_2, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 1),                ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 0),      ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1), true),
            EXIT(ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),       ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2), true),
        },
        .events = {
            EVENT_RANDOINF("Access To Spring Water", RANDO_INF_HAS_ACCESS_TO_SPRING_WATER, true),
        },
    } },
    { RR_ROMANI_RANCH, RandoRegion{ .sceneId = SCENE_F01,
        .checks = {
            // TODO:
            // CHECK(RC_ROMANI_RANCH_EPONAS_SONG, CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG) && HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 1),                    ENTRANCE(ROMANI_RANCH, 0), true),
            EXIT(ENTRANCE(RANCH_HOUSE, 0),                  ENTRANCE(ROMANI_RANCH, 2), true), // Barn
            EXIT(ENTRANCE(RANCH_HOUSE, 1),                  ENTRANCE(ROMANI_RANCH, 3), true), // House
            EXIT(ENTRANCE(CUCCO_SHACK, 0),                  ENTRANCE(ROMANI_RANCH, 4), true),
            EXIT(ENTRANCE(DOGGY_RACETRACK, 0),              ENTRANCE(ROMANI_RANCH, 5), true),
        },
    } },
    { RR_SAKON_HIDEOUT, RandoRegion{ .sceneId = SCENE_SECOM,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 6),                 ENTRANCE(SAKONS_HIDEOUT, 0), true),
        },
    } },
    { RR_SECRET_SHRINE_ENTRANCE, RandoRegion{ .name = "Entrance", .sceneId = SCENE_RANDOM,
        .checks = {
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
    } },
    { RR_SECRET_SHRINE, RandoRegion{ .sceneId = SCENE_RANDOM,
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
    } },
    { RR_SNOWHEAD_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD, 2),                     ENTRANCE(FAIRY_FOUNTAIN, 2), true),
        },
    } },
    { RR_SNOWHEAD_NEAR_PATH, RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .checks = {
            CHECK(RC_SNOWHEAD_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 1),             ENTRANCE(SNOWHEAD, 0), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_TEMPLE, CAN_BE_GORON && CAN_PLAY_SONG(LULLABY)),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_SNOWHEAD),
        },
        .oneWayEntrances = {
            ENTRANCE(SNOWHEAD, 3), // From Song of Soaring
        }
    } },
    { RR_SNOWHEAD_NEAR_TEMPLE, RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD_TEMPLE, 0),              ENTRANCE(SNOWHEAD, 1), (
                // TODO: THIS IS TEMPORARY. Once snowhead is properly split up, this will be replaced with a proper logic check.
                CAN_BE_GORON && CAN_BE_DEKU &&
                HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_ITEM(ITEM_ARROW_FIRE) && 
                HAS_MAGIC && CAN_LIGHT_TORCH_NEAR_ANOTHER && CAN_USE_SWORD && CAN_USE_EXPLOSIVE
                // TODO: We can't really add requirement for key count, as the keys need to be in the pool
                // to be shuffled, and to be in the pool their vanilla location has to be accessible. Once
                // all key locations are logically accessible we can re-add this check.
                /* && KEY_COUNT(SNOWHEAD_TEMPLE) >= 3 */
            )),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 2),               ENTRANCE(SNOWHEAD, 2), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_PATH, true),
        },
    } },
    { RR_SNOWHEAD_TEMPLE_BOSS_ROOM, RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Goht)?
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_HC, HAS_ITEM(ITEM_BOW) || (CAN_BE_GORON && HAS_MAGIC)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_WARP, HAS_ITEM(ITEM_BOW) || (CAN_BE_GORON && HAS_MAGIC)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 7),               ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(GOHTS_LAIR, 0), // Snowhead Temple Boss Room
        },
    } },
    // TODO: Split this up into regions
    { RR_SNOWHEAD_TEMPLE, RandoRegion{ .sceneId = SCENE_HAKUGIN,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_LEDGE_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BLOCK_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_KEY, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_ALCOVE_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_LEDGE_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_COMPASS, true),
            CHECK(RC_SNOWHEAD_TEMPLE_FIRE_ARROW, true),
            CHECK(RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_ALCOVE_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_ICICLE_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_MAP_ALCOVE_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_MAP, true),
            CHECK(RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_CHEST, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_BRIDGE_PILLAR, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_BRIDGE_UNDER_PLATFORM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_COMPASS_ROOM_CRATE, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DUAL_SWITCHES, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_MAP_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_SNOW_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_10, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_6, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_7, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_8, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_9, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_6, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_7, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_6, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_5, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD, 1),                     ENTRANCE(SNOWHEAD_TEMPLE, 0), true),
            EXIT(ENTRANCE(GOHTS_LAIR, 0),                            ONE_WAY_EXIT, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_SNOWHEAD_TEMPLE)),
        },
    } },
    { RR_SOUTHERN_SWAMP_NORTH, RandoRegion{ .name = "North Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_HP, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), true),
            EXIT(ENTRANCE(TOURIST_INFORMATION, 0),          ENTRANCE(SOUTHERN_SWAMP_POISONED, 1), true),
            EXIT(ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 5), true),
            EXIT(ENTRANCE(WOODS_OF_MYSTERY, 0),             ENTRANCE(SOUTHERN_SWAMP_POISONED, 7), true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, (Flags_GetSceneSwitch(SCENE_20SICHITAI, 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE))),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP),
            EVENT_RANDOINF("Access To Spring Water", RANDO_INF_HAS_ACCESS_TO_SPRING_WATER, true),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 9), // From river in Ikana
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // From Song of Soaring
        }
    } },
    { RR_SOUTHERN_SWAMP_SOUTH, RandoRegion{ .name = "South Section", .sceneId = SCENE_20SICHITAI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 0),                     ENTRANCE(SOUTHERN_SWAMP_POISONED, 2), CAN_BE_DEKU || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
            EXIT(ENTRANCE(DEKU_PALACE, 0),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 3), true),
            EXIT(ENTRANCE(DEKU_PALACE, 5),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 4), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(SWAMP_SPIDER_HOUSE, 0),           ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_NORTH, (Flags_GetSceneSwitch(SCENE_20SICHITAI, 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE))),
        },
    } },
    { RR_STONE_TOWER_BOTTOM, RandoRegion{ .name = "Bottom", .sceneId = SCENE_F40,
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
    } },
    { RR_STONE_TOWER_INVERTED_LOWER, RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER, 1),                  ENTRANCE(STONE_TOWER_INVERTED, 0), HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_UPPER, CAN_GROW_BEAN_PLANT),
            CONNECTION(RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, true),
        },
    } },
    { RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, RandoRegion{ .sceneId = SCENE_F41,
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
            CONNECTION(RR_STONE_TOWER_BOTTOM, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_GORON && CAN_BE_ZORA),
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
    } },
    { RR_STONE_TOWER_TEMPLE_BOSS_ROOM, RandoRegion{ .sceneId = SCENE_INISIE_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Twinmold)?
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_HC, HAS_MAGIC),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP, HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 15),                         ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(TWINMOLDS_LAIR, 0), // Stone Tower Temple Pre Boss Room
        },
    } },
    { RR_STONE_TOWER_TEMPLE_INVERTED, RandoRegion{ .sceneId = SCENE_INISIE_R,
        .checks = {
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_HC, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_KEY, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_DEATH_ARMOS_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_EAST_LOWER_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_EAST_MIDDLE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_EAST_UPPER_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_GIANT_MASK, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_5, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_6, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_7, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_8, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_5, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 1),         ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0), true),
            EXIT(ENTRANCE(TWINMOLDS_LAIR, 0),                        ONE_WAY_EXIT, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_STONE_TOWER_TEMPLE)),
        },
    } },
    { RR_STONE_TOWER_TEMPLE, RandoRegion{ .sceneId = SCENE_INISIE_N,
        .checks = {
            CHECK(RC_STONE_TOWER_TEMPLE_BEFORE_WATER_BRIDGE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_CENTER_ACROSS_WATER_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_CENTER_SUN_BLOCK_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_COMPASS, true),
            CHECK(RC_STONE_TOWER_TEMPLE_ENTRANCE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_ENTRANCE_SWITCH_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_LIGHT_ARROW, true),
            CHECK(RC_STONE_TOWER_TEMPLE_MAP, true),
            CHECK(RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_CENTER_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_RIGHT_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_UNDER_WEST_GARDEN_LAVA_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_UNDER_WEST_GARDEN_LEDGE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_WATER_BRIDGE_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_WATER_SUN_SWITCH_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_WIND_ROOM_JAIL_CHEST, true),
            CHECK(RC_STONE_TOWER_TEMPLE_WIND_ROOM_LEDGE_CHEST, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_1, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_2, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_3, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_4, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_5, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_6, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_7, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_8, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_ENTRANCE_1, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_ENTRANCE_2, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_4, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_BRIDGE_1, true),
            // CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_BRIDGE_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_1, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_2, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_3, true),
            CHECK(RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER, 2),                  ENTRANCE(STONE_TOWER_TEMPLE, 0), true),
        },
    } },
    // TODO: Probably need to split this up to account for entrance rando later
    { RR_STONE_TOWER_TOP, RandoRegion{ .name = "Top", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_1, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_2, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_3, true),
            CHECK(RC_STONE_TOWER_POT_OWL_STATUE_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 0),         ENTRANCE(STONE_TOWER, 1), CAN_PLAY_SONG(ELEGY) && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE, 0),           ENTRANCE(STONE_TOWER, 2), (
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
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_STONE_TOWER),
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
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_STONE_TOWER_TOP, HAS_ITEM(ITEM_HOOKSHOT))
        },
    } },
    { RR_SWAMP_SHOOTING_GALLERY, RandoRegion{ .sceneId = SCENE_SYATEKI_MORI,
        .checks = {
            CHECK(RC_SWAMP_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_SWAMP_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2),       ENTRANCE(SWAMP_SHOOTING_GALLERY, 0), true),
        },
    } },
    { RR_SWAMP_SPIDER_HOUSE, RandoRegion{ .sceneId = SCENE_KINSTA1,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 8),      ENTRANCE(SWAMP_SPIDER_HOUSE, 0), true),
        },
    } },
    { RR_SWORDSMAN_SCHOOL, RandoRegion{ .sceneId = SCENE_DOUJOU,
        .checks = {
            CHECK(RC_SWORDSMAN_SCHOOL_HP, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_1, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_2, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_3, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_4, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_5, CAN_USE_HUMAN_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 3),              ENTRANCE(SWORDMANS_SCHOOL, 0), true),
        },
    } },
    { RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST, RandoRegion{ .name = "Before Great Bay Coast", .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 0),              ENTRANCE(TERMINA_FIELD, 2), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_PLAY_SONG(EPONA)),
        },
    } },
    { RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE, RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0),     ENTRANCE(TERMINA_FIELD, 3), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, HAS_ITEM(ITEM_BOW)),
        },
    } },
    { RR_TERMINA_FIELD, RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_TERMINA_FIELD_KAMARO, CAN_PLAY_SONG(HEALING)),
            CHECK(RC_TERMINA_FIELD_POT, CAN_GROW_BEAN_PLANT),
            CHECK(RC_TERMINA_FIELD_TALL_GRASS_CHEST, true),
            CHECK(RC_TERMINA_FIELD_TREE_STUMP_CHEST, CAN_GROW_BEAN_PLANT || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_TERMINA_FIELD_WATER_CHEST, CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 0),              ENTRANCE(TERMINA_FIELD, 0), true),
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0),       ENTRANCE(TERMINA_FIELD, 1), true),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 0),                ENTRANCE(TERMINA_FIELD, 4), true),
            EXIT(ENTRANCE(MILK_ROAD, 0),                    ENTRANCE(TERMINA_FIELD, 5), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 1),             ENTRANCE(TERMINA_FIELD, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 0),              ENTRANCE(TERMINA_FIELD, 7), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 0),             ENTRANCE(TERMINA_FIELD, 8), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE, HAS_ITEM(ITEM_BOW)),
            CONNECTION(RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST, CAN_PLAY_SONG(EPONA)),
            CONNECTION(RR_ASTRAL_OBSERVATORY_OUTSIDE, CAN_BE_DEKU)
        },
    } },
    { RR_TOURIST_INFORMATION, RandoRegion{ .sceneId = SCENE_MAP_SHOP,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 1),      ENTRANCE(TOURIST_INFORMATION, 0), true),
        },
        .events = {
            EVENT( // Killing Octorok blocking the southern swamp south section
                "Kill Octorok", 
                Flags_GetSceneSwitch(SCENE_20SICHITAI, 0x01), 
                Flags_SetSceneSwitch(SCENE_20SICHITAI, 0x01), 
                Flags_ClearSceneSwitch(SCENE_20SICHITAI, 0x01), 
                true // TODO: Conditions for starting swamp tour
            ),
        },
    } },
    { RR_TOWN_DEKU_PLAYGROUND, RandoRegion{ .sceneId = SCENE_DEKUTES,
        .checks = {
            // TODO : Add checks for getting best time on all three days/Freestanding items.
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 4),             ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), true),
        },
    } },
    { RR_TOWN_SHOOTING_GALLERY, RandoRegion{ .sceneId = SCENE_SYATEKI_MIZU,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 8),              ENTRANCE(TOWN_SHOOTING_GALLERY, 0), true),
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
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 5),              ENTRANCE(TRADING_POST, 0), true),
        
        },
    } },
    { RR_TREASURE_SHOP, RandoRegion{ .sceneId = SCENE_TAKARAYA,
        .checks = {
            // TODO : Add check for each form(minus FD)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 4),              ENTRANCE(TREASURE_CHEST_SHOP, 0), true),
        },
    } },
    { RR_WATERFALL_RAPIDS, RandoRegion{ .sceneId = SCENE_35TAKI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 4),                    ENTRANCE(WATERFALL_RAPIDS, 0), true),
        },
    } },
    { RR_WOODFALL_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Woodfall", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            // TODO: We can't add this till all stray fairies are in the pool
            // CHECK(RC_WOODFALL_GREAT_FAIRY, gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_WOODFALL_TEMPLE] >= 15),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 2),                     ENTRANCE(FAIRY_FOUNTAIN, 1), true),
        },
    } },
    { RR_WOODFALL_TEMPLE_BOSS_KEY_ROOM, RandoRegion{ .name = "Boss Key Room", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_BOSS_ROOM, RandoRegion{ .sceneId = SCENE_MITURIN_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Odolwa)?
            CHECK(RC_WOODFALL_TEMPLE_BOSS_CONTAINER, true),
            CHECK(RC_WOODFALL_TEMPLE_BOSS_WARP, true),
        },
        .oneWayEntrances = {
            ENTRANCE(ODOLWAS_LAIR, 0), // From Woodfall Temple Pre-Boss Room
        },
    } },
    { RR_WOODFALL_TEMPLE_BOW_ROOM, RandoRegion{ .name = "Bow Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DINOLFOS)?
            CHECK(RC_WOODFALL_TEMPLE_BOW, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, true),
        },
    } },
    { RR_WOODFALL_TEMPLE_COMPASS_ROOM, RandoRegion{ .name = "Compass Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DRAGONFLY)?
            CHECK(RC_WOODFALL_TEMPLE_COMPASS, CAN_BE_DEKU || CAN_USE_EXPLOSIVE),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAZE_ROOM, true),
        },
    } },
    { RR_WOODFALL_TEMPLE_DARK_ROOM, RandoRegion{ .name = "Dark Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(DARK_FOG_DUDE)?
            CHECK(RC_WOODFALL_TEMPLE_DARK_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, CAN_BE_DEKU && CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CONNECTION(RR_WOODFALL_TEMPLE_MAZE_ROOM, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    } },
    { RR_WOODFALL_TEMPLE_ENTRANCE, RandoRegion{ .name = "Entrance", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, RandoRegion{ .name = "Main Room Upper", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_MAIN_ROOM, RandoRegion{ .name = "Main Room", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_MAP_ROOM, RandoRegion{ .name = "Map Room", .sceneId = SCENE_MITURIN,
        .checks = {
            // CAN_KILL_ENEMY(SNAPPER)?
            CHECK(RC_WOODFALL_TEMPLE_MAP, CAN_BE_DEKU || CAN_USE_EXPLOSIVE),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_WATER_ROOM, true),
        },
    } },
    { RR_WOODFALL_TEMPLE_MAZE_ROOM, RandoRegion{ .name = "Maze Room", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_PRE_BOSS_ROOM, RandoRegion{ .name = "Pre Boss Room", .sceneId = SCENE_MITURIN,
        .checks = {
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_BOTTOM_RIGHT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_LEFT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_TOP_RIGHT, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_TEMPLE_POT_PRE_BOSS_1, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_POT_PRE_BOSS_2, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
            CHECK(RC_WOODFALL_TEMPLE_SF_PRE_BOSS_PILLAR, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ODOLWAS_LAIR, 0),                          ONE_WAY_EXIT, CAN_BE_DEKU && HAS_ITEM(ITEM_BOW) && CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_WOODFALL_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_TEMPLE_MAIN_ROOM_UPPER, true),
        },
    } },
    { RR_WOODFALL_TEMPLE_WATER_ROOM_UPPER, RandoRegion{ .name = "Water Room Upper", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL_TEMPLE_WATER_ROOM, RandoRegion{ .name = "Water Room", .sceneId = SCENE_MITURIN,
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
    } },
    { RR_WOODFALL, RandoRegion{ .sceneId = SCENE_21MITURINMAE,
        .checks = {
            CHECK(RC_WOODFALL_ENTRANCE_CHEST, CAN_BE_DEKU || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)),
            CHECK(RC_WOODFALL_HP_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_WOODFALL_NEAR_OWL_CHEST, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 2),      ENTRANCE(WOODFALL, 0), true),
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 0),              ENTRANCE(WOODFALL, 1), CAN_BE_DEKU && CAN_PLAY_SONG(SONATA)),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 1),               ENTRANCE(WOODFALL, 2), CAN_BE_DEKU),
        },
    } },
    { RR_WOODS_OF_MYSTERY, RandoRegion{ .sceneId = SCENE_26SARUNOMORI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 7),      ENTRANCE(WOODS_OF_MYSTERY, 0), true),
        },
    } },   
    { RR_ZORA_CAPE, RandoRegion{ .sceneId = SCENE_31MISAKI,
        .checks = {
            // TODO: Grottos
            CHECK(RC_ZORA_CAPE_GROTTO,                    CAN_USE_EXPLOSIVE || CAN_BE_GORON),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_1,             HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_DEKU),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_2,             HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ZORA_CAPE_UNDERWATER_CHEST,          CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_WATERFALL_HP,              CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_1,        true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_2,        true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_1,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_2,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_3,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_4,     true),
            CHECK(RC_ZORA_CAPE_OWL_STATUE,                CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 1),              ENTRANCE(ZORA_CAPE, 0), true),
            EXIT(ENTRANCE(ZORA_HALL, 0),                    ENTRANCE(ZORA_CAPE, 1), CAN_BE_ZORA),
            EXIT(ENTRANCE(ZORA_HALL,        1),             ENTRANCE(ZORA_CAPE, 2), true),
            EXIT(ENTRANCE(WATERFALL_RAPIDS, 0),             ENTRANCE(ZORA_CAPE, 4), HAS_ITEM(ITEM_HOOKSHOT)),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 3),               ENTRANCE(ZORA_CAPE, 5), HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_EXPLOSIVE),
            EXIT(ENTRANCE(GREAT_BAY_TEMPLE, 1),             ENTRANCE(ZORA_CAPE, 7), (
                // TODO: THIS IS TEMPORARY. Once great bay is properly split up, this will be replaced with a proper logic check.
                CAN_BE_ZORA && CAN_BE_DEKU &&
                HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_HOOKSHOT) && 
                HAS_MAGIC && CAN_LIGHT_TORCH_NEAR_ANOTHER && CAN_USE_SWORD && CAN_USE_EXPLOSIVE && CAN_PLAY_SONG(BOSSA_NOVA)
                // TODO: We can't really add requirement for key count, as the keys need to be in the pool
                // to be shuffled, and to be in the pool their vanilla location has to be accessible. Once
                // all key locations are logically accessible we can re-add this check.
                /* && KEY_COUNT(GREAT_BAY_TEMPLE) >= 1 */
            )),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_ZORA_CAPE),
        },
        .oneWayEntrances = {
            ENTRANCE(ZORA_CAPE, 6), // From Song of Soaring
            ENTRANCE(ZORA_CAPE, 9), // From Great Bay Temple Blue Warp
        },
    } },
    { RR_ZORA_HALL_EVANS_ROOM, RandoRegion{ .name = "Evan's Room", .sceneId = SCENE_BANDROOM,
        // TODO: Missing HP check to add here later.
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 4),                    ENTRANCE(ZORA_HALL_ROOMS, 3), true),
        },
    } },
    { RR_ZORA_HALL_JAPAS_ROOM, RandoRegion{ .name = "Japa's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 5),                    ENTRANCE(ZORA_HALL_ROOMS, 1), true),
        },
    } },
    { RR_ZORA_HALL_LULUS_ROOM, RandoRegion{ .name = "Lulu's Room", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_HALL_SCRUB_DEED,          Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON),
            CHECK(RC_ZORA_HALL_SCRUB_HP,            Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON && CAN_BE_DEKU),
            CHECK(RC_ZORA_HALL_SCRUB_POTION_REFILL, CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 3),                    ENTRANCE(ZORA_HALL_ROOMS, 2), true),
        },
    } },
    { RR_ZORA_HALL_MIKAUS_ROOM, RandoRegion{ .name = "Mikau's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 6),                    ENTRANCE(ZORA_HALL_ROOMS, 0), true),
        },
    } },
    { RR_ZORA_HALL_SHOP, RandoRegion{ .name = "Shop", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_SHOP_ITEM_1,              true),
            CHECK(RC_ZORA_SHOP_ITEM_2,              true),
            CHECK(RC_ZORA_SHOP_ITEM_3,              true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 2),                    ENTRANCE(ZORA_HALL_ROOMS, 5), true),
        },
    } },
    { RR_ZORA_HALL, RandoRegion{ .sceneId = SCENE_33ZORACITY,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 1),                    ENTRANCE(ZORA_HALL, 0), true),           
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 5),              ENTRANCE(ZORA_HALL, 2), true), // To Shop
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 2),              ENTRANCE(ZORA_HALL, 3), CAN_BE_ZORA), // To Lulu's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 3),              ENTRANCE(ZORA_HALL, 4), CAN_BE_ZORA), // To Evan's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 1),              ENTRANCE(ZORA_HALL, 5), CAN_BE_ZORA), // To Japas's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 0),              ENTRANCE(ZORA_HALL, 6), CAN_BE_ZORA), // To Mikaus's Room
        },
    } },
    { RR_MAX, RandoRegion{ .sceneId = SCENE_MAX,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),                      ONE_WAY_EXIT, true), // Save warp
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 9),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_CLOCK_TOWN)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 10),              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP)),
            EXIT(ENTRANCE(MILK_ROAD, 4),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MILK_ROAD)),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),               ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE)),
            EXIT(ENTRANCE(SNOWHEAD, 3),                              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SNOWHEAD)),
            EXIT(ENTRANCE(GREAT_BAY_COAST, 11),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_GREAT_BAY_COAST)),
            EXIT(ENTRANCE(ZORA_CAPE, 6),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_ZORA_CAPE)),
            EXIT(ENTRANCE(IKANA_CANYON, 4),                          ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_IKANA_CANYON)),
            EXIT(ENTRANCE(STONE_TOWER, 3),                           ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_STONE_TOWER)),
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
