#include "ArchipelagoConsoleWindow.h"
#include "Archipelago.h"
#include "ExportYaml.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "2s2h/BenGui/BenMenu.h"
#include <imgui.h>
#include <filesystem>
#include <string>
#include <cstring>
#include "2s2h/Network/Archipelago/Archipelago.h"

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
} // namespace BenGui

using namespace UIWidgets;

static void DrawArchipelagoMenu() {
    UIWidgets::CVarCheckbox("Enable Archipelago for new saves", "gArchipelago.Enabled",
                            UIWidgets::CheckboxOptions()
                                .Color(THEME_COLOR)
                                .Tooltip("When enabled, creating a new file will mark it as an Archipelago save.\n"
                                         "Existing saves are not changed.\n\n"
                                         "Note: Archipelago will override Randomizer mode."));

    ImGui::SeparatorText("Connection info");

    // Connect / Disconnect button + status (match SoH placement)
    const bool connected = Archipelago::Instance->IsConnected();
    const bool connecting = Archipelago::Instance->GetState() >= 1 && Archipelago::Instance->GetState() <= 3;

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
    UIWidgets::CVarInputString("##ArchipelagoPassword", "gArchipelago.Password",
                               UIWidgets::InputOptions()
                                   .Color(THEME_COLOR)
                                   .IsSecret(true)
                                   .Size(ImVec2(ImGui::GetFontSize() * 15, 0))
                                   .LabelPosition(UIWidgets::LabelPosition::None));

    ImGui::EndDisabled();

    ImGui::PopStyleColor();
    UIWidgets::PopStyleCombobox();

    if (!connected && !connecting) {
        if (UIWidgets::Button("Connect", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0f, 0.0f)))) {
            CVarSetInteger("gArchipelago.Enabled", 1);
            CVarSave();
            Archipelago::Instance->Enable();
        }
    } else {
        if (UIWidgets::Button("Disconnect", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.0f, 0.0f)))) {
            Archipelago::Instance->Disable();
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

    ImGui::SeparatorText("Player Options File");

    static std::string exportedPath;
    static std::string exportError;

    if (UIWidgets::Button(
            "Export Current Rando Settings as YAML",
            UIWidgets::ButtonOptions()
                .Color(THEME_COLOR)
                .Size(ImVec2(0.0f, 0.0f))
                .Tooltip("Writes your current Randomizer settings to an Archipelago options file, so you don't have "
                         "to fill one in by hand, then opens the folder it wrote it to.\nOptions that only exist in "
                         "Archipelago keep their defaults."))) {
        exportedPath.clear();
        exportError.clear();
        if (ArchipelagoYaml::Export(exportedPath, exportError)) {
            SDL_OpenURL(std::string("file:///" + std::filesystem::path(exportedPath).parent_path().string()).c_str());
        }
    }

    if (!exportedPath.empty()) {
        ImGui::TextWrapped("Saved to %s", exportedPath.c_str());
    }

    if (!exportError.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::TextWrapped("%s", exportError.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::SeparatorText("Additional Options");
    if (UIWidgets::CVarCheckbox(
            "Death Link", "gArchipelago.DeathLink",
            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Tooltip("You die, others die.\nOthers die, you die!"))) {
        Archipelago::Instance->UpdateDeathLinkTag();
    }

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

static RegisterMenuInitFunc initFunc([]() {
    // BenGui::mBenMenu->AddMenuEntry("Network", "gSettings.Menu.NetworkSidebarSection");

    BenGui::mBenMenu->AddSidebarEntry("Network", "Archipelago", 2);

    {
        WidgetPath left = { "Network", "Archipelago", SECTION_COLUMN_1 };
        BenGui::mBenMenu->AddWidget(left, "Settings", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
            DrawArchipelagoMenu();
        });
    }

    {
        WidgetPath right = { "Network", "Archipelago", SECTION_COLUMN_2 };
        BenGui::mBenMenu->AddWidget(right, "Console", WIDGET_WINDOW_BUTTON)
            .CVar("gWindows.ArchipelagoConsole")
            .WindowName("Archipelago Console");
    }
});
