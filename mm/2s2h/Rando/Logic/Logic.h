#ifndef RANDO_LOGIC_H
#define RANDO_LOGIC_H

#include <unordered_map>
#include <set>
#include "Rando/Rando.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64item.h"
#include "z64scene.h"
}

namespace Rando {

namespace Logic {

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions);
RandoRegionId GetRegionIdFromEntrance(s32 entrance);
void ApplyGlitchlessLogicToSaveContext();
void ApplyNoLogicToSaveContext();

struct RandoEvent {
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

} // namespace Logic

} // namespace Rando

#endif // RANDO_LOGIC_H
