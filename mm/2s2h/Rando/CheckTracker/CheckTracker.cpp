
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"

bool showLogic = false;
bool hideCollected = false;
bool updateHeaders = false;
bool expandHeader = true;

std::vector<std::tuple<std::string, SceneId, bool, bool, uint32_t>> sceneCheckList;

std::string convertToReadableName(const std::string& input) {
    std::string result;
    std::string content = input;

    // Step 1: Remove "RC_" prefix if present
    const std::string prefix = "RC_";
    if (content.rfind(prefix, 0) == 0) {
        content = content.substr(prefix.size());
    }

    // Step 2: Split the string by '_'
    std::vector<std::string> words;
    std::string word;
    std::istringstream stream(content);
    while (std::getline(stream, word, '_')) {
        words.push_back(word);
    }

    // Step 3: Capitalize the first letter of each word
    for (auto& w : words) {
        std::transform(w.begin(), w.end(), w.begin(), [](unsigned char c) { return std::tolower(c); });
        if (!w.empty()) {
            w[0] = std::toupper(w[0]);
        }
    }

    // Step 4: Join the words with spaces
    for (size_t i = 0; i < words.size(); ++i) {
        result += words[i];
        if (i < words.size() - 1) {
            result += " ";
        }
    }

    return result;
}

uint32_t totalCheckAmount(SceneId sceneId) {
    uint32_t collected = 0;
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
        if (sceneId == randoStaticCheck.sceneId) {
            if (randoSaveCheck.obtained) {
                collected++;
            }
        }
    }
    return collected;
}

namespace Rando {

namespace CheckTracker {

void CheckTrackerUpdateSceneList(int32_t action) {
    if (action == SCENE_LOAD) {
        for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
            bool sceneExists = false;
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
            for (auto& scene : sceneCheckList) {
                sceneExists = false;
                if (std::get<1>(scene) == randoStaticCheck.sceneId) {
                    sceneExists = true;
                    std::get<4>(scene)++;
                    break;
                }
            }
            if (!sceneExists) {
                sceneCheckList.push_back(std::make_tuple(Ship_GetSceneName(randoStaticCheck.sceneId),
                                                         randoStaticCheck.sceneId, true, false, 1));
            }
        }
    } else {
        for (auto& scene : sceneCheckList) {
            std::get<2>(scene) = expandHeader;
            std::get<3>(scene) = false;
        }
    }
}

bool checkTrackerShouldShowRow(bool obtained) {
    bool showCheck = true;
    if (hideCollected && obtained) {
        showCheck = false;
    }
    return showCheck;
}

void Window::DrawElement() {
    if (showLogic) {
        std::set<RandoRegionId> reachableRegions = {};
        // Get connected entrances from starting & warp points
        Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
        // Get connected regions from current entrance (TODO: Make this optional)
        Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                           reachableRegions);

        for (RandoRegionId regionId : reachableRegions) {
            auto& randoRegion = Rando::Logic::Regions[regionId];
            std::vector<std::pair<RandoCheckId, std::string>> availableChecks;

            for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
                auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc.first()) {
                    availableChecks.push_back({ randoCheckId, accessLogicFunc.second });
                }
            }

            if (availableChecks.size() > 0) {
                std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
                if (randoRegion.name != "") {
                    regionName += " - ";
                    regionName += randoRegion.name;
                }
                ImGui::SeparatorText(regionName.c_str());

                for (auto& [checkId, accessLogicString] : availableChecks) {
                    auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                    ImGui::Text("%s", randoStaticCheck.name);
                    if (accessLogicString != "") {
                        UIWidgets::Tooltip(accessLogicString.c_str());
                    }
                }
            }
        }
    } else {
        for (auto& scene : sceneCheckList) {
            if (std::get<0>(scene) == "Unknown") {
                continue;
            }
            ImGui::PushID(std::get<1>(scene));
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            uint32_t popCount = 1;
            ImGui::Separator();
            uint32_t collectedChecks = totalCheckAmount(std::get<1>(scene));
            std::string headerText = std::get<0>(scene).c_str();
            headerText += " (" + std::to_string(collectedChecks) + "/" + std::to_string(std::get<4>(scene)) + ")";
            if (collectedChecks == std::get<4>(scene)) {
                ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::Colors::LightGreen);
                popCount++;
            }
            bool shouldExpand = std::get<2>(scene);
            if (updateHeaders) {
                ImGui::SetNextItemOpen(shouldExpand);
            } else {
                ImGui::SetNextItemOpen(shouldExpand, ImGuiCond_Once);
            }
            if (ImGui::CollapsingHeader(headerText.c_str())) {
                ImGui::Indent(20.0f);
                for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (std::get<1>(scene) == randoStaticCheck.sceneId) {
                        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
                        if (!randoSaveCheck.shuffled) {
                            continue;
                        }
                        ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained ? UIWidgets::Colors::LightGreen
                                                                                     : UIWidgets::Colors::White);
                        if (checkTrackerShouldShowRow(randoSaveCheck.obtained)) {
                            ImGui::Text(convertToReadableName(randoStaticCheck.name).c_str());
                            if (randoSaveCheck.obtained) {
                                ImGui::SameLine(0, 50.0f);
                                std::string itemName = "(";
                                itemName +=
                                    convertToReadableName(Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                                itemName += ")";
                                ImGui::Text(itemName.c_str());
                            }
                        }
                        ImGui::PopStyleColor();
                    }
                }
                ImGui::Unindent(20.0f);
            }
            if (ImGui::IsItemClicked()) {
                bool isExpanded = std::get<2>(scene);
                std::get<2>(scene) = !isExpanded;
            }
            ImGui::PopStyleColor(popCount);
            ImGui::PopID();
        }
        updateHeaders = false;
    }
}

void SettingsWindow::DrawElement() {
    ImGui::SeparatorText("Check Tracker Settings");
    UIWidgets::Checkbox("Show Logic", &showLogic);
    UIWidgets::Checkbox("Hide Collected", &hideCollected);
    if (ImGui::Button("Expand/Collapse All")) {
        expandHeader = !expandHeader;
        updateHeaders = true;
        CheckTrackerUpdateSceneList(SCENE_UPDATE);
    }
}

void Window::InitElement() {
    COND_HOOK(OnSaveLoad, &gSaveContext.save.shipSaveInfo.rando,
              [](uint32_t fileNum) { CheckTrackerUpdateSceneList(SCENE_LOAD); });
}

} // namespace CheckTracker

} // namespace Rando
