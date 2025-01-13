#ifndef RANDO_LOGIC_H
#define RANDO_LOGIC_H

#include <libultraship/bridge.h>
#include "Rando/Rando.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

#include <unordered_map>
#include <set>

extern "C" {
#include "functions.h"
#include "variables.h"
}

namespace Rando {

namespace Logic {

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions);
RandoRegionId GetRegionIdFromEntrance(s32 entrance);
void ApplyFrenchVanillaLogicToSaveContext();
void ApplyGlitchlessLogicToSaveContext();
void ApplyNoLogicToSaveContext();

struct RandoEvent {
    std::string name;
    bool applyWhenAccessible;
    std::function<bool()> isApplied;
    std::function<void()> onApply;
    std::function<void()> onRemove;
    std::function<bool()> condition;
    std::string conditionString;
};

struct RandoRegionExit {
    s32 returnEntrance;
    std::function<bool()> condition;
    std::string conditionString;
};

struct RandoRegion {
    const char* name = "";
    SceneId sceneId;
    std::unordered_map<RandoCheckId, std::pair<std::function<bool()>, std::string>> checks;
    std::unordered_map<s32, RandoRegionExit> exits;
    std::unordered_map<RandoRegionId, std::pair<std::function<bool()>, std::string>> connections;
    std::vector<RandoEvent> events;
    std::set<s32> oneWayEntrances;
};

extern std::unordered_map<RandoRegionId, RandoRegion> Regions;

// TODO: This may not stay here
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
#define HAS_MOON_MASKS(target) (MoonMaskCount() >= target)
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
#define CAN_ACCESS(randoAccess) (RANDO_ACCESS[RANDO_ACCESS_##randoAccess])
#define CAN_GROW_BEAN_PLANT        \
    (HAS_ITEM(ITEM_MAGIC_BEANS) && \
     (CAN_PLAY_SONG(STORMS) || (HAS_BOTTLE && (CAN_ACCESS(SPRING_WATER) || CAN_ACCESS(HOT_SPRING_WATER)))))
#define CAN_USE_MAGIC_ARROW(arrowType) (HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_##arrowType) && HAS_MAGIC)
#define CAN_LIGHT_TORCH_NEAR_ANOTHER (HAS_ITEM(ITEM_DEKU_STICK) || CAN_USE_MAGIC_ARROW(FIRE))
#define KEY_COUNT(dungeon) (gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_##dungeon])
#define CAN_AFFORD(rc)                                                                                                \
    ((RANDO_SAVE_CHECKS[rc].price < 100) || (RANDO_SAVE_CHECKS[rc].price <= 200 && CUR_UPG_VALUE(UPG_WALLET) >= 1) || \
     (CUR_UPG_VALUE(UPG_WALLET) >= 2))
#define HAS_ALL_STRAY_FAIRIES(dungeonIndex) (gSaveContext.save.saveInfo.inventory.strayFairies[dungeonIndex] >= 15)

#define EVENT(name, isApplied, onApply, onRemove, condition)                                    \
    {                                                                                           \
        name, false, [] { return isApplied; }, [] { return onApply; }, [] { return onRemove; }, \
            [] { return condition; }, LogicString(#condition)                                   \
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
#define EVENT_OWL_WARP(owlId)                                                                            \
    {                                                                                                    \
        "Owl Statue", false, [] { return CAN_OWL_WARP(owlId); }, [] { SET_OWL_WARP(owlId); },            \
            [] { CLEAR_OWL_WARP(owlId); },                                                               \
            [] { return RANDO_SAVE_OPTIONS[RO_SHUFFLE_OWL_STATUES] == RO_GENERIC_NO && CAN_USE_SWORD; }, \
            "CAN_USE_SWORD"                                                                              \
    }
#define EVENT_WEEKEVENTREG(name, flag, condition)                                               \
    {                                                                                           \
        name, false, [] { return CHECK_WEEKEVENTREG(flag); }, [] { SET_WEEKEVENTREG(flag); },   \
            [] { CLEAR_WEEKEVENTREG(flag); }, [] { return condition; }, LogicString(#condition) \
    }
#define EVENT_RANDOINF(name, flag, condition)                                                    \
    {                                                                                            \
        name, false, [] { return Flags_GetRandoInf(flag); }, [] { Flags_SetRandoInf(flag); },    \
            [] { Flags_ClearRandoInf(flag); }, [] { return condition; }, LogicString(#condition) \
    }
#define EVENT_ACCESS(flag, condition)                                                                                \
    {                                                                                                                \
        convertEnumToReadableName(#flag), true, [] { return RANDO_ACCESS[flag] > 0; }, [] { RANDO_ACCESS[flag]++; }, \
            [] { RANDO_ACCESS[flag]--; }, [] { return condition; }, LogicString(#condition)                          \
    }
// TODO: This is for sure not the right place for these
inline void Flags_SetSceneSwitch(s32 scene, s32 flag) {
    if (((flag & ~0x1F) >> 5) == 0) {
        gSaveContext.cycleSceneFlags[scene].switch0 |= 1 << (flag & 0x1F);
    } else {
        gSaveContext.cycleSceneFlags[scene].switch1 |= 1 << (flag & 0x1F);
    }
}

inline void Flags_ClearSceneSwitch(s32 scene, s32 flag) {
    if (((flag & ~0x1F) >> 5) == 0) {
        gSaveContext.cycleSceneFlags[scene].switch0 &= ~(1 << (flag & 0x1F));
    } else {
        gSaveContext.cycleSceneFlags[scene].switch1 &= ~(1 << (flag & 0x1F));
    }
}

inline bool Flags_GetSceneSwitch(s32 scene, s32 flag) {
    if (gPlayState != NULL && gPlayState->sceneId == scene) {
        return gPlayState->actorCtx.sceneFlags.switches[(flag & ~0x1F) >> 5] & (1 << (flag & 0x1F));
    }

    if (((flag & ~0x1F) >> 5) == 0) {
        return gSaveContext.cycleSceneFlags[scene].switch0 & (1 << (flag & 0x1F));
    } else {
        return gSaveContext.cycleSceneFlags[scene].switch1 & (1 << (flag & 0x1F));
    }
}

inline bool Flags_GetSceneClear(s32 scene, s32 roomNumber) {
    if (gPlayState != NULL && gPlayState->sceneId == scene) {
        return (gPlayState->actorCtx.sceneFlags.clearedRoom & (1 << roomNumber));
    }

    return (gSaveContext.cycleSceneFlags[scene].clearedRoom & (1 << roomNumber));
}

inline void Flags_SetSceneClear(s32 scene, s32 roomNumber) {
    gSaveContext.cycleSceneFlags[scene].clearedRoom |= (1 << roomNumber);
}

inline void Flags_UnsetSceneClear(s32 scene, s32 roomNumber) {
    gSaveContext.cycleSceneFlags[scene].clearedRoom &= ~(1 << roomNumber);
}

inline std::string LogicString(std::string condition) {
    if (condition == "true")
        return "";

    return condition;
}

inline uint32_t MoonMaskCount() {
    uint32_t count = 0;
    for (int i = ITEM_MASK_TRUTH; i <= ITEM_MASK_GIANT; i++) {
        if (INV_CONTENT(i) == i) {
            count++;
        }
    }
    return count;
}

inline bool CanKillEnemy(ActorId EnemyId) {
    switch (EnemyId) {
        case ACTOR_BOSS_01: // Odolwa
            return (CAN_USE_SWORD || CAN_BE_GORON || CAN_BE_ZORA || CAN_USE_EXPLOSIVE || CAN_USE_MAGIC_ARROW(FIRE) ||
                    CAN_USE_MAGIC_ARROW(LIGHT));
        case ACTOR_BOSS_02: // Twinmold
            return (HAS_ITEM(ITEM_BOW) || (HAS_ITEM(ITEM_MASK_GIANT) && HAS_MAGIC && CAN_USE_HUMAN_SWORD));
        case ACTOR_BOSS_03: // Gyorg
            return ((CAN_BE_DEITY && HAS_MAGIC) || (CAN_BE_ZORA && HAS_MAGIC));
        case ACTOR_BOSS_04: // Wart
            return (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA);
        case ACTOR_BOSS_HAKUGIN: // Goht
            return (CAN_USE_MAGIC_ARROW(FIRE));
        case ACTOR_EN_KNIGHT: // Igos du Ikana/IdI Lackey
            return (CAN_USE_MAGIC_ARROW(FIRE) &&
                    (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) >= EQUIP_VALUE_SHIELD_MIRROR) &&
                    (CAN_USE_HUMAN_SWORD || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA));
        case ACTOR_EN_KAIZOKU: // Fighter Pirate
            return (CAN_USE_SWORD || CAN_BE_ZORA);
        case ACTOR_EN_PAMETFROG: // Swamp Gekko
            return (HAS_ITEM(ITEM_BOW) && (CAN_BE_DEKU || CAN_USE_EXPLOSIVE || CAN_BE_GORON));
        case ACTOR_EN_SW: // Gold Skulltula
            return (CAN_USE_PROJECTILE || CAN_BE_DEKU || CAN_BE_GORON || CAN_USE_HUMAN_SWORD || CAN_USE_EXPLOSIVE);
        case ACTOR_EN_DINOFOS: // Dinofos
            return (CAN_USE_SWORD || CAN_BE_GORON || HAS_ITEM(ITEM_BOW) || (CAN_BE_DEKU && HAS_MAGIC));
        case ACTOR_EN_WIZ: // Wizrobe
            return (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_SWORD || CAN_BE_GORON);
        case ACTOR_EN_WF: // Wolfos
            return (CAN_USE_HUMAN_SWORD || (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_GORON || CAN_BE_ZORA);
        case ACTOR_EN_JSO2: // Garo Master
            return (HAS_ITEM(ITEM_BOW) || CAN_BE_GORON || CAN_USE_SWORD);
        case ACTOR_EN_IK: // Iron Knuckle
            return (CAN_USE_HUMAN_SWORD || CAN_BE_GORON);
        case ACTOR_EN_GRASSHOPPER: // Dragonfly
            return ((CAN_BE_DEKU && HAS_MAGIC) || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_NUT) || CAN_USE_SWORD ||
                    CAN_BE_ZORA);
        case ACTOR_EN_MKK: // Boe
            return ((CAN_BE_DEKU && HAS_MAGIC) || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_NUT) || CAN_USE_SWORD ||
                    CAN_BE_ZORA || CAN_BE_GORON);
        case ACTOR_EN_BIGPAMET: // Snapper
            return (CAN_BE_DEKU || CAN_USE_EXPLOSIVE || CAN_BE_GORON);
        case ACTOR_EN_ST: // Large Skulltula
            return (CAN_USE_SWORD || CAN_USE_PROJECTILE || CAN_BE_GORON || CAN_USE_EXPLOSIVE);
        case ACTOR_EN_BAT: // Bat Bat
            return (CAN_USE_SWORD || HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_BOW) || CAN_USE_EXPLOSIVE ||
                    CAN_BE_GORON || CAN_BE_ZORA);
        case ACTOR_EN_DEKUBABA: // Neck bending Deku Baba
            return (CAN_USE_HUMAN_SWORD || CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA || HAS_ITEM(ITEM_BOW) ||
                    CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK));
        default: // Incorrect actor ID inputed.
            assert(false);
            return false;
    }
}

} // namespace Logic

} // namespace Rando

#endif // RANDO_LOGIC_H
