
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/Rando/StaticData/StaticData.h"
#include <sstream>
#include <cstring>

extern "C" {
s16 Play_GetOriginalSceneId(s16 sceneId);
}

bool showLogic = false;
bool hideCollected = false;
bool expandHeaders = true;
bool expandedheaderState = true;
bool scrollToCurrentScene = true;
s32 scrollToTargetScene = -1;
s32 scrollToTargetEntrance = -1;

std::map<SceneId, std::vector<RandoCheckId>> sceneChecks;
std::vector<SceneId> sceneIdsSortedByAlphabetical;
std::unordered_map<RandoCheckId, std::string> readableCheckNames;

uint32_t getSumOfObtainedChecks(std::vector<RandoCheckId>& checks) {
    uint32_t collected = 0;
    for (RandoCheckId checkId : checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
        if (randoSaveCheck.obtained) {
            collected++;
        }
    }
    return collected;
}

void initializeSceneChecks() {
    sceneChecks.clear();
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
        if (!randoSaveCheck.shuffled) {
            continue;
        }

        SceneId sceneId = (SceneId)Play_GetOriginalSceneId(randoStaticCheck.sceneId);
        sceneChecks[sceneId].push_back(randoStaticCheck.randoCheckId);
    }

    sceneIdsSortedByAlphabetical.clear();
    for (auto& [sceneId, _] : sceneChecks) {
        sceneIdsSortedByAlphabetical.push_back(sceneId);
    }
    std::sort(sceneIdsSortedByAlphabetical.begin(), sceneIdsSortedByAlphabetical.end(),
              [](SceneId a, SceneId b) { return std::strcmp(Ship_GetSceneName(a), Ship_GetSceneName(b)) < 0; });
}

bool checkTrackerShouldShowRow(bool obtained) {
    bool showCheck = true;
    if (hideCollected && obtained) {
        showCheck = false;
    }
    return showCheck;
}

namespace Rando {

namespace CheckTracker {

void Window::DrawElement() {
    static ImGuiTextFilter filter;

    UIWidgets::PushStyleCombobox();
    filter.Draw("##filter", ImGui::GetContentRegionAvail().x);
    UIWidgets::PopStyleCombobox();
    if (!filter.IsActive()) {
        ImGui::SameLine(18.0f);
        ImGui::Text("Search");
    }

    ImGui::BeginChild("Checks", ImVec2(0, 0));
    if (showLogic) {
        std::set<RandoRegionId> reachableRegions = {};
        // Get connected entrances from starting & warp points
        Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
        // Get connected regions from current entrance (TODO: Make this optional)
        Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                           reachableRegions);

        for (RandoRegionId regionId : reachableRegions) {
            if (scrollToCurrentScene && scrollToTargetEntrance != -1 &&
                Rando::Logic::GetRegionIdFromEntrance(scrollToTargetEntrance) == regionId) {
                ImGui::SetScrollHereY();
                scrollToTargetScene = -1;
                scrollToTargetEntrance = -1;
            }
            auto& randoRegion = Rando::Logic::Regions[regionId];
            std::vector<std::pair<RandoCheckId, std::string>> availableChecks;
            uint32_t obtainedCheckSum = 0;

            for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
                auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                if (randoSaveCheck.shuffled && accessLogicFunc.first()) {
                    if (!filter.PassFilter(readableCheckNames[randoCheckId].c_str())) {
                        continue;
                    }

                    availableChecks.push_back({ randoCheckId, accessLogicFunc.second });
                    if (randoSaveCheck.obtained) {
                        obtainedCheckSum++;
                    }
                }
            }

            if (availableChecks.size() > 0) {
                std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
                if (randoRegion.name != "") {
                    regionName += " - ";
                    regionName += randoRegion.name;
                }

                regionName +=
                    " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(availableChecks.size()) + ")";

                ImGui::PushID(regionId);
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
                if (expandedheaderState != expandHeaders) {
                    ImGui::SetNextItemOpen(expandHeaders);
                }
                if (ImGui::CollapsingHeader(regionName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent(20.0f);
                    for (auto& [checkId, accessLogicString] : availableChecks) {
                        auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                        auto& randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
                        ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained ? UIWidgets::Colors::LightGreen
                                                                                     : UIWidgets::Colors::White);
                        if (checkTrackerShouldShowRow(randoSaveCheck.obtained)) {
                            ImGui::Text("%s", readableCheckNames[checkId].c_str());
                            if (accessLogicString != "") {
                                UIWidgets::Tooltip(accessLogicString.c_str());
                            }
                            if (randoSaveCheck.obtained) {
                                ImGui::SameLine(0, 50.0f);
                                ImGui::Text("%s", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                            }
                        }
                        ImGui::PopStyleColor();
                    }
                    ImGui::Unindent(20.0f);
                }
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }
        expandedheaderState = expandHeaders;
    } else {
        for (auto& sceneId : sceneIdsSortedByAlphabetical) {
            if (sceneId == SCENE_MAX) {
                continue;
            }

            auto& unfilteredChecks = sceneChecks[sceneId];
            std::vector<RandoCheckId> checks;
            uint32_t obtainedCheckSum = 0;

            for (auto& checkId : unfilteredChecks) {
                if (RANDO_SAVE_CHECKS[checkId].obtained) {
                    obtainedCheckSum++;
                    if (hideCollected) {
                        continue;
                    }
                }

                if (!filter.PassFilter(readableCheckNames[checkId].c_str())) {
                    continue;
                }

                checks.push_back(checkId);
            }

            if (checks.size() == 0) {
                continue;
            }

            if (scrollToCurrentScene && scrollToTargetScene != -1 && scrollToTargetScene == sceneId) {
                ImGui::SetScrollHereY();
                scrollToTargetScene = -1;
                scrollToTargetEntrance = -1;
            }

            ImGui::PushID(sceneId);
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            std::string headerText = Ship_GetSceneName(sceneId);
            headerText += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(unfilteredChecks.size()) + ")";

            ImGui::PushStyleColor(ImGuiCol_Text, obtainedCheckSum == unfilteredChecks.size()
                                                     ? UIWidgets::Colors::LightGreen
                                                     : UIWidgets::Colors::White);

            if (expandedheaderState != expandHeaders) {
                ImGui::SetNextItemOpen(expandHeaders);
            }
            if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(20.0f);
                for (auto& randoCheckId : checks) {
                    Rando::StaticData::RandoStaticCheck& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained ? UIWidgets::Colors::LightGreen
                                                                                 : UIWidgets::Colors::White);
                    if (checkTrackerShouldShowRow(randoSaveCheck.obtained)) {
                        ImGui::Text("%s", readableCheckNames[randoCheckId].c_str());
                        if (randoSaveCheck.obtained) {
                            ImGui::SameLine(0, 50.0f);
                            ImGui::Text("%s", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                        }
                    }
                    ImGui::PopStyleColor();
                }
                ImGui::Unindent(20.0f);
            }
            ImGui::PopStyleColor(2);
            ImGui::PopID();
        }
        expandedheaderState = expandHeaders;
    }
    ImGui::EndChild();
}

void SettingsWindow::DrawElement() {
    ImGui::SeparatorText("Check Tracker Settings");
    UIWidgets::Checkbox("Show Logic", &showLogic);
    UIWidgets::Checkbox("Hide Collected", &hideCollected);
    UIWidgets::Checkbox("Auto Scroll To Current Scene", &scrollToCurrentScene);
    if (UIWidgets::Button("Expand/Collapse All")) {
        expandHeaders = !expandHeaders;
    }
}

void Init() {
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        readableCheckNames[randoCheckId] = convertEnumToReadableName(randoStaticCheck.name);
    }
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s8 sceneId, s8 spawnNum) {
        if (scrollToCurrentScene) {
            scrollToTargetScene = Play_GetOriginalSceneId(sceneId);
            scrollToTargetEntrance = gSaveContext.save.entrance;
        }
    });
}

void OnFileLoad() {
    if (!IS_RANDO) {
        return;
    }

    initializeSceneChecks();
}

} // namespace CheckTracker

} // namespace Rando
