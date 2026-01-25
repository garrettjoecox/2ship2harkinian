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
#define CVAR_NAME_ROUTE_DETAIL "gRando.EntranceTracker.RouteDetailMode"
#define CVAR_NAME_SHOW_SEARCH "gRando.EntranceTracker.ShowSearch"

#define CVAR_SHOW_ENTRANCE_TRACKER CVarGetInteger(CVAR_NAME_SHOW_ENTRANCE_TRACKER, 0)
#define CVAR_VISIBILITY_MODE CVarGetInteger(CVAR_NAME_VISIBILITY_MODE, ENTRANCE_TRACKER_VISIBILITY_MODE_ALWAYS)
#define CVAR_VISIBILITY_BTN CVarGetInteger(CVAR_NAME_VISIBILITY_BTN, BTN_CUSTOM_MODIFIER1)
#define CVAR_TRACKER_OPACITY CVarGetFloat(CVAR_NAME_TRACKER_OPACITY, 0.5f)
#define CVAR_TRACKER_SCALE CVarGetFloat(CVAR_NAME_TRACKER_SCALE, 1.0f)
#define CVAR_SPOILER_MODE CVarGetInteger(CVAR_NAME_SPOILER_MODE, 1) // Default: show all
#define CVAR_ROUTE_DETAIL CVarGetInteger(CVAR_NAME_ROUTE_DETAIL, 0) // Default: compact
#define CVAR_SHOW_SEARCH CVarGetInteger(CVAR_NAME_SHOW_SEARCH, 1)

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

// Entrance name mapping - built from scene names with spawn-specific suffixes
static std::map<s32, std::string> sEntranceNames;

// Helper to extract scene ID from entrance
static SceneId GetSceneFromEntrance(s32 entrance) {
    return static_cast<SceneId>((entrance >> 9) & 0x7F);
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

// Build the entrance name mapping with better names for specific entrances
static void BuildEntranceNameMap() {
    sEntranceNames.clear();

    // Add custom names for specific entrances (more descriptive than just scene name)
    sEntranceNames[ENTRANCE(STOCK_POT_INN, 0)] = "Stock Pot Inn (Main)";
    sEntranceNames[ENTRANCE(STOCK_POT_INN, 1)] = "Stock Pot Inn (Upstairs)";
    sEntranceNames[ENTRANCE(CURIOSITY_SHOP, 0)] = "Curiosity Shop (Front)";
    sEntranceNames[ENTRANCE(CURIOSITY_SHOP, 1)] = "Curiosity Shop (Back)";
    sEntranceNames[ENTRANCE(RANCH_HOUSE, 0)] = "Romani Ranch House (Main)";
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
    sEntranceNames[ENTRANCE(CLOCK_TOWER_INTERIOR, 1)] = "Clock Tower Interior";

    // Overworld connections - use more descriptive names
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 3)] = "East Clock Town (from South)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 2)] = "South Clock Town (from East)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 2)] = "West Clock Town (from South)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 3)] = "South Clock Town (from West)";
    sEntranceNames[ENTRANCE(NORTH_CLOCK_TOWN, 2)] = "North Clock Town (from South)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 4)] = "South Clock Town (from North)";
    sEntranceNames[ENTRANCE(WEST_CLOCK_TOWN, 1)] = "West Clock Town (Swordsman)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 5)] = "South Clock Town (Swordsman)";
    sEntranceNames[ENTRANCE(LAUNDRY_POOL, 0)] = "Laundry Pool";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 6)] = "South Clock Town (Laundry)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 1)] = "East Clock Town (Chest Shop)";
    sEntranceNames[ENTRANCE(SOUTH_CLOCK_TOWN, 7)] = "South Clock Town (Chest Shop)";
    sEntranceNames[ENTRANCE(NORTH_CLOCK_TOWN, 1)] = "North Clock Town (Fairy)";
    sEntranceNames[ENTRANCE(EAST_CLOCK_TOWN, 5)] = "East Clock Town (Fairy)";
}

// Get the source scene for an entrance (where you are when you use it)
static SceneId GetSourceSceneForEntrance(s32 entrance) {
    // Use the region data to find where the entrance originates
    RandoRegionId regionId = Rando::Logic::GetRegionIdFromEntrance(entrance);
    if (regionId != RR_MAX && Rando::Logic::Regions.count(regionId) > 0) {
        return Rando::Logic::Regions[regionId].sceneId;
    }
    // Fallback: the entrance scene itself
    return GetSceneFromEntrance(entrance);
}

// Build entrance data grouped by source area
static void BuildEntranceData() {
    sEntrancesByArea.clear();
    sSortedSceneIds.clear();

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

            // Get source scene for grouping
            SceneId sourceScene = GetSourceSceneForEntrance(original);

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

// Build region list for route finder dropdowns
static void BuildRegionList() {
    sRegionList.clear();

    for (const auto& [regionId, region] : Rando::Logic::Regions) {
        if (region.name != nullptr && strlen(region.name) > 0) {
            sRegionList.push_back({ regionId, region.name });
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

        // Check exits (using shuffled entrances)
        for (const auto& [exitId, regionExit] : region.exits) {
            s32 actualExit = Rando::EntranceShuffle::GetShuffledEntrance(exitId);
            RandoRegionId targetRegion = Rando::Logic::GetRegionIdFromEntrance(actualExit);

            if (targetRegion == to) {
                // Found the destination
                result = path;
                RouteStep step;
                step.region = to;
                step.entranceUsed = exitId;
                step.description = Rando::EntranceTracker::GetEntranceName(actualExit);
                result.push_back(step);
                return result;
            }

            if (targetRegion != RR_MAX && visited.find(targetRegion) == visited.end()) {
                visited.insert(targetRegion);
                auto newPath = path;
                RouteStep step;
                step.region = targetRegion;
                step.entranceUsed = exitId;
                step.description = Rando::EntranceTracker::GetEntranceName(actualExit);
                newPath.push_back(step);
                queue.push(newPath);
            }
        }

        // Check connections (same-scene region transitions)
        for (const auto& [connectedRegion, condition] : region.connections) {
            if (connectedRegion == to) {
                result = path;
                RouteStep step;
                step.region = to;
                step.entranceUsed = -1;
                step.description = "";
                result.push_back(step);
                return result;
            }

            if (visited.find(connectedRegion) == visited.end()) {
                visited.insert(connectedRegion);
                auto newPath = path;
                RouteStep step;
                step.region = connectedRegion;
                step.entranceUsed = -1;
                step.description = "";
                newPath.push_back(step);
                queue.push(newPath);
            }
        }
    }

    return result; // Empty if no route found
}

// Format route for display
static std::string FormatRoute(RandoRegionId from, const std::vector<RouteStep>& route, bool detailed) {
    if (route.empty()) {
        return "No route found";
    }

    std::string result;

    if (detailed) {
        // Detailed format
        if (Rando::Logic::Regions.count(from) > 0) {
            result = "From " + std::string(Rando::Logic::Regions[from].name);
        }

        for (size_t i = 0; i < route.size(); i++) {
            const auto& step = route[i];
            if (step.entranceUsed != -1) {
                result += "\n  -> Take '" + step.description + "'";
            }
            if (Rando::Logic::Regions.count(step.region) > 0) {
                result += "\n  => " + std::string(Rando::Logic::Regions[step.region].name);
            }
        }
    } else {
        // Compact format
        if (Rando::Logic::Regions.count(from) > 0) {
            result = Rando::Logic::Regions[from].name;
        }

        for (const auto& step : route) {
            if (Rando::Logic::Regions.count(step.region) > 0) {
                result += " -> " + std::string(Rando::Logic::Regions[step.region].name);
            }
        }
    }

    return result;
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

    // Search bar
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

    // Tab bar for Entrances and Route Finder
    if (ImGui::BeginTabBar("EntranceTrackerTabs")) {
        if (ImGui::BeginTabItem("Entrances")) {
            ImGui::BeginChild("EntranceList");

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

                        // Highlight if this entrance was shuffled (different from original)
                        if (conn->originalEntrance != conn->shuffledEntrance) {
                            ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow), "%s", line.c_str());
                        } else {
                            ImGui::Text("%s", line.c_str());
                        }
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

            // From dropdown
            ImGui::Text("From:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##FromRegion", sRouteFromIndex < (int)sRegionList.size()
                                  ? sRegionList[sRouteFromIndex].second.c_str() : "Select...")) {
                for (int i = 0; i < (int)sRegionList.size(); i++) {
                    bool selected = (i == sRouteFromIndex);
                    if (ImGui::Selectable(sRegionList[i].second.c_str(), selected)) {
                        sRouteFromIndex = i;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // "Use Current Location" button
            ImGui::SameLine();
            if (UIWidgets::Button("Current", UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                if (gPlayState) {
                    RandoRegionId currentRegion = Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance);
                    for (int i = 0; i < (int)sRegionList.size(); i++) {
                        if (sRegionList[i].first == currentRegion) {
                            sRouteFromIndex = i;
                            break;
                        }
                    }
                }
            }
            UIWidgets::Tooltip("Use current location");

            // To dropdown
            ImGui::Text("To:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##ToRegion", sRouteToIndex < (int)sRegionList.size()
                                  ? sRegionList[sRouteToIndex].second.c_str() : "Select...")) {
                for (int i = 0; i < (int)sRegionList.size(); i++) {
                    bool selected = (i == sRouteToIndex);
                    if (ImGui::Selectable(sRegionList[i].second.c_str(), selected)) {
                        sRouteToIndex = i;
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Find Route button
            if (UIWidgets::Button("Find Route", UIWidgets::ButtonOptions().Color(UIWidgets::Colors::Green))) {
                if (sRouteFromIndex < (int)sRegionList.size() && sRouteToIndex < (int)sRegionList.size()) {
                    RandoRegionId from = sRegionList[sRouteFromIndex].first;
                    RandoRegionId to = sRegionList[sRouteToIndex].first;
                    auto route = FindRoute(from, to);
                    sRouteResult = FormatRoute(from, route, CVAR_ROUTE_DETAIL);
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

        UIWidgets::CVarCheckbox("Detailed Route Display", CVAR_NAME_ROUTE_DETAIL);
        UIWidgets::Tooltip("When enabled, shows step-by-step instructions instead of compact path.");

        UIWidgets::CVarCheckbox("Show Search", CVAR_NAME_SHOW_SEARCH, UIWidgets::CheckboxOptions().DefaultValue(true));

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
