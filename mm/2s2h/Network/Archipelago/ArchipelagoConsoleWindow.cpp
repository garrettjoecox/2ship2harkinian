#include "ArchipelagoConsoleWindow.h"
#include "2s2h/Network/Archipelago/Archipelago.h"
#include "BenGui/BenGui.hpp"
#include "BenGui/UIWidgets.hpp"
#include "ArchipelagoTypes.h"
#include "ArchipelagoStatusWindow.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <fast/Fast3dGui.h>

extern "C" {
#include "2s2h_assets.h"
}

#include <imgui.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

static constexpr size_t MAX_CONSOLE_ITEMS = 50;

std::vector<std::vector<AP_Text::ColoredTextNode>> Items;
bool autoScroll = true;

using namespace UIWidgets;

void ArchipelagoConsole_SendMessage(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    AP_Text::ColoredTextNode node;
    node.text = std::string(buf);
    node.color = AP_Text::TextColor::COLOR_WHITE;

    if (strstr(buf, "[ERROR]")) {
        node.color = AP_Text::TextColor::COLOR_ERROR;
    } else if (strstr(buf, "[LOG]")) {
        node.color = AP_Text::TextColor::COLOR_LOG;
    }

    std::vector<AP_Text::ColoredTextNode> line;
    line.push_back(node);
    Items.push_back(line);

    if (Items.size() > MAX_CONSOLE_ITEMS) {
        Items.erase(Items.begin());
    }
}

void ArchipelagoConsole_PrintJson(const std::vector<AP_Text::ColoredTextNode> nodes) {
    Items.push_back(nodes);
    if (Items.size() > MAX_CONSOLE_ITEMS) {
        Items.erase(Items.begin());
    }
}

void ArchipelagoConsoleWindow::DrawElement() {
    float consoleScale = CVarGetFloat("gArchipelago.Console.Scale", 1.0f);

    ImGui::SeparatorText("Archipelago Log");

    const float inputPaddingY = 8.0f;
    const float inputRowHeight = ImGui::GetFontSize() + inputPaddingY * 2.0f + ImGui::GetStyle().ItemSpacing.y;
    float scrollHeight = ImGui::GetContentRegionAvail().y - inputRowHeight;
    if (scrollHeight < 50.0f)
        scrollHeight = 50.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 1.0f));

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollHeight), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        ImGui::SetWindowFontScale(consoleScale);
        for (const std::vector<AP_Text::ColoredTextNode>& line : Items) {
            if (line.empty())
                continue;

            if (line.size() == 1) {
                ImGui::PushStyleColor(ImGuiCol_Text, getColorVal(line[0].color));
                ImGui::TextWrapped("%s", line[0].text.c_str());
                ImGui::PopStyleColor();
            } else {
                float contentWidth = ImGui::GetContentRegionAvail().x;
                float lineUsed = 0.0f;
                bool needSameLine = false;
                for (size_t i = 0; i < line.size(); i++) {
                    ImVec4 color = getColorVal(line[i].color);
                    const char* ptr = line[i].text.c_str();
                    while (*ptr) {
                        const char* chunkStart = ptr;
                        while (*ptr && *ptr != ' ')
                            ptr++;
                        if (*ptr == ' ')
                            ptr++;
                        if (ptr == chunkStart)
                            break;

                        float chunkWidth = ImGui::CalcTextSize(chunkStart, ptr).x;
                        if (needSameLine) {
                            if (lineUsed + chunkWidth <= contentWidth) {
                                ImGui::SameLine(0.0f, 0.0f);
                            } else {
                                lineUsed = 0.0f; // wrap to new line
                            }
                        }
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                        ImGui::TextUnformatted(chunkStart, ptr);
                        ImGui::PopStyleColor();
                        lineUsed += chunkWidth;
                        if (lineUsed > contentWidth)
                            lineUsed = contentWidth;
                        needSameLine = true;
                    }
                }
            }
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    static char textEntryBuf[1024];
    static bool keepFocus = false;

    if (keepFocus) {
        ImGui::SetKeyboardFocusHere();
        keepFocus = false;
    }

    PushStyleInput(THEME_COLOR);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, inputPaddingY));

    if (ImGui::InputText("##AP_MessageField", textEntryBuf, 1023, ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (textEntryBuf[0] != '\0') {
            ArchipelagoConsole_SendMessage("> %s", textEntryBuf); // local echo
            Archipelago::Instance->SendChat(textEntryBuf);        // send to server
        }
        textEntryBuf[0] = '\0';
        keepFocus = true;
    }

    ImGui::PopStyleVar();
    PopStyleInput();

    ImGui::SameLine();

    if (UIWidgets::Button("Send", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0, 0.0)))) {
        if (textEntryBuf[0] != '\0') {
            ArchipelagoConsole_SendMessage("> %s", textEntryBuf);
            Archipelago::Instance->SendChat(textEntryBuf);
        }
        textEntryBuf[0] = '\0';
        keepFocus = true;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

ImVec4 ArchipelagoConsoleWindow::getColorVal(const AP_Text::TextColor color) {
    using apt = AP_Text::TextColor;
    switch (color) {
        case apt::COLOR_ERROR:
            return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        case apt::COLOR_LOG:
            return ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
        case apt::COLOR_BLACK:
            return ImVec4(0.000f, 0.000f, 0.000f, 1.00f);
        case apt::COLOR_RED:
            return ImVec4(0.933f, 0.000f, 0.000f, 1.00f);
        case apt::COLOR_GREEN:
            return ImVec4(0.000f, 1.000f, 0.498f, 1.00f);
        case apt::COLOR_YELLOW:
            return ImVec4(0.980f, 0.980f, 0.824f, 1.00f);
        case apt::COLOR_BLUE:
            return ImVec4(0.392f, 0.584f, 0.929f, 1.00f);
        case apt::COLOR_CYAN:
            return ImVec4(0.000f, 1.000f, 1.000f, 1.00f);
        case apt::COLOR_MAGENTA:
            return ImVec4(1.000f, 0.000f, 1.000f, 1.00f);
        case apt::COLOR_SLATEBLUE:
            return ImVec4(0.416f, 0.353f, 0.804f, 1.00f);
        case apt::COLOR_PLUM:
            return ImVec4(0.867f, 0.627f, 0.867f, 1.00f);
        case apt::COLOR_SALMON:
            return ImVec4(0.980f, 0.502f, 0.447f, 1.00f);
        case apt::COLOR_WHITE:
            return ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
        case apt::COLOR_ORANGE:
            return ImVec4(1.000f, 0.647f, 0.000f, 1.00f);
        case apt::COLOR_GRAY:
            return ImVec4(0.600f, 0.600f, 0.600f, 1.00f);
        default:
            return ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
    }
}

void ArchipelagoStatusWindow::DrawElement() {
}

void ArchipelagoStatusWindow::Draw() {
    if (!CVarGetInteger("gArchipelago.Enabled", 0)) {
        mConnectedTime = 0.0f;
        return;
    }

    if (CVarGetInteger("gArchipelago.StatusIndicator.Hidden", 0)) {
        return;
    }

    int status = Archipelago::Instance->GetState();

    if (status == 4 && mLastStatus != 4) {
        mConnectedTime = 0.0f;
    }
    mLastStatus = status;

    if (status == 4) {
        mConnectedTime += ImGui::GetIO().DeltaTime;
    }

    const char* statusText = nullptr;
    bool shouldDrawText = true;

    switch (status) {
        case 0: // Not Connected
            statusText = "Not Connected";
            break;
        case 1: // Connecting
        case 2: // Connection error, retrying
        case 3: // Connected (not fully ready)
            statusText = "Connecting...";
            break;
        case 4: // Connected + Locations Scouted (fully ready)
            // Fade out after 10 seconds
            if (mConnectedTime < 10.0f) {
                statusText = "Connected";
            } else {
                shouldDrawText = false; // Only show icon
            }
            break;
    }

    auto vp = ImGui::GetMainViewport();
    float posX = CVarGetFloat("gArchipelago.StatusIndicator.PosX", 15.0f);
    float posYFromBottom = CVarGetFloat("gArchipelago.StatusIndicator.PosY", 45.0f);
    float scale = CVarGetFloat("gArchipelago.StatusIndicator.Scale", 1.0f);

    const float iconSize = 30.0f * scale;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 0));

    ImVec2 defaultPos = ImVec2(vp->Pos.x + posX, vp->Pos.y + vp->Size.y - posYFromBottom);
    ImGui::SetNextWindowViewport(vp->ID);

    if (CVarGetInteger("gArchipelago.StatusIndicator.NeedsReset", 0)) {
        ImGui::SetNextWindowPos(defaultPos, ImGuiCond_Always);
        CVarSetInteger("gArchipelago.StatusIndicator.NeedsReset", 0);
    } else {
        ImGui::SetNextWindowPos(defaultPos, ImGuiCond_FirstUseEver);
    }

    ImGui::Begin("ArchipelagoStatus", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    ImVec2 curPos = ImGui::GetWindowPos();
    float newPosX = curPos.x - vp->Pos.x;
    float newPosYFromBottom = (vp->Pos.y + vp->Size.y) - curPos.y;
    if (fabsf(newPosX - posX) > 0.5f || fabsf(newPosYFromBottom - posYFromBottom) > 0.5f) {
        CVarSetFloat("gArchipelago.StatusIndicator.PosX", newPosX);
        CVarSetFloat("gArchipelago.StatusIndicator.PosY", newPosYFromBottom);
        CVarSave();
    }

    auto texture = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                       ->GetTextureByName(gArchipelagoUsefulIconTex);
    if (texture) {
        ImGui::Image(texture, ImVec2(iconSize, iconSize));
    }

    if (shouldDrawText && statusText != nullptr) {
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + -2.0f);
        ImGui::SetWindowFontScale(1.8f * scale);
        ImVec4 textColor = (status == 0) ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::TextColored(textColor, "%s", statusText);
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}
