#include <libultraship/bridge/consolevariablebridge.h>
#include "EnemyRando.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Rando/Rando.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "overlays/actors/ovl_En_Slime/z_en_slime.h"
#include "overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "overlays/actors/ovl_En_Ik/z_en_ik.h"
#include "overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "overlays/actors/ovl_En_Rat/z_en_rat.h"
#include "overlays/actors/ovl_En_Rd/z_en_rd.h"
#include "overlays/actors/ovl_En_Snowman/z_en_snowman.h"
#include "overlays/actors/ovl_En_Karebaba/z_en_karebaba.h"
#include "overlays/actors/ovl_En_Pp/z_en_pp.h"
#include "overlays/actors/ovl_En_Rr/z_en_rr.h"
#include "overlays/actors/ovl_En_Jso2/z_en_jso2.h"
#include "overlays/actors/ovl_En_Bigpo/z_en_bigpo.h"
#include "overlays/actors/ovl_En_Wallmas/z_en_wallmas.h"
#include "overlays/actors/ovl_En_Famos/z_en_famos.h"
#include "overlays/actors/ovl_En_Bee/z_en_bee.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_En_Dekunuts/z_en_dekunuts.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"
}

// Actor entries pack flags into the high bits of their id.
static constexpr s16 ACTOR_ID_MASK = 0x1FFF;

// ===== Enemy pool =====

enum EnemySpawnFlags {
    // Respawns or splits, so it never leaves the enemy list for good.
    ENEMY_SPAWN_RENEWABLE = 1 << 0,
    // Belongs at the vanilla placement's height; everything else drops to the floor beneath it.
    ENEMY_SPAWN_AIRBORNE = 1 << 1,
    // Only joins ACTORCAT_ENEMY once the player comes near, so a room can read as clear too early.
    ENEMY_SPAWN_DORMANT = 1 << 2,
    // Leaves a bottleable soul behind, so its death does not free the spot right away. A soul also
    // turns up where a Stray Fairy was expected.
    ENEMY_SPAWN_LEAVES_SOUL = 1 << 3,
    // Listed in the menu but out of the pool until ticked on, for the ones that make a spot a nuisance.
    ENEMY_SPAWN_OFF_BY_DEFAULT = 1 << 4,
    // Unusable wherever something waits on this exact enemy dying.
    ENEMY_SPAWN_UNRELIABLE_DEATH = ENEMY_SPAWN_RENEWABLE | ENEMY_SPAWN_DORMANT,
};

struct EnemySpawnEntry {
    s16 actorId;
    s16 params;
    const char* name;
    u8 flags = 0;
};

static const std::vector<EnemySpawnEntry> sEnemyPool = {
    { ACTOR_EN_FIREFLY, KEESE_FIRE_FLY, "Fire Keese", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_FIREFLY, KEESE_NORMAL_FLY, "Keese", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_FIREFLY, KEESE_ICE_FLY, "Ice Keese", ENEMY_SPAWN_AIRBORNE },

    { ACTOR_EN_SLIME, EN_SLIME_TYPE_BLUE, "Blue ChuChu", ENEMY_SPAWN_RENEWABLE },
    { ACTOR_EN_SLIME, EN_SLIME_TYPE_GREEN, "Green ChuChu", ENEMY_SPAWN_RENEWABLE },
    { ACTOR_EN_SLIME, EN_SLIME_TYPE_YELLOW, "Yellow ChuChu", ENEMY_SPAWN_RENEWABLE },
    { ACTOR_EN_SLIME, EN_SLIME_TYPE_RED, "Red ChuChu", ENEMY_SPAWN_RENEWABLE },

    { ACTOR_EN_TITE, ENTITE_MINUS_0, "Tektite" },
    { ACTOR_EN_TITE, ENTITE_MINUS_2, "Blue Tektite" },

    { ACTOR_EN_IK, IK_TYPE_SILVER, "Silver Iron Knuckle" },
    { ACTOR_EN_IK, IK_TYPE_BLACK, "Black Iron Knuckle" },
    { ACTOR_EN_IK, IK_TYPE_WHITE, "White Iron Knuckle" },

    { ACTOR_EN_WF, 0, "Wolfos" },
    { ACTOR_EN_WF, 1, "White Wolfos" },

    { ACTOR_EN_DEKUBABA, DEKUBABA_NORMAL, "Deku Baba" },
    { ACTOR_EN_DEKUBABA, DEKUBABA_BIG, "Big Deku Baba" },

    { ACTOR_EN_RAT, EN_RAT_TYPE_DUNGEON, "Real Bombchu", ENEMY_SPAWN_RENEWABLE },
    { ACTOR_EN_RAT, static_cast<s16>(0x8000), "Real Bombchu (Overworld)", ENEMY_SPAWN_RENEWABLE },

    { ACTOR_EN_RD, EN_RD_TYPE_GIBDO, "Gibdo" },
    { ACTOR_EN_RD, EN_RD_TYPE_REGULAR, "ReDead" },

    { ACTOR_EN_SNOWMAN, EN_SNOWMAN_TYPE_SMALL, "Eeno" },
    { ACTOR_EN_SNOWMAN, EN_SNOWMAN_TYPE_LARGE, "Large Eeno" },

    { ACTOR_EN_MKK, 0, "Black Boe" },
    { ACTOR_EN_MKK, 1, "White Boe" },

    { ACTOR_EN_KAREBABA, ENKAREBABA_1, "Wilted Deku Baba" },
    { ACTOR_EN_KAREBABA, KAREBABA_MINI, "Mini Baba", ENEMY_SPAWN_RENEWABLE },

    { ACTOR_EN_NEO_REEBA, 0, "Leever" },
    { ACTOR_EN_NEO_REEBA, static_cast<s16>(0x8000), "Large Leever" },

    { ACTOR_EN_PP, EN_PP_TYPE_MASKED, "Hiploop (Masked)" },
    { ACTOR_EN_PP, EN_PP_TYPE_NO_MASK, "Hiploop (No Mask)" },
    { ACTOR_EN_PP, EN_PP_TYPE_UNMASKED, "Hiploop (Unmasked)" },

    { ACTOR_EN_PEEHAT, 0, "Peahat", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_PEEHAT, 1, "Peahat Larva", ENEMY_SPAWN_AIRBORNE },

    { ACTOR_EN_DODONGO, 0, "Dodongo" },
    { ACTOR_EN_DODONGO, 1, "Large Dodongo" },

    { ACTOR_EN_RR, LIKE_LIKE_PARAM_1, "Like Like" },
    { ACTOR_EN_RR, LIKE_LIKE_PARAM_3, "Giant Like Like" },

    { ACTOR_EN_JSO2, EN_JSO2_TYPE_NORMAL, "Garo Master", ENEMY_SPAWN_OFF_BY_DEFAULT },

    { ACTOR_EN_BIGPO, BIG_POE_TYPE_REGULAR, "Big Poe", ENEMY_SPAWN_AIRBORNE | ENEMY_SPAWN_LEAVES_SOUL },

    { ACTOR_EN_AM, 0, "Armos" },
    { ACTOR_EN_BAGUO, 0, "Nejiron" },
    { ACTOR_EN_BAT, 0, "Bad Bat", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_BB, 0, "Blue Bubble", ENEMY_SPAWN_RENEWABLE | ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_BBFALL, 0, "Red Bubble", ENEMY_SPAWN_AIRBORNE | ENEMY_SPAWN_OFF_BY_DEFAULT },
    { ACTOR_EN_BEE, BEE_BEHAVIOR_ATTACK, "Giant Bee", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_CROW, 0, "Guay", ENEMY_SPAWN_RENEWABLE | ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_DEKUNUTS, 0, "Mad Scrub" },
    { ACTOR_EN_DINOFOS, 0, "Dinolfos" },
    { ACTOR_EN_FAMOS, FAMOS_PATH_INDEX_NONE, "Death Armos" },
    { ACTOR_EN_FLOORMAS, 0, "Floormaster" },
    { ACTOR_EN_FZ, 0, "Freezard" },
    { ACTOR_EN_GRASSHOPPER, 0, "Dragonfly", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_KAME, 0, "Snapper" },
    { ACTOR_EN_POH, 0, "Poe", ENEMY_SPAWN_AIRBORNE | ENEMY_SPAWN_LEAVES_SOUL },
    { ACTOR_EN_SKB, 0, "Stalchild" },
    { ACTOR_EN_VM, 0, "Beamos" },
    { ACTOR_EN_WALLMAS, WALLMASTER_TYPE_TIMER_ONLY, "Wallmaster", ENEMY_SPAWN_OFF_BY_DEFAULT },
    { ACTOR_EN_WDHAND, 0, "Dexihand", ENEMY_SPAWN_OFF_BY_DEFAULT },
    { ACTOR_EN_ST, 0, "Large Skulltula", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_THIEFBIRD, 0, "Takkuri", ENEMY_SPAWN_AIRBORNE },
    { ACTOR_EN_TUBO_TRAP, 0, "Flying Pot Trap", ENEMY_SPAWN_DORMANT },

    // Deliberately absent: Skullwalltula (would replace gold skulltulas), Desbreko and Shellblade
    // (not damageable), Octorok (needs water-safe spot tagging and the Great Bay Temple ones excluded).
};

static const std::unordered_set<s16> sRandomizableActorIds = [] {
    std::unordered_set<s16> ids;
    for (const EnemySpawnEntry& entry : sEnemyPool) {
        ids.insert(entry.actorId);
    }
    return ids;
}();

// Named after the entry rather than its index, so reordering the list keeps each setting.
static const std::vector<std::string> sEnemyPoolCVars = [] {
    std::vector<std::string> cVars;
    for (const EnemySpawnEntry& entry : sEnemyPool) {
        cVars.push_back("gModes.EnemyRando.Pool." + Ship_RemoveSpecialCharacters(entry.name));
    }
    return cVars;
}();

static bool IsInPoolByDefault(size_t index) {
    return !(sEnemyPool[index].flags & ENEMY_SPAWN_OFF_BY_DEFAULT);
}

static bool IsInPool(size_t index) {
    return CVarGetInteger(sEnemyPoolCVars[index].c_str(), IsInPoolByDefault(index)) != 0;
}

static void SetWholePool(bool inPool) {
    for (const std::string& cVar : sEnemyPoolCVars) {
        CVarSetInteger(cVar.c_str(), inPool);
    }
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void EnemyRando_DrawPoolSelector() {
    if (UIWidgets::Button("Enable All", UIWidgets::ButtonOptions({ { .tooltip = "Add every enemy to the pool" } })
                                            .Size(UIWidgets::Sizes::Inline)
                                            .Color(UIWidgets::Colors::Green))) {
        SetWholePool(true);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Disable All", UIWidgets::ButtonOptions({ { .tooltip = "Remove every enemy from the pool" } })
                                             .Size(UIWidgets::Sizes::Inline)
                                             .Color(UIWidgets::Colors::Red))) {
        SetWholePool(false);
    }

    ImGui::BeginChild("EnemyRandoPool", ImVec2(0.0f, ImGui::GetFrameHeightWithSpacing() * 10.0f),
                      ImGuiChildFlags_Borders);
    for (size_t i = 0; i < sEnemyPool.size(); i++) {
        UIWidgets::CVarCheckbox(sEnemyPool[i].name, sEnemyPoolCVars[i].c_str(),
                                UIWidgets::CheckboxOptions().DefaultValue(IsInPoolByDefault(i)));
    }
    ImGui::EndChild();
}

// ===== Per-placement exceptions =====

// Actor IDs a placement will not take, keyed by (scene, room, actor list index), for enemies that
// break that specific vanilla spot (e.g. no room for a Large Skulltula to hang).
static const std::map<std::tuple<s16, s8, s16>, std::vector<s16>> sBlacklistedActorsByLocation = {
    // This Skulltula hangs right above a chest, and the Deku Flower a Mad Scrub leaves behind when it
    // dies would sit on top of it.
    { { SCENE_KAKUSIANA, 5, 3 }, { ACTOR_EN_DEKUNUTS } },
};

// Placements left vanilla, for enemies a room is built around rather than fought. Freeze the Great Bay
// Temple ChuChu with an ice arrow and it becomes the platform up to the next floor.
static const std::set<std::tuple<s16, s8, s16>> sExcludedSpots = {
    { SCENE_SEA, 2, 0 },
};

// ===== Room rules =====

// Computed once per room load instead of per spawn.
struct RoomEnemyRules {
    bool killGated = false;
    std::set<s16> strayFairyHostIndices;
    std::set<s16> inTreeIndices;
    // A room only stocks the objects its own actor list asks for, so a replacement finds no slot, and
    // neither does anything it spawns later like an Eeno's snowball. Everything else stays suppressed.
    std::unordered_set<s16> bypassedObjectIds;
};

static std::map<std::pair<s16, s8>, RoomEnemyRules> sRoomRules;

static const RoomEnemyRules* FindRoomRules(s16 sceneId, s8 room) {
    auto it = sRoomRules.find({ sceneId, room });
    return it != sRoomRules.end() ? &it->second : NULL;
}

// Braces at a macro call site would read as extra macro arguments, so hook bodies go through this.
static RoomEnemyRules& GetRoomRules(s16 sceneId, s8 room) {
    return sRoomRules[{ sceneId, room }];
}

// Killing a room's last ACTORCAT_ENEMY actor only sets the temporary clear flag; these are the actors
// that promote it to the permanent one with Flags_SetClear, which is what everything else waits on.
static bool IsRoomClearPromoter(s16 actorId) {
    switch (actorId) {
        case ACTOR_DOOR_SHUTTER:
        case ACTOR_OBJ_ROOMTIMER:
        case ACTOR_BG_IKANA_SHUTTER:
            return true;
        default:
            return false;
    }
}

static bool IsRoomClearChest(ActorEntry* entry) {
    return ((entry->id & ACTOR_ID_MASK) == ACTOR_EN_BOX) && ((ENBOX_GET_TYPE(entry) == ENBOX_TYPE_BIG_ROOM_CLEAR) ||
                                                             (ENBOX_GET_TYPE(entry) == ENBOX_TYPE_SMALL_ROOM_CLEAR));
}

// Shutters are transition actors, so the room list alone misses them. Only an effective
// DOORSHUTTER_TYPE_1 installs the room-clear action, and func_808A0900 collapses a declared type 1 to
// type 0 on the front side, while types 6 and 7 collapse into type 1 there.
static bool TransitionPromotesRoomClear(TransitionActorEntry* entry, s8 room) {
    // Actor_SpawnTransitionActors has already run and negated the id of everything it spawned.
    s16 actorId = ABS_ALT(entry->id) & ACTOR_ID_MASK;

    if (actorId == ACTOR_DOOR_SHUTTER) {
        switch (DOORSHUTTER_PARAMS_GET_TYPE(entry->params)) {
            case DOORSHUTTER_TYPE_1:
                return entry->sides[1].room == room;
            case DOORSHUTTER_TYPE_6:
            case DOORSHUTTER_TYPE_7:
                return entry->sides[0].room == room;
            default:
                return false;
        }
    }

    return IsRoomClearPromoter(actorId) && ((entry->sides[0].room == room) || (entry->sides[1].room == room));
}

static bool ComputeIsRoomKillGated(s8 room) {
    ActorEntry* entries = gPlayState->setupActorList;

    if (entries != NULL) {
        for (s32 i = 0; i < gPlayState->numSetupActors; i++) {
            if (IsRoomClearPromoter(entries[i].id & ACTOR_ID_MASK) || IsRoomClearChest(&entries[i])) {
                return true;
            }
        }
    }

    TransitionActorEntry* transitions = gPlayState->transitionActors.list;

    if (transitions != NULL) {
        for (s32 i = 0; i < gPlayState->transitionActors.count; i++) {
            if (TransitionPromotesRoomClear(&transitions[i], room)) {
                return true;
            }
        }
    }

    return false;
}

// Vanilla ties two placements together by giving them the same coordinates, so this matches a spot by
// what shares its position. It skips the actor that supplied the positions.
static std::set<s16> ComputeSpotsSharingPosition(s16 markerActorId, bool matchY,
                                                 bool (*markerFilter)(ActorEntry*) = NULL) {
    std::set<s16> spots;
    ActorEntry* entries = gPlayState->setupActorList;

    if (entries == NULL) {
        return spots;
    }

    std::vector<Vec3s> positions;
    for (s32 i = 0; i < gPlayState->numSetupActors; i++) {
        if (((entries[i].id & ACTOR_ID_MASK) == markerActorId) &&
            ((markerFilter == NULL) || markerFilter(&entries[i]))) {
            positions.push_back(entries[i].pos);
        }
    }

    for (s32 i = 0; i < gPlayState->numSetupActors; i++) {
        if ((entries[i].id & ACTOR_ID_MASK) == markerActorId) {
            continue;
        }

        for (const Vec3s& position : positions) {
            if ((entries[i].pos.x == position.x) && (entries[i].pos.z == position.z) &&
                (!matchY || (entries[i].pos.y == position.y))) {
                spots.insert(i);
                break;
            }
        }
    }

    return spots;
}

// ===== Picking a replacement =====

// A single Ship_Random draw over the eligible entries, so seeded mode keeps picking the same enemy for
// a spot. Returns NULL when no entry is eligible, which leaves the vanilla enemy in place.
static const EnemySpawnEntry* PickReplacement(const RoomEnemyRules& rules, s16 sceneId, s8 room, s16 actorListIndex,
                                              bool needsReliableDeath) {
    auto blacklist = sBlacklistedActorsByLocation.find({ sceneId, room, actorListIndex });
    bool holdsStrayFairy = rules.strayFairyHostIndices.count(actorListIndex) != 0;

    u8 bannedFlags = 0;
    if (needsReliableDeath || holdsStrayFairy || rules.killGated) {
        bannedFlags |= ENEMY_SPAWN_UNRELIABLE_DEATH;
    }
    if (holdsStrayFairy) {
        bannedFlags |= ENEMY_SPAWN_LEAVES_SOUL;
    }

    std::vector<const EnemySpawnEntry*> eligible;
    eligible.reserve(sEnemyPool.size());

    for (size_t i = 0; i < sEnemyPool.size(); i++) {
        const EnemySpawnEntry& entry = sEnemyPool[i];
        bool blacklisted =
            (blacklist != sBlacklistedActorsByLocation.end()) &&
            (std::find(blacklist->second.begin(), blacklist->second.end(), entry.actorId) != blacklist->second.end());

        if ((entry.flags & bannedFlags) || blacklisted || !IsInPool(i)) {
            continue;
        }

        eligible.push_back(&entry);
    }

    if (eligible.empty()) {
        return NULL;
    }

    return eligible[Ship_Random(0, static_cast<s32>(eligible.size()))];
}

static void EnemyRando_InitRandomSeed(Actor* actor, s16 actorListIndex) {
    uint64_t baseSeed =
        IS_RANDO ? gSaveContext.save.shipSaveInfo.rando.finalSeed : gSaveContext.save.shipSaveInfo.fileCreatedAt;
    uint64_t seed = baseSeed + (static_cast<uint64_t>(gPlayState->sceneId) * 7919) +
                    (static_cast<uint64_t>(actor->room) * 104729) +
                    (static_cast<uint64_t>(static_cast<u16>(actorListIndex)) * 1299709) +
                    (static_cast<uint64_t>(actor->id) * 15485867);
    Ship_Random_Seed(seed);
}

// ===== Spawning the replacement =====

// Starts slightly above the spot, so an enemy already sitting flush on the floor, or a little inside
// it, still finds it. Same as EnKusa_SnapToFloor.
static f32 FloorYAt(f32 x, f32 y, f32 z) {
    CollisionPoly* poly;
    Vec3f rayStart = { x, y + 30.0f, z };

    return BgCheck_EntityRaycastFloor1(&gPlayState->colCtx, &poly, &rayStart);
}

// Straight down from an in-tree spot is the middle of the trunk, so ground enemies step aside first.
// 60 units clears the ~18 unit trunk, and the floor-height test keeps them off ledges and rock faces.
static void EnemyRando_StepOutOfTree(Vec3f* pos) {
    f32 treeFloorY = FloorYAt(pos->x, pos->y, pos->z);
    s16 startAngle = Ship_Random(0, 0x10000);

    for (s32 i = 0; i < 8; i++) {
        s16 angle = startAngle + (i * (0x10000 / 8));
        f32 x = pos->x + (Math_SinS(angle) * 60.0f);
        f32 z = pos->z + (Math_CosS(angle) * 60.0f);

        f32 floorY = FloorYAt(x, pos->y, z);
        if ((floorY > BGCHECK_Y_MIN) && (fabsf(floorY - treeFloorY) < 100.0f)) {
            pos->x = x;
            pos->z = z;
            return;
        }
    }
}

// A Keese's spot sits well off the ground, and most ground enemies never apply gravity of their own,
// so without this they hover there forever.
static void EnemyRando_SnapSpawnPosToFloor(Vec3f* pos) {
    f32 floorY = FloorYAt(pos->x, pos->y, pos->z);

    if (floorY > BGCHECK_Y_MIN) {
        pos->y = floorY;
    }
}

// Not every scene-placed instance of a pool actor is something to swap. Both of these actors mask
// params down to the low byte in their Init, which is where their type lives.
static bool ShouldSkipAtSpawn(Actor* actor) {
    switch (actor->id) {
        case ACTOR_EN_DEKUNUTS:
            // The Deku Shrine's scrubs. Type 1 clears the collider's material and marks it
            // ACELEM_NO_DAMAGE, so nothing can kill it. It only ducks in and out of its flower.
            return (actor->params & 0xFF) == ENDEKUNUTS_GET_FF00_1;
        case ACTOR_EN_BIGPO:
            // Dampe's grave hides Big Poes as the flames he digs up and summons one more for the fight
            // at the end of it (see VB_BIG_POE_APPEAR_AFTER_FLAME_HUNT). This only swaps the well poe.
            return (actor->params & 0xFF) != BIG_POE_TYPE_REGULAR;
        default:
            return false;
    }
}

// Returns the spawned actor, or NULL if nothing was eligible. `needsReliableDeath` is for spots where
// something waits on this exact enemy dying rather than on the room as a whole.
static Actor* EnemyRando_SpawnReplacement(Actor* actor, s16 actorListIndex, bool needsReliableDeath) {
    if (ENEMY_RANDO_MODE == ENEMY_RANDO_RANDOM_SEEDED) {
        EnemyRando_InitRandomSeed(actor, actorListIndex);
    }

    RoomEnemyRules& rules = GetRoomRules(gPlayState->sceneId, actor->room);
    const EnemySpawnEntry* replacement =
        PickReplacement(rules, gPlayState->sceneId, actor->room, actorListIndex, needsReliableDeath);
    if (replacement == NULL) {
        return NULL;
    }

    Vec3f spawnPos = actor->world.pos;

    if (!(replacement->flags & ENEMY_SPAWN_AIRBORNE)) {
        if (rules.inTreeIndices.count(actorListIndex) != 0) {
            EnemyRando_StepOutOfTree(&spawnPos);
        }
        EnemyRando_SnapSpawnPosToFloor(&spawnPos);
    }

    SPDLOG_DEBUG("EnemyRando: replacing {} with {} ({} room {})", GetActorDescription(actor->id), replacement->name,
                 Ship_GetSceneName(gPlayState->sceneId), actor->room);

    rules.bypassedObjectIds.insert(gActorOverlayTable[replacement->actorId].profile->objectId);
    // A ChuChu frozen by an ice arrow builds its ice block out of a separate object.
    if (replacement->actorId == ACTOR_EN_SLIME) {
        rules.bypassedObjectIds.insert(OBJECT_ICE_BLOCK);
    }

    // Only the vanilla spot's facing carries over. Its pitch and roll belong to whatever was placed
    // there, and EnRd for one skips its ground check unless shape.rot.x is 0, falling through the world.
    return Actor_Spawn(&gPlayState->actorCtx, gPlayState, replacement->actorId, spawnPos.x, spawnPos.y, spawnPos.z, 0,
                       actor->shape.rot.y, 0, replacement->params);
}

// ===== Dampe's flame hunt =====

static HOOK_ID sSummonedPoeDeathHookId = 0;

// Stands a randomized enemy in for the Big Poe that Dampe's flame hunt would have summoned. Returns
// false if nothing was eligible, so the poe appears as usual.
static bool EnemyRando_TakeOverFlameHuntFight(EnBigpo* poe) {
    Actor* replacement = EnemyRando_SpawnReplacement(&poe->actor, GetActorListIndex(&poe->actor), true);

    if (replacement == NULL) {
        return false;
    }

    // The bottle chest waits on the switch flag the poe would have set as it died, so hand that to
    // whatever fights in its place.
    if (sSummonedPoeDeathHookId != 0) {
        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorKill>(sSummonedPoeDeathHookId);
    }

    sSummonedPoeDeathHookId = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorKill>(
        (uintptr_t)replacement, [switchFlag = poe->switchFlag](Actor*) {
            if (switchFlag != BIG_POE_SWITCH_FLAG_NONE) {
                Flags_SetSwitch(gPlayState, switchFlag);
            }

            // GameInteractor defers unregistering, so calling it from inside the hook is fine, and it
            // stops a recycled actor slot at this address from tripping the hook again.
            GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorKill>(sSummonedPoeDeathHookId);
            sSummonedPoeDeathHookId = 0;
        });

    // Without this the poe keeps counting flames and asking again every frame, and the revealed flames
    // burn on forever.
    for (Actor* fire = poe->actor.child; fire != NULL; fire = fire->child) {
        Actor_Kill(fire);
    }
    Actor_Kill(&poe->actor);

    return true;
}

// ===== Registration =====

void RegisterEnemyRando() {
    COND_VB_SHOULD(VB_ENABLE_OBJECT_DEPENDENCY, ENEMY_RANDO_MODE, {
        ObjectId objectId = (ObjectId)va_arg(args, int);
        const RoomEnemyRules* curRules = FindRoomRules(gPlayState->sceneId, gPlayState->roomCtx.curRoom.num);
        const RoomEnemyRules* prevRules = FindRoomRules(gPlayState->sceneId, gPlayState->roomCtx.prevRoom.num);

        if (((curRules != NULL) && (curRules->bypassedObjectIds.count(objectId) != 0)) ||
            ((prevRules != NULL) && (prevRules->bypassedObjectIds.count(objectId) != 0))) {
            *should = false;
        }
    });

    // The freeze only ever checked XZ distance. Harmless where vanilla puts ReDeads, but the randomizer
    // can put one a floor below, freezing you through it. Bounded to its own 150 unit radius.
    COND_VB_SHOULD(VB_REDEAD_FREEZE_PLAYER, ENEMY_RANDO_MODE, {
        EnRd* redead = va_arg(args, EnRd*);

        if (fabsf(redead->actor.playerHeightRel) > 150.0f) {
            *should = false;
        }
    });

    // Match on XZ only, since the replacement may have dropped to the floor. A pending init means a
    // vanilla enemy ShouldActorInit is about to swap out, which would free the fairy early.
    COND_VB_SHOULD(VB_STRAY_FAIRY_IS_HELD_BY_ENEMY, ENEMY_RANDO_MODE, {
        Actor* fairy = va_arg(args, Actor*);
        Actor* enemy = va_arg(args, Actor*);

        *should = (enemy->init == NULL) && (enemy->home.pos.x == fairy->home.pos.x) &&
                  (enemy->home.pos.z == fairy->home.pos.z);
    });

    COND_HOOK(AfterRoomSceneCommands, ENEMY_RANDO_MODE, [](s16 sceneId, s8 roomNum) {
        RoomEnemyRules& rules = GetRoomRules(sceneId, roomNum);

        rules.bypassedObjectIds.clear();
        rules.killGated = ComputeIsRoomKillGated(roomNum);
        // Vanilla pairs a trapped Stray Fairy with its host by identical coordinates, and only that
        // exact actor's death frees it (see EnElforg_GetHoldingEnemy).
        rules.strayFairyHostIndices = ComputeSpotsSharingPosition(
            ACTOR_EN_ELFORG, true, [](ActorEntry* entry) { return STRAY_FAIRY_TYPE(entry) == STRAY_FAIRY_TYPE_ENEMY; });
        // Boes hide inside the snow trees on the way to Mountain Village, at the tree's exact XZ a
        // couple hundred units up in the canopy.
        rules.inTreeIndices = ComputeSpotsSharingPosition(ACTOR_EN_SNOWWD, false);
    });

    COND_HOOK(ShouldActorInit, ENEMY_RANDO_MODE, [](Actor* actor, bool* should) {
        if (!sRandomizableActorIds.count(actor->id) || ShouldSkipAtSpawn(actor)) {
            return;
        }

        // GetActorListIndex returns -1 for anything spawned dynamically rather than placed in the
        // room's static actor list, which is the only thing with a fixed spot to randomize.
        s16 actorListIndex = GetActorListIndex(actor);
        if ((actorListIndex < 0) || (sExcludedSpots.count({ gPlayState->sceneId, actor->room, actorListIndex }) != 0)) {
            return;
        }

        // With nothing left in the pool for this spot, the vanilla enemy stays. Emptying the room
        // instead would leave anything gated on clearing it unopenable.
        if (EnemyRando_SpawnReplacement(actor, actorListIndex, false) == NULL) {
            return;
        }

        *should = false;

        // Mad Scrub always drops a bouncy Deku Flower on death. Spawn it immediately instead of
        // relying on the replacement to ever die.
        if (actor->id == ACTOR_EN_DEKUNUTS) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_OBJ_ETCETERA, actor->home.pos.x, actor->home.pos.y,
                        actor->home.pos.z, 0, actor->home.rot.y, 0,
                        DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE));
        }
    });

    COND_VB_SHOULD(VB_BIG_POE_APPEAR_AFTER_FLAME_HUNT, ENEMY_RANDO_MODE, {
        EnBigpo* poe = va_arg(args, EnBigpo*);

        *should = !EnemyRando_TakeOverFlameHuntFight(poe);
    });
}

static RegisterShipInitFunc initFunc(RegisterEnemyRando, { CVAR_ENEMY_RANDO_MODE });
