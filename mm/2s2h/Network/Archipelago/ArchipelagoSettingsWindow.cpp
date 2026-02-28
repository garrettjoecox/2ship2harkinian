#include "ArchipelagoSettingsWindow.h"
#include "ArchipelagoConsoleWindow.h"
#include "Archipelago.h"
#include "BenGui/BenGui.hpp"
#include "BenGui/UIWidgets.hpp"
#include <imgui.h>
#include <string>
#include <cstring>

using namespace UIWidgets;

void ArchipelagoSettingsWindow::DrawElement() {
    // This controls whether NEWLY CREATED saves become SAVETYPE_ARCHI in OnFileCreate.
    // Existing saves use shipSaveInfo.saveType and are not changed by this.
    if (UIWidgets::CVarCheckbox("Enable Archipelago for new saves", "gArchipelago.Enabled",
                                UIWidgets::CheckboxOptions()
                                    .Color(THEME_COLOR)
                                    .Tooltip("When enabled, creating a new file will mark it as an Archipelago save.\n"
                                             "Existing saves are not changed.\n\n"
                                             "Note: enabling Archipelago will disable Randomizer mode."))) {
        if (CVarGetInteger("gArchipelago.Enabled", 0)) {
            CVarSetInteger("gRando.Enabled", 0);
            CVarSave();
        }
    }

    ImGui::SeparatorText("Connection info");

    // Connect / Disconnect button + status (match SoH placement)
    const bool connected = Archipelago::IsConnected();
    const bool connecting = Archipelago::IsConnecting();

    UIWidgets::PushStyleCombobox(THEME_COLOR);
    ImGui::PushStyleColor(ImGuiCol_Border, UIWidgets::ColorValues.at(THEME_COLOR));

    ImGui::BeginDisabled(connected || connecting);

    ImGui::Text("Server Address");
    UIWidgets::CVarInputString("##ArchipelagoServerAddress", "gArchipelago.ServerAddress",
                               UIWidgets::InputOptions()
                                   .Color(THEME_COLOR)
                                   .PlaceholderText("archipelago.gg:38281")
                                   .DefaultValue("archipelago.gg:38281")
                                   .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                                   .LabelPosition(UIWidgets::LabelPosition::None));

    ImGui::Text("Slot Name");
    UIWidgets::CVarInputString("##ArchipelagoSlotName", "gArchipelago.Slot",
                               UIWidgets::InputOptions()
                                   .Color(THEME_COLOR)
                                   .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                                   .LabelPosition(UIWidgets::LabelPosition::None));

    ImGui::Text("Password (leave blank for no password)");
    // 2Ship doesn't have IsSecret(true), so we mirror SoH visuals but use ImGui Password.
    {
        static char passBuf[256];
        std::string pass = CVarGetString("gArchipelago.Password", "");
        std::strncpy(passBuf, pass.c_str(), sizeof(passBuf));
        passBuf[sizeof(passBuf) - 1] = '\0';

        UIWidgets::PushStyleInput(THEME_COLOR);
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
        if (ImGui::InputText("##ArchipelagoPassword", passBuf, sizeof(passBuf),
                             ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CallbackAlways,
                             [](ImGuiInputTextCallbackData* data) {
                                 CVarSetString("gArchipelago.Password", data->Buf);
                                 return 0;
                             })) {}
        UIWidgets::PopStyleInput();
    }

    ImGui::EndDisabled();

    ImGui::PopStyleColor();
    UIWidgets::PopStyleCombobox();

    if (!connected) {
        if (UIWidgets::Button("Connect", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0f, 0.0f)))) {
            CVarSetInteger("gArchipelago.Enabled", 1);
            CVarSave();
            Archipelago::ConnectFromCvars();
        }
    } else {
        if (UIWidgets::Button("Disconnect", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0f, 0.0f)))) {
            Archipelago::Disconnect();
        }
    }

    ImGui::SameLine();

    if (connected) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::Text("Connected");
    } else if (connecting) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("Connecting...");
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("Not Connected");
    }
    ImGui::PopStyleColor();

    ImGui::SeparatorText("Additional Options");
    if (UIWidgets::CVarCheckbox(
            "Death Link", "gArchipelago.DeathLink",
            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Tooltip("You die, others die.\nOthers die, you die!"))) {
        Archipelago::SetDeathLinkTag();
    }
    UIWidgets::CVarCheckbox(
        "Show External 2Ship Item", "gArchipelago.ShowExternal2ShipItem",
        UIWidgets::CheckboxOptions().Color(THEME_COLOR).Tooltip("If the item is from 2ship, blah blah"));

    UIWidgets::CVarSliderFloat("Console Scale", "gArchipelago.Console.Scale",
                               UIWidgets::FloatSliderOptions()
                                   .Color(THEME_COLOR)
                                   .Min(0.7f)
                                   .Max(2.5f)
                                   .DefaultValue(1.0f)
                                   .Step(0.1f)
                                   .Format("Scale: %.1f")
                                   .LabelPosition(UIWidgets::LabelPosition::None)
                                   .Tooltip("Scales the text in the Archipelago console."));

    ImGui::SeparatorText("Status Indicator");

    if (UIWidgets::Button("Default", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0f, 0.0f)))) {
        CVarSetFloat("gArchipelago.StatusIndicator.PosX", 15.0f);
        CVarSetFloat("gArchipelago.StatusIndicator.PosY", 45.0f);
        CVarSetFloat("gArchipelago.StatusIndicator.Scale", 1.0f);
        CVarSetInteger("gArchipelago.StatusIndicator.Hidden", 0);
        CVarSetInteger("gArchipelago.StatusIndicator.NeedsReset", 1);
        CVarSave();
    }
    ImGui::SameLine();

    UIWidgets::CVarCheckbox("Hidden", "gArchipelago.StatusIndicator.Hidden",
                            UIWidgets::CheckboxOptions()
                                .Color(THEME_COLOR)
                                .Tooltip("Hides the Archipelago connection status indicator overlay."));

    UIWidgets::CVarSliderFloat("Scale", "gArchipelago.StatusIndicator.Scale",
                               UIWidgets::FloatSliderOptions()
                                   .Color(THEME_COLOR)
                                   .Min(0.25f)
                                   .Max(4.0f)
                                   .DefaultValue(1.0f)
                                   .Format("Scale: %.2fx")
                                   .LabelPosition(UIWidgets::LabelPosition::None)
                                   .Tooltip("Size multiplier for the status indicator icon and text."));
}

void ArchipelagoSettingsWindow::InitElement() {
}
