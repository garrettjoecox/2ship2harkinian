#pragma once
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <nlohmann/json.hpp>
#include "Rando/Types.h"

namespace ArchipelagoBridge {

struct PendingItem {
    uint64_t itemId = 0;
    int fromPlayer = -1;
    int64_t index = -1;   // AP item index for dedupe/persistence
    uint32_t flags = 0;   // AP item flags (trap/advancement/useful), optional
    std::string itemName; // AP item display name
};

struct PendingDeathLink {
    std::string source;
    std::string cause;
};

bool IsInGame();

// Called by Archipelago.cpp handlers or Update() to queue work.
void EnqueueItem(uint64_t itemId, int player, int64_t index, uint32_t flags, const std::string& itemName);
void EnqueueDeathLink(const std::string& source, const std::string& cause);

bool IsLocationChecked(uint64_t locationId);
void MarkLocationChecked(uint64_t locationId);

// RandoCheckId <-> Archipelago Location ID mapping
uint64_t GetLocationIdFromRandoCheck(RandoCheckId rc);
RandoCheckId GetRandoCheckFromLocationId(uint64_t apLocationId);

// Populate location rewards from AP data
void PopulateLocationRewards(const nlohmann::json& locationInfo);
void RepopulateLocationRewardsFromCache(); // Reapply cached location data (e.g., after file load)
RandoItemId GetRandoItemIdFromAPItemId(uint64_t apItemId, const std::string& itemName, uint32_t flags = 0);

// Cache and apply server-checked locations for new save files
void CacheCheckedLocations(const std::set<int64_t>& checkedLocations);
void ApplyCachedCheckedLocations(); // Mark cached checked locations as obtained (call during OnFileLoad)

// Apply slot options from AP server
void ApplySlotOptions(const nlohmann::json& slotData);

// Get formatted Archipelago item text (e.g., "Player 2's Hookshot")
// Returns empty string if not an Archipelago item
std::string GetArchipelagoItemText(RandoCheckId checkId);

// Get the player name and item name separately for an Archipelago check.
// playerName is empty if the item belongs to the local player.
void GetArchipelagoItemComponents(RandoCheckId checkId, std::string& playerName, std::string& itemName);

// Get the underlying RandoItemId for an Archipelago item if its name matches a local game item
// Returns RI_NONE if the item name doesn't match any local items
RandoItemId GetLocalItemFromArchipelagoCheck(RandoCheckId checkId);

bool ReapplySlotOptionsFromCache(); // Reapply cached slot_data (e.g., after file load). Returns true if data was
                                    // applied.

// Called from Archipelago::Update() on the main thread.
void Tick();
void Reset();
void OnFileLoad();         // Clear session-only dedupe when loading a file
void ClearSessionDedupe(); // Clear session-only item dedupe (for ResyncItems)

// Archipelago state persistence (temporary; you can swap to real save later)
void SaveState(const nlohmann::json& state);
bool LoadState(nlohmann::json& out);

} // namespace ArchipelagoBridge
