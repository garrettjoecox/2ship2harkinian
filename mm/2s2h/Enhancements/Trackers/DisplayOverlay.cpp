#include "DisplayOverlay.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
uint64_t GetUnixTimestamp();
}

#include "ShipUtils.h"
#include "interface/parameter_static/parameter_static.h"
#include "GameInteractor/GameInteractor.h"

float windowScale = 1.0f;
ImVec4 windowBG = ImVec4(0, 0, 0, 0.5f);

std::string formatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

void DrawInGameTimer(uint32_t timer) {
    std::string timerStr = formatTimeDisplay(timer).c_str();
    char* textToDecode = new char[timerStr.size() + 1];
    textToDecode = std::strcpy(textToDecode, timerStr.c_str());
    size_t textLength = timerStr.length();
    uint16_t textureIndex = 0;

    for (size_t i = 0; i < textLength; i++) {
        ImVec2 originalCursorPos = ImGui::GetCursorPos();
        if (textToDecode[i] == ':' || textToDecode[i] == '.') {
            textureIndex = 10;
        } else {
            textureIndex = textToDecode[i] - '0';
        }
        if (textToDecode[i] == '.') {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(digitList[textureIndex]),
                         ImVec2(8.0f * windowScale, 8.0f * windowScale), ImVec2(0, 0.5f), ImVec2(1, 1));
        } else {
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(digitList[textureIndex]),
                         ImVec2(8.0f * windowScale, 16.0f * windowScale), ImVec2(0, 0), ImVec2(1, 1));
        }
        ImGui::SameLine(0, 0);
    }
}

void DisplayOverlayWindow::Draw() {
    if (!gPlayState) {
        return;
    }
    if (!CVarGetInteger("gWindows.DisplayOverlay", 0)) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(windowScale);

    ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(gTimerClockIconTex),
                 ImVec2(16.0f * windowScale, 16.0f * windowScale));
    ImGui::SameLine(0, 10.0f);
    // DrawInGameTimer(TOTAL_GAMEPLAY_TIME); // TODO: Once LUS is bumped to include the Texture Alpha changes, this can
    // take over.
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (2.0f * windowScale));
    ImGui::Text(formatTimeDisplay(TOTAL_GAMEPLAY_TIME).c_str());

    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);
}

void DisplayOverlayInitSettings() {
    windowScale = CVarGetFloat("gDisplayOverlay.Scale", 1.0f);
    if (windowScale < 1.0f) {
        windowScale = 1.0f;
    }
    windowBG = !CVarGetInteger("gDisplayOverlay.Background", 0) ? ImVec4(0, 0, 0, 0.5f) : ImVec4(0, 0, 0, 0);
}

void DisplayOverlayWindow::InitElement() {
    DisplayOverlayInitSettings();
    COND_HOOK(OnSaveLoad, true, [](s16 fileNum) {
        if (gSaveContext.save.shipSaveInfo.fileCreatedAt == 0) {
            gSaveContext.save.shipSaveInfo.fileCreatedAt = GetUnixTimestamp();
        }
    });
}