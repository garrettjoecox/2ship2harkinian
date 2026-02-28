#include "ArchipelagoBridge.h"
#include "ArchipelagoConsoleWindow.h"
#include "Archipelago.h"
#include <libultraship/libultraship.h>
#include <queue>
#include <string>
#include <utility>
#include <cstring>

#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>

#include "Rando/Rando.h"
#include "Rando/StaticData/StaticData.h"
#include "Rando/Types.h"
#include "Rando/MiscBehavior/Traps.h"
#include "2s2h/BenGui/Notification.h"
#include "CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

namespace ArchipelagoBridge {

static std::queue<PendingItem> sPendingItems;
static std::queue<PendingDeathLink> sPendingDeaths;
static std::unordered_set<int64_t> sProcessedItemIndices;
static std::unordered_set<int64_t> sSeenItemIndices; // queued or processed this session
static nlohmann::json sCachedLocationInfo;           // Cache location data to reapply after file creation
static nlohmann::json sCachedSlotData;               // Cache slot_data to reapply after file creation
static std::set<int64_t> sCachedCheckedLocations;    // Cache server-checked locations for new save files

// Store custom text for Archipelago items: RandoCheckId -> (playerName, itemName)
static std::unordered_map<RandoCheckId, std::pair<std::string, std::string>> sArchipelagoItemText;

// Metadata for Archipelago items from other players (for GameInteractor events)
struct APItemMetadata {
    RandoItemId randoItemId;
    std::string playerName;
    std::string displayItemName;
    bool fromOtherPlayer;
};
static std::unordered_map<s16, APItemMetadata> sAPItemMetadata;
static s16 sNextAPItemId = 1; // Counter for unique IDs

// Include generated mapping from RandoCheckId to AP Location ID
#include "APLocationMapping.h"

static bool sSaveReadyForArchiInit = false;

uint64_t GetLocationIdFromRandoCheck(RandoCheckId rc) {
    static const auto& mapping = GetAPLocationIdMap();
    auto it = mapping.find(rc);
    if (it != mapping.end()) {
        return it->second;
    }
    SPDLOG_WARN("[AP][Bridge] No AP location ID mapping for RandoCheckId {}", (int)rc);
    return 0; // Return 0 for unmapped locations
}

// Reverse lookup: AP Location ID -> RandoCheckId
RandoCheckId GetRandoCheckFromLocationId(uint64_t apLocationId) {
    static const auto& mapping = GetAPLocationIdMap();

    // Build reverse map on first call
    static std::unordered_map<uint64_t, RandoCheckId> reverseMap;
    static bool initialized = false;
    if (!initialized) {
        for (const auto& [rc, apLoc] : mapping) {
            reverseMap[apLoc] = rc;
        }
        initialized = true;
    }

    auto it = reverseMap.find(apLocationId);
    if (it != reverseMap.end()) {
        return it->second;
    }

    SPDLOG_WARN("[AP][Bridge] No RandoCheckId mapping for AP location {}", apLocationId);
    return RC_UNKNOWN;
}

RandoItemId GetRandoItemIdFromAPItemId(uint64_t apItemId, const std::string& itemName, uint32_t flags) {
    // Mode 1: AP item ID is already a RandoItemId
    if (CVarGetInteger("gArchipelago.ItemIdIsRandoItemId", 0)) {
        return static_cast<RandoItemId>(apItemId);
    }

    // Special-case: both triforce RIs share the same name; always prefer the canonical one.
    if (itemName == "Piece of the Triforce") {
        return RI_TRIFORCE_PIECE;
    }

    // Mode 2: resolve by AP display name -> Rando::StaticData::Items[*].name
    if (!itemName.empty() && itemName != "Unknown") {
        for (const auto& [id, staticItem] : Rando::StaticData::Items) {
            if (staticItem.name == itemName) {
                return id;
            }
        }
    }

    // Mode 3: Item is from another game - return Archipelago item based on flags
    // AP item flags: 0b001 = progression, 0b010 = useful, 0b100 = trap
    if (flags & 0x1) { // Progression
        return RI_ARCHIPELAGO_PROGRESSIVE;
    } else if (flags & 0x2) { // Useful
        return RI_ARCHIPELAGO_USEFUL;
    } else { // Junk/Filler
        return RI_ARCHIPELAGO_JUNK;
    }
}

// Shared implementation used by both PopulateLocationRewards and RepopulateLocationRewardsFromCache.
static void PopulateLocationRewardsImpl(const nlohmann::json& locationInfo) {
    // Clear all shuffled flags and reset items to vanilla first, so only checks in this seed are
    // marked as shuffled. Unshuffled checks (e.g. boss remains when shuffle_boss_remains is off)
    // will keep their vanilla item rather than defaulting to junk.
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        if (randoCheckId != RC_UNKNOWN) {
            RANDO_SAVE_CHECKS[randoCheckId].shuffled = false;
            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
        }
    }

    const int localPlayer = Archipelago::GetPlayerNumber();

    for (auto& [apLocStr, itemData] : locationInfo.items()) {
        try {
            uint64_t apLocationId = std::stoull(apLocStr);

            RandoCheckId rc = GetRandoCheckFromLocationId(apLocationId);
            if (rc == RC_UNKNOWN) {
                SPDLOG_WARN("[AP][Bridge] Could not find RandoCheckId for AP location {}", apLocationId);
                continue;
            }

            uint64_t apItemId = itemData["item"];
            int playerOwner = itemData["player"];
            uint32_t flags = itemData.value("flags", 0);
            std::string itemName = itemData.value("item_name", "");

            RandoItemId randoItemId;

            // If the item belongs to another player, do NOT resolve by name (Mode 2).
            // Name-based resolution would incorrectly match remote items like "Progressive Wallet"
            // to real 2ship items. Force Mode 3 (flag-based classification) instead.
            if (playerOwner != localPlayer && playerOwner >= 0 && localPlayer >= 0) {
                randoItemId = GetRandoItemIdFromAPItemId(apItemId, "", flags);
            } else {
                randoItemId = GetRandoItemIdFromAPItemId(apItemId, itemName, flags);
            }

            if (randoItemId == RI_UNKNOWN) {
                SPDLOG_WARN("[AP][Bridge] Location {} -> unknown item (AP item {} '{}')", apLocationId, apItemId,
                            itemName);
                continue;
            }

            // Populate RANDO_SAVE_CHECKS with the AP item.
            // Note: prices are set separately via ApplySlotOptions from slot_data.
            auto& saveCheck = RANDO_SAVE_CHECKS[rc];
            saveCheck.randoItemId = randoItemId;
            saveCheck.shuffled = true;

            // For Archipelago placeholder items (from other players/games), store display text.
            if (randoItemId == RI_ARCHIPELAGO_PROGRESSIVE || randoItemId == RI_ARCHIPELAGO_USEFUL ||
                randoItemId == RI_ARCHIPELAGO_JUNK) {

                std::string displayItemName = itemName;
                if (displayItemName == "Unknown" || displayItemName.empty()) {
                    displayItemName = Archipelago::GetItemName(apItemId, "2 Ship 2 Harkinian (MM)");
                    if (displayItemName == "Unknown" || displayItemName.empty()) {
                        displayItemName = Archipelago::GetItemName(apItemId, Archipelago::GetPlayerGame(playerOwner));
                    }
                }

                if (playerOwner != localPlayer && playerOwner >= 0 && localPlayer >= 0) {
                    sArchipelagoItemText[rc] = { Archipelago::GetPlayerAlias(playerOwner), displayItemName };
                } else {
                    sArchipelagoItemText[rc] = { "", displayItemName };
                }
            }

        } catch (const std::exception& e) {
            SPDLOG_ERROR("[AP][Bridge] Error processing location {}: {}", apLocStr, e.what());
        }
    }

    // Mark starting item checks as eligible so they auto-collect on game start.
    if (!RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].obtained &&
        !RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].eligible) {
        RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].eligible = true;
    }
    if (!RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].obtained &&
        !RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].eligible) {
        RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].eligible = true;
    }
}

void PopulateLocationRewards(const nlohmann::json& locationInfo) {
    if (!locationInfo.is_object()) {
        SPDLOG_ERROR("[AP][Bridge] PopulateLocationRewards: locationInfo is not an object");
        return;
    }

    // Cache the location data so we can reapply it after file creation.
    sCachedLocationInfo = locationInfo;

    PopulateLocationRewardsImpl(locationInfo);
}

void RepopulateLocationRewardsFromCache() {
    if (sCachedLocationInfo.is_null() || !sCachedLocationInfo.is_object()) {
        SPDLOG_WARN("[AP][Bridge] RepopulateLocationRewardsFromCache: No cached location data");
        return;
    }

    PopulateLocationRewardsImpl(sCachedLocationInfo);
}

void CacheCheckedLocations(const std::set<int64_t>& checkedLocations) {
    sCachedCheckedLocations = checkedLocations;

    // If we're already in an Archipelago save and ready, apply immediately.
    if (sSaveReadyForArchiInit && IS_ARCHI) {
        ApplyCachedCheckedLocations();
    }
}

void ApplyCachedCheckedLocations() {
    if (sCachedCheckedLocations.empty()) {
        return;
    }

    for (int64_t apLocId : sCachedCheckedLocations) {
        // Convert AP location ID to RandoCheckId
        RandoCheckId rc = GetRandoCheckFromLocationId(static_cast<uint64_t>(apLocId));

        if (rc != RC_UNKNOWN) {
            auto& saveCheck = RANDO_SAVE_CHECKS[rc];

            // Mark as checked in our save file
            if (!IsLocationChecked(static_cast<uint64_t>(apLocId))) {
                MarkLocationChecked(static_cast<uint64_t>(apLocId));
            }

            // Mark as obtained so the item doesn't spawn
            if (!saveCheck.obtained) {
                saveCheck.obtained = true;
                saveCheck.cycleObtained = true;
                saveCheck.eligible = false; // Don't queue for giving (already collected on server)
            }
        }
    }
}

// Map AP option names (from Options.py) to RandoOptionId enum values
static std::unordered_map<std::string, RandoOptionId> GetAPOptionMapping() {
    static const std::unordered_map<std::string, RandoOptionId> mapping = {
        { "access_dungeons", RO_ACCESS_DUNGEONS },
        { "access_majora_masks_count", RO_ACCESS_MAJORA_MASKS_COUNT },
        { "access_majora_remains_count", RO_ACCESS_MAJORA_REMAINS_COUNT },
        { "access_moon_masks_count", RO_ACCESS_MOON_MASKS_COUNT },
        { "access_moon_remains_count", RO_ACCESS_MOON_REMAINS_COUNT },
        { "access_trials", RO_ACCESS_TRIALS },
        { "clock_shuffle_progressive", RO_CLOCK_SHUFFLE_PROGRESSIVE },
        { "clock_shuffle", RO_CLOCK_SHUFFLE },
        { "clock_terminal_time", RO_CLOCK_TERMINAL_TIME },
        { "exclude_termina_field_grass", RO_EXCLUDE_TERMINA_FIELD_GRASS },
        { "exclude_cow_grotto_grass", RO_EXCLUDE_COW_GROTTO_GRASS },
        { "hints_boss_remains", RO_HINTS_BOSS_REMAINS },
        { "hints_gossip_stones", RO_HINTS_GOSSIP_STONES },
        { "hints_hookshot", RO_HINTS_HOOKSHOT },
        { "hints_oath_to_order", RO_HINTS_OATH_TO_ORDER },
        { "hints_purchaseable", RO_HINTS_PURCHASEABLE },
        { "hints_song_of_soaring", RO_HINTS_SONG_OF_SOARING },
        { "hints_spider_houses", RO_HINTS_SPIDER_HOUSES },
        { "logic", RO_LOGIC },
        { "plentiful_items", RO_PLENTIFUL_ITEMS },
        { "shuffle_barrel_drops", RO_SHUFFLE_BARREL_DROPS },
        { "shuffle_boss_remains", RO_SHUFFLE_BOSS_REMAINS },
        { "shuffle_boss_souls", RO_SHUFFLE_BOSS_SOULS },
        { "shuffle_cows", RO_SHUFFLE_COWS },
        { "shuffle_crate_drops", RO_SHUFFLE_CRATE_DROPS },
        { "shuffle_enemy_drops", RO_SHUFFLE_ENEMY_DROPS },
        { "shuffle_enemy_souls", RO_SHUFFLE_ENEMY_SOULS },
        { "shuffle_freestanding_items", RO_SHUFFLE_FREESTANDING_ITEMS },
        { "shuffle_frogs", RO_SHUFFLE_FROGS },
        { "shuffle_gold_skulltulas", RO_SHUFFLE_GOLD_SKULLTULAS },
        { "shuffle_grass_drops", RO_SHUFFLE_GRASS_DROPS },
        { "shuffle_ocarina_buttons", RO_SHUFFLE_OCARINA_BUTTONS },
        { "shuffle_ocarina", RO_SHUFFLE_OCARINA },
        { "shuffle_owl_statues", RO_SHUFFLE_OWL_STATUES },
        { "shuffle_pot_drops", RO_SHUFFLE_POT_DROPS },
        { "shuffle_shield", RO_SHUFFLE_SHIELD },
        { "shuffle_shops", RO_SHUFFLE_SHOPS },
        { "shuffle_snowball_drops", RO_SHUFFLE_SNOWBALL_DROPS },
        { "shuffle_song_double_time", RO_SHUFFLE_SONG_DOUBLE_TIME },
        { "shuffle_song_inverted_time", RO_SHUFFLE_SONG_INVERTED_TIME },
        { "shuffle_song_saria", RO_SHUFFLE_SONG_SARIA },
        { "shuffle_song_sun", RO_SHUFFLE_SONG_SUN },
        { "shuffle_song_time", RO_SHUFFLE_SONG_TIME },
        { "shuffle_sword", RO_SHUFFLE_SWORD },
        { "shuffle_swim", RO_SHUFFLE_SWIM },
        { "shuffle_tingle_shops", RO_SHUFFLE_TINGLE_SHOPS },
        { "shuffle_traps", RO_SHUFFLE_TRAPS },
        { "shuffle_tree_drops", RO_SHUFFLE_TREE_DROPS },
        { "shuffle_triforce_pieces", RO_SHUFFLE_TRIFORCE_PIECES },
        { "skulltula_tokens_max", RO_SKULLTULA_TOKENS_MAX },
        { "skulltula_tokens_required", RO_SKULLTULA_TOKENS_REQUIRED },
        { "starting_bunny_hood", RO_STARTING_BUNNY_HOOD },
        { "starting_consumables", RO_STARTING_CONSUMABLES },
        { "starting_health", RO_STARTING_HEALTH },
        { "starting_maps_and_compasses", RO_STARTING_MAPS_AND_COMPASSES },
        { "starting_rupees", RO_STARTING_RUPEES },
        { "stray_fairies_max", RO_STRAY_FAIRIES_MAX },
        { "stray_fairies_required", RO_STRAY_FAIRIES_REQUIRED },
        { "trap_amount", RO_TRAP_AMOUNT },
        { "triforce_pieces_max", RO_TRIFORCE_PIECES_MAX },
        { "triforce_pieces_required", RO_TRIFORCE_PIECES_REQUIRED },
    };
    return mapping;
}

static bool IsArchiSaveValidNoInit() {
    if (!IS_ARCHI) {
        return false;
    }
    const auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;
    return archi.magic == ARCHI_SAVE_MAGIC && archi.version == ARCHI_SAVE_VERSION;
}

void ApplySlotOptions(const nlohmann::json& slotData) {
    if (!slotData.is_object()) {
        SPDLOG_ERROR("[AP][Bridge] ApplySlotOptions: slotData is not an object");
        return;
    }

    // Cache slot_data for later use (e.g., when loading a file that was created before connecting)
    sCachedSlotData = slotData;

    const auto& optionMapping = GetAPOptionMapping();
    for (const auto& [apOptionName, randoOptionId] : optionMapping) {
        if (slotData.contains(apOptionName)) {
            try {
                uint32_t value = slotData[apOptionName];
                RANDO_SAVE_OPTIONS[randoOptionId] = value;
            } catch (const std::exception& e) {
                SPDLOG_ERROR("[AP][Bridge] Error applying option {}: {}", apOptionName, e.what());
            }
        }
    }

    // Apply shop prices from slot_data
    if (slotData.contains("shop_prices") && slotData["shop_prices"].is_object()) {
        for (auto& [locationName, price] : slotData["shop_prices"].items()) {
            try {
                // Find the RandoCheckId for this location name
                RandoCheckId foundCheck = RC_UNKNOWN;
                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.name == locationName) {
                        foundCheck = randoCheckId;
                        break;
                    }
                }

                if (foundCheck != RC_UNKNOWN) {
                    int priceValue = price.get<int>();
                    RANDO_SAVE_CHECKS[foundCheck].price = priceValue;
                } else {
                    SPDLOG_WARN("[AP][Bridge] Could not find RandoCheckId for shop location: {}", locationName);
                }
            } catch (const std::exception& e) {
                SPDLOG_ERROR("[AP][Bridge] Error applying price for {}: {}", locationName, e.what());
            }
        }
    }

    // Grant computed starting items ONCE, as soon as slot_data arrives.
    // OnFileLoad may run before we have slot_data, so we must also handle the "slot_data arrives later" case here.
    if (sSaveReadyForArchiInit && IS_ARCHI && IsArchiSaveValidNoInit()) {

        auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;

        if (!archi.startingItemsGranted) {
            Rando::GrantStartingItems();
            archi.startingItemsGranted = 1;
        }
    }
}

bool ReapplySlotOptionsFromCache() {
    if (sCachedSlotData.is_null() || !sCachedSlotData.is_object()) {
        return false;
    }

    ApplySlotOptions(sCachedSlotData);
    return true;
}

bool IsInGame() {
    return gPlayState != nullptr;
}

static void EnsureArchiSaveInitialized() {
    if (!IS_ARCHI) {
        return;
    }

    // Critical: do not reset during boot/connection phase before the save is loaded.
    if (!sSaveReadyForArchiInit) {
        auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;
        SPDLOG_WARN("[AP][Bridge] EnsureArchiSaveInitialized called before save-ready. "
                    "Skipping init. magic={:#x} version={} receivedItemCount={}",
                    archi.magic, archi.version, archi.receivedItemCount);
        return;
    }

    auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;

    if (archi.magic != ARCHI_SAVE_MAGIC || archi.version != ARCHI_SAVE_VERSION) {
        SPDLOG_WARN("[AP][Bridge] RESETTING Archi save data due to magic/version mismatch!");
        memset(&archi, 0, sizeof(archi));
        archi.magic = ARCHI_SAVE_MAGIC;
        archi.version = ARCHI_SAVE_VERSION;
        archi.serverHost[0] = '\0';
        archi.slotName[0] = '\0';
        archi.receivedItemCount = 0;
        archi.checkedLocationCount = 0;
        memset(archi.checkedLocations, 0, sizeof(archi.checkedLocations));
    }
}

void EnqueueItem(uint64_t itemId, int fromPlayer, int64_t index, uint32_t flags, const std::string& itemName) {
    // Persisted skip: do not queue historical items if the save has already processed past them.
    if (index >= 0) {
        if (!sSaveReadyForArchiInit) {
            return; // Don't enqueue items before save is loaded - they'll be resynced in OnFileLoad
        } else if (IsArchiSaveValidNoInit()) {
            uint32_t currentReceivedCount = gSaveContext.save.shipSaveInfo.rando.archipelago.receivedItemCount;
            if ((uint32_t)index < currentReceivedCount) {
                return;
            }
        }
    }

    // Session-only dedupe: prevent duplicate queue spam within one session/reconnect cycle.
    if (index >= 0) {
        const auto [it, inserted] = sSeenItemIndices.insert(index);
        if (!inserted) {
            return;
        }
    }

    PendingItem p;
    p.itemId = itemId;
    p.fromPlayer = fromPlayer;
    p.index = index;
    p.flags = flags;
    p.itemName = itemName;
    sPendingItems.push(p);
}

static bool IsArchiSaveActive() {
    if (!sSaveReadyForArchiInit) {
        return false;
    }
    return IsArchiSaveValidNoInit();
}

static bool GetCheckedBit(uint64_t locationId) {
    if (!IsArchiSaveActive()) {
        return false;
    }
    if (locationId >= (uint64_t)RC_MAX) {
        return false;
    }
    const uint64_t byteIndex = locationId / 8;
    const uint8_t bitMask = (uint8_t)(1u << (locationId % 8));
    return (gSaveContext.save.shipSaveInfo.rando.archipelago.checkedLocations[byteIndex] & bitMask) != 0;
}

static void SetCheckedBit(uint64_t locationId) {
    if (!IsArchiSaveActive()) {
        return;
    }
    if (locationId >= (uint64_t)RC_MAX) {
        return;
    }
    const uint64_t byteIndex = locationId / 8;
    const uint8_t bitMask = (uint8_t)(1u << (locationId % 8));
    uint8_t& b = gSaveContext.save.shipSaveInfo.rando.archipelago.checkedLocations[byteIndex];
    if ((b & bitMask) == 0) {
        b |= bitMask;
        gSaveContext.save.shipSaveInfo.rando.archipelago.checkedLocationCount++;
    }
}

bool IsLocationChecked(uint64_t locationId) {
    if (GetCheckedBit(locationId)) {
        return true;
    }
    // AP location IDs are larger than RC_MAX so GetCheckedBit always returns false for them.
    // Fall back to checking the in-memory cache that MarkLocationChecked maintains.
    return sCachedCheckedLocations.count(static_cast<int64_t>(locationId)) > 0;
}

void MarkLocationChecked(uint64_t locationId) {
    SetCheckedBit(locationId);

    // Keep sCachedCheckedLocations updated so subsequent save loads also mark this location
    // as obtained. Without this, locations checked after the initial connection snapshot
    // (from get_checked_locations() at connect time) are invisible to ApplyCachedCheckedLocations()
    // when a different save file is loaded later in the same session.
    sCachedCheckedLocations.insert(static_cast<int64_t>(locationId));

    // Immediately mark the corresponding rando check as obtained in the current save.
    // This handles the case where the server's location_checked_handler fires for a location
    // that was checked from a different save file during this session.
    if (sSaveReadyForArchiInit && IS_ARCHI) {
        RandoCheckId rc = GetRandoCheckFromLocationId(locationId);
        if (rc != RC_UNKNOWN) {
            auto& saveCheck = RANDO_SAVE_CHECKS[rc];
            if (!saveCheck.obtained) {
                saveCheck.obtained = true;
                saveCheck.cycleObtained = true;
                saveCheck.eligible = false;
            }
        }
    }
}

void EnqueueDeathLink(const std::string& source, const std::string& cause) {
    sPendingDeaths.push(PendingDeathLink{ source, cause });
}

static bool TryResolveRandoItemIdByName(const std::string& name, RandoItemId& out) {
    if (name.empty()) {
        return false;
    }

    // Special-case: both RI_TRIFORCE_PIECE and RI_TRIFORCE_PIECE_PREVIOUS share the same display name.
    // Always resolve the AP item name to the canonical RI_TRIFORCE_PIECE.
    if (name == "Piece of the Triforce") {
        out = RI_TRIFORCE_PIECE;
        return true;
    }

    for (const auto& [id, staticItem] : Rando::StaticData::Items) {
        if (staticItem.name == name) {
            out = id;
            return true;
        }
    }
    return false;
}

// The external max is stored per save-slot so that loading an older slot doesn't prevent
// it from receiving items that were earned on a different slot in the same session.
// Key format: "gArchipelago.MaxReceivedItemCount.{fileNum}"
static std::string GetExternalMaxCVarKey() {
    return std::string("gArchipelago.MaxReceivedItemCount.") + std::to_string(gSaveContext.fileNum);
}

static uint32_t GetMaxReceivedItemCountExternal() {
    int v = CVarGetInteger(GetExternalMaxCVarKey().c_str(), 0);
    if (v < 0)
        v = 0;
    return (uint32_t)v;
}

static void SetMaxReceivedItemCountExternal(uint32_t v) {
    if (v > (uint32_t)INT32_MAX) {
        v = (uint32_t)INT32_MAX;
    }
    CVarSetInteger(GetExternalMaxCVarKey().c_str(), (int)v);
}

static void PersistProcessedItemIndexIfNeeded(const PendingItem& item) {
    if (item.index < 0) {
        return;
    }

    if (!sSaveReadyForArchiInit) {
        return;
    }

    if (!IsArchiSaveValidNoInit()) {
        SPDLOG_WARN("[AP][Bridge] Persist skipped: invalid archi save (magic={:#x} version={}) item index={}",
                    gSaveContext.save.shipSaveInfo.rando.archipelago.magic,
                    gSaveContext.save.shipSaveInfo.rando.archipelago.version, item.index);
        return;
    }

    auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;
    const uint32_t oldCount = archi.receivedItemCount;
    const uint32_t nextCount = (uint32_t)item.index + 1;
    if (oldCount < nextCount) {
        archi.receivedItemCount = nextCount;
    }

    // Also persist "max ever received" outside the save so older-save loads can't roll it back.
    uint32_t extOld = GetMaxReceivedItemCountExternal();
    if (extOld < archi.receivedItemCount) {
        SetMaxReceivedItemCountExternal(archi.receivedItemCount);
    }
}

static void ApplyOneItem(const PendingItem& item) {
    // Session-only dedupe by AP item index.
    if (item.index >= 0) {
        const auto [it, inserted] = sProcessedItemIndices.insert(item.index);
        if (!inserted) {
            return;
        }
    }

    RandoItemId randoItemId = RI_UNKNOWN;
    bool resolved = false;

    // Mode 1: AP item ID is already a RandoItemId
    if (CVarGetInteger("gArchipelago.ItemIdIsRandoItemId", 0)) {
        randoItemId = static_cast<RandoItemId>(item.itemId);
        resolved = true;
    } else {
        // Mode 2: resolve by AP display name -> Rando::StaticData::Items[*].name
        resolved = TryResolveRandoItemIdByName(item.itemName, randoItemId);
    }

    // Mode 3 (fallback): Item from another game - classify as Archipelago item based on flags
    // This runs if Mode 1 or Mode 2 failed
    if (!resolved || randoItemId == RI_UNKNOWN) {
        randoItemId = GetRandoItemIdFromAPItemId(item.itemId, item.itemName, item.flags);
        resolved = true;
    }

    if (!resolved || randoItemId == RI_UNKNOWN) {
        SPDLOG_WARN("[AP][Bridge] Could not resolve itemId={} name='{}' to a RandoItemId", item.itemId, item.itemName);
        return;
    }

    // Determine if item is from another player
    int localPlayer = Archipelago::GetPlayerNumber();
    bool fromOtherPlayer = (item.fromPlayer != localPlayer && item.fromPlayer >= 0 && localPlayer >= 0);

    // Get display name for all items
    std::string displayItemName = item.itemName;
    if (displayItemName == "Unknown" || displayItemName.empty()) {
        displayItemName = Archipelago::GetItemName(item.itemId, "2 Ship 2 Harkinian (MM)");
        if (displayItemName == "Unknown" || displayItemName.empty()) {
            std::string playerGame = Archipelago::GetPlayerGame(item.fromPlayer);
            displayItemName = Archipelago::GetItemName(item.itemId, playerGame);
        }
    }

    // Get player name for items from other players
    std::string playerName = "";
    if (fromOtherPlayer) {
        playerName = Archipelago::GetPlayerAlias(item.fromPlayer);
    }

    // Allocate a unique ID for this item
    s16 apItemId = sNextAPItemId++;
    if (sNextAPItemId <= 0)
        sNextAPItemId = 1; // Wrap around if needed

    // Store metadata for the giveItem lambda
    sAPItemMetadata[apItemId] = { .randoItemId = randoItemId,
                                  .playerName = playerName,
                                  .displayItemName = displayItemName,
                                  .fromOtherPlayer = fromOtherPlayer };

    // Queue a GameInteractor event for proper item display
    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
        .showGetItemCutscene = Rando::StaticData::ShouldShowGetItemCutscene(randoItemId),
        .param = apItemId,
        .giveItem =
            [](Actor* actor, PlayState* play) {
                // Look up metadata using CUSTOM_ITEM_PARAM
                s16 apItemId = CUSTOM_ITEM_PARAM;
                auto it = sAPItemMetadata.find(apItemId);
                if (it == sAPItemMetadata.end()) {
                    SPDLOG_ERROR("[AP][Bridge] giveItem: No metadata for apItemId {}", apItemId);
                    return;
                }

                const APItemMetadata& meta = it->second;
                RandoItemId randoItemId = meta.randoItemId;

                // Convert the item for giving purposes (e.g., progressive upgrades, bombs -> junk if no bag).
                // Only convert actual MM items, not Archipelago placeholder items from other games.
                RandoItemId convertedItemId = randoItemId;
                if (randoItemId != RI_ARCHIPELAGO_PROGRESSIVE && randoItemId != RI_ARCHIPELAGO_USEFUL &&
                    randoItemId != RI_ARCHIPELAGO_JUNK) {

                    convertedItemId = Rando::ConvertItem(randoItemId, RC_UNKNOWN);

                    // If ConvertItem returned RI_JUNK, convert to an actual junk item (rupees, arrows, etc.)
                    if (convertedItemId == RI_JUNK) {
                        convertedItemId = Rando::CurrentJunkItem(RC_UNKNOWN);
                    }
                }

                // Build message pieces:
                // - formattedMessage is for CustomMessage textboxes (supports %g/%w)
                // - plainMessage is for Notification::Emit (does NOT support %g/%w)
                // Always use the AP display name (e.g. "Progressive Bomb Bag") regardless of
                // how the item was locally converted, since this item came from the AP server.
                std::string prefix = "You received";
                std::string displayItemName = meta.displayItemName;

                bool isTrap = (convertedItemId == RI_TRAP);
                if (isTrap) {
                    prefix = "";
                    displayItemName = GetTrapMessage();
                    if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0) >= 2) {
                        displayItemName = CustomMessage::RemoveColorCodes(displayItemName);
                    }
                }

                std::string plainMessage;
                if (meta.fromOtherPlayer && !isTrap) {
                    plainMessage = displayItemName + " from " + meta.playerName;
                } else {
                    plainMessage = displayItemName;
                }

                std::string formattedMessage;
                if (meta.fromOtherPlayer && !isTrap) {
                    formattedMessage = "%g" + displayItemName + "%w from %y" + meta.playerName + "%w";
                } else {
                    formattedMessage = "%g" + displayItemName + "%w";
                }

                CustomMessage::Entry entry = {
                    .textboxType = 2,
                    .icon = Rando::StaticData::GetIconForZMessage(convertedItemId),
                    .msg = (prefix.empty() ? "" : prefix + " ") + formattedMessage + (isTrap ? "" : "!"),
                };

                // Show message based on cutscene settings
                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                    CustomMessage::SetActiveCustomMessage(entry.msg, entry);
                } else if (Rando::StaticData::ShouldShowGetItemCutscene(convertedItemId)) {
                    CustomMessage::StartTextbox(entry.msg + "\x1C\x02\x10", entry);
                } else {
                    Notification::Emit({
                        .itemIcon = Rando::StaticData::GetIconTexturePath(convertedItemId),
                        .message = prefix,
                        .suffix = plainMessage,
                    });
                }

                // Give the item ONLY if it's a real MM item
                const bool isApPlaceholder =
                    (convertedItemId == RI_ARCHIPELAGO_PROGRESSIVE || convertedItemId == RI_ARCHIPELAGO_USEFUL ||
                     convertedItemId == RI_ARCHIPELAGO_JUNK);

                if (!isApPlaceholder) {
                    Rando::GiveItem(convertedItemId);
                }

                // Set CUSTOM_ITEM_PARAM to the convertedItemId so drawItem can use it
                // directly after CALLED_ACTION without needing the metadata map.
                // This mirrors the CheckQueue pattern (avoids recovery heart flash).
                sAPItemMetadata.erase(apItemId);
                CUSTOM_ITEM_PARAM = (s16)convertedItemId;
            },
        .drawItem =
            [](Actor* actor, PlayState* play) {
                RandoItemId randoItemId;

                if (CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION) {
                    // After give: giveItem set CUSTOM_ITEM_PARAM to the convertedItemId.
                    randoItemId = (RandoItemId)CUSTOM_ITEM_PARAM;
                } else {
                    // Before give: CUSTOM_ITEM_PARAM holds the apItemId (metadata key).
                    s16 apItemId = CUSTOM_ITEM_PARAM;
                    auto it = sAPItemMetadata.find(apItemId);
                    if (it == sAPItemMetadata.end()) {
                        SPDLOG_ERROR("[AP][Bridge] drawItem: No metadata for apItemId {}", apItemId);
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_RECOVERY_HEART, RC_UNKNOWN, actor);
                        return;
                    }
                    randoItemId = it->second.randoItemId;
                }

                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(randoItemId, RC_UNKNOWN, actor);
            } });

    PersistProcessedItemIndexIfNeeded(item);
}

static void ApplyOneDeathLink(const PendingDeathLink& dl) {
    // If we aren�t in game, don�t apply.
    if (!IsInGame() || gPlayState == nullptr) {
        return;
    }

    // Basic implementation: force death by zeroing health.
    // This mirrors the simplest approach and can be refined later if you need a �clean� death flow.
    gSaveContext.save.saveInfo.playerData.health = 0;

    // Show notification matching SoH format
    std::string prefixText = dl.source + " died.";
    Notification::Emit({ .prefix = prefixText.c_str(), .message = "Cause:", .suffix = dl.cause.c_str() });
}

void Tick() {
    if (!IsInGame()) {
        return;
    }

    if (!IS_ARCHI) {
        return;
    }

    if (!sSaveReadyForArchiInit) {
        return;
    }

    while (!sPendingItems.empty()) {
        PendingItem item = sPendingItems.front();
        sPendingItems.pop();
        ApplyOneItem(item);
    }

    while (!sPendingDeaths.empty()) {
        PendingDeathLink dl = sPendingDeaths.front();
        sPendingDeaths.pop();
        ApplyOneDeathLink(dl);
    }
}

void Reset() {
    std::queue<PendingItem> items;
    std::swap(sPendingItems, items);

    std::queue<PendingDeathLink> deaths;
    std::swap(sPendingDeaths, deaths);

    // NOTE:
    // Do NOT clear sProcessedItemIndices here.
    // Reset() is being called during reconnect / client resets, and we want session-only dedupe
    // to survive reconnects so replayed history does not re-apply.
}

void OnFileLoad() {
    // Discard any pending items from the previous file's session.
    // Items left in sPendingItems would be applied to the NEW file's save context, advancing
    // receivedItemCount before ResyncItems runs and skipping items the new file actually needs.
    {
        std::queue<PendingItem> empty;
        std::swap(sPendingItems, empty);
    }

    // Clear session-only dedupe when loading a file
    // This allows items to be re-applied if loading a different file or new game
    ClearSessionDedupe();

    // Mark save as ready and validate/init the archi blob (but DO NOT wipe valid saves)
    sSaveReadyForArchiInit = true;
    EnsureArchiSaveInitialized();

    // Prevent older-save loads from rolling back receivedItemCount.
    // IMPORTANT: Do NOT apply the global external max to a fresh save, or it will skip early item indices (0..N-1).
    if (IsArchiSaveValidNoInit()) {
        auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;

        // A save is "fresh" if it has never received any items from the server.
        // We intentionally do NOT include checkedLocationCount here: ApplyCachedCheckedLocations
        // can set checkedLocationCount > 0 before this check runs (e.g. on a new save that just
        // had cached checked-locations applied), which would wrongly treat a brand new file as
        // non-fresh and clamp receivedItemCount to the external max, blocking all items.
        const bool isFreshSave = (archi.receivedItemCount == 0);

        // Reset the external max to match the save's receivedItemCount so that
        // reloading a save always re-applies items received after the save point.
        if (!isFreshSave) {
            SetMaxReceivedItemCountExternal(archi.receivedItemCount);
        }
    }

    // If this is an Archipelago save, handle cached state re-application and one-time starting items.
    if (IS_ARCHI) {
        auto& archi = gSaveContext.save.shipSaveInfo.rando.archipelago;

        // --- MIGRATION / SAFETY ---
        // If this save is clearly not brand new (already received items or checked locations),
        // do NOT run computed starting-item grants again.
        // This protects older saves created before startingItemsGranted existed.
        if (IsArchiSaveValidNoInit() && !archi.startingItemsGranted) {
            if (archi.receivedItemCount > 0 || archi.checkedLocationCount > 0) {
                SPDLOG_WARN("[AP][Bridge] OnFileLoad: existing save detected; forcing startingItemsGranted=1 "
                            "(receivedItemCount={} checkedLocationCount={})",
                            archi.receivedItemCount, archi.checkedLocationCount);
                archi.startingItemsGranted = 1;
            }
        }

        // 1) Reapply cached slot options FIRST (so computed starting items are correct).
        const bool haveSlotData = ReapplySlotOptionsFromCache();

        // 2) Reapply cached location rewards (so shuffled items are set up for this seed).
        RepopulateLocationRewardsFromCache();

        // 3) Apply any cached checked locations (so already-checked locations don't spawn items).
        ApplyCachedCheckedLocations();

        // 4) Grant computed starting items ONCE per save, and only if we actually have slot_data.
        // If slot_data isn't available yet (connected later), this will be deferred until slot_data arrives.
        if (haveSlotData && IsArchiSaveValidNoInit() && !archi.startingItemsGranted) {
            Rando::GrantStartingItems();
            archi.startingItemsGranted = 1;
        }
    }
}

void ClearSessionDedupe() {
    // Clear session-only dedupe to allow items to be re-enqueued and re-applied
    // This is used by ResyncItems to ensure items can always be re-processed
    sSeenItemIndices.clear();
    sProcessedItemIndices.clear();
}

static constexpr const char* CVAR_AP_STATE_JSON = "gArchipelago.TempStateJson";

void SaveState(const nlohmann::json& state) {
    const std::string dump = state.dump();
    CVarSetString(CVAR_AP_STATE_JSON, dump.c_str());
}

bool LoadState(nlohmann::json& out) {
    const char* raw = CVarGetString(CVAR_AP_STATE_JSON, "");
    if (raw == nullptr || raw[0] == '\0') {
        return false;
    }

    try {
        out = nlohmann::json::parse(raw);
        return true;
    } catch (...) { return false; }
}

std::string GetArchipelagoItemText(RandoCheckId checkId) {
    auto it = sArchipelagoItemText.find(checkId);
    if (it != sArchipelagoItemText.end()) {
        const auto& [playerName, itemName] = it->second;
        // If playerName is empty, it's from the same player - don't add possessive
        if (playerName.empty()) {
            return itemName;
        }
        return playerName + "'s " + itemName;
    }
    return "";
}

void GetArchipelagoItemComponents(RandoCheckId checkId, std::string& playerName, std::string& itemName) {
    auto it = sArchipelagoItemText.find(checkId);
    if (it != sArchipelagoItemText.end()) {
        playerName = it->second.first;
        itemName = it->second.second;
    } else {
        playerName = "";
        itemName = "";
    }
}

RandoItemId GetLocalItemFromArchipelagoCheck(RandoCheckId checkId) {
    auto it = sArchipelagoItemText.find(checkId);
    if (it == sArchipelagoItemText.end()) {
        return RI_NONE;
    }

    const auto& [playerName, itemName] = it->second;

    // Try to resolve the item name to a local RandoItemId
    if (!itemName.empty() && itemName != "Unknown") {
        for (const auto& [id, staticItem] : Rando::StaticData::Items) {
            if (staticItem.name == itemName) {
                return id;
            }
        }
    }

    return RI_NONE;
}

} // namespace ArchipelagoBridge
