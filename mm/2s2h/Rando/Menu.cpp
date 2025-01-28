#include "Rando/Rando.h"
#include "Rando/Spoiler/Spoiler.h"
#include <libultraship/libultraship.h>
#include "2s2h/BenGui/UIWidgets.hpp"
#include "Rando/CheckTracker/CheckTracker.h"
#include "BenPort.h"
#include "build.h"
#include "2s2h/BenGui/BenMenu.h"

// TODO: This block should come from elsewhere, tied to data in Rando::StaticData::Options
std::unordered_map<int32_t, const char*> logicOptions = {
    { RO_LOGIC_GLITCHLESS, "Glitchless" },
    { RO_LOGIC_NO_LOGIC, "No Logic" },
    { RO_LOGIC_NEARLY_NO_LOGIC, "Nearly No Logic" },
    { RO_LOGIC_FRENCH_VANILLA, "French Vanilla" },
    { RO_LOGIC_VANILLA, "Vanilla" },
};

namespace BenGui {
extern std::shared_ptr<Rando::CheckTracker::CheckTrackerWindow> mRandoCheckTrackerWindow;
extern std::shared_ptr<Rando::CheckTracker::SettingsWindow> mRandoCheckTrackerSettingsWindow;
extern std::shared_ptr<BenMenu> mBenMenu;
} // namespace BenGui

using namespace BenGui;
using namespace UIWidgets;

extern "C" {
#include "archives/icon_item_24_static/icon_item_24_static_yar.h"
}

static void DrawGeneralTab() {
    ImGui::BeginChild("randoSettings", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0));
    ImGui::SeparatorText("Seed Generation");
    UIWidgets::CVarCheckbox("Enable Rando (Randomizes new files upon creation)", "gRando.Enabled");

    if (UIWidgets::CVarCombobox("Seed", "gRando.SpoilerFileIndex", Rando::Spoiler::spoilerOptions)) {
        if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
            CVarSetString("gRando.SpoilerFile", "");
        } else {
            CVarSetString("gRando.SpoilerFile",
                          Rando::Spoiler::spoilerOptions[CVarGetInteger("gRando.SpoilerFileIndex", 0)].c_str());
        }
    }

    if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
        UIWidgets::PushStyleSlider();
        static char seed[256];
        std::string stringSeed = CVarGetString("gRando.InputSeed", "");
        strcpy(seed, stringSeed.c_str());
        ImGui::InputText("##Seed", seed, sizeof(seed), ImGuiInputTextFlags_CallbackAlways,
                         [](ImGuiInputTextCallbackData* data) {
                             CVarSetString("gRando.InputSeed", data->Buf);
                             return 0;
                         });
        if (stringSeed.length() < 1) {
            ImGui::SameLine(17.0f);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "Leave blank for random seed");
        }
        UIWidgets::PopStyleSlider();

        UIWidgets::CVarCheckbox("Generate Spoiler File", "gRando.GenerateSpoiler");
    }
    ImGui::SeparatorText("Enhancements");
    UIWidgets::CVarCheckbox("Container Style Matches Contents", "gRando.CSMC");
    UIWidgets::Tooltip("This will make the contents of a container match the container itself. This currently only "
                       "applies to chests and pots");
    UIWidgets::WindowButton("Check Tracker", "gWindows.CheckTracker", BenGui::mRandoCheckTrackerWindow,
                            { .size = ImVec2((ImGui::GetContentRegionAvail().x - 48.0f), 40.0f) });
    ImGui::SameLine();
    if (UIWidgets::Button(ICON_FA_COG, { .size = ImVec2(40.0f, 40.0f) })) {
        BenGui::mRandoCheckTrackerSettingsWindow->ToggleVisibility();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoDisclaimer");
    ImGui::PushStyleColor(ImGuiCol_Text, ColorValues.at(Colors::Gray));
    if (gGitCommitTag[0] == 0) {
        ImGui::Text("%s | %s", (char*)gGitBranch, (char*)gGitCommitHash);
    } else {
        ImGui::Text("%s", (char*)gBuildVersion);
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text, ColorValues.at(Colors::Orange));
    ImGui::SeparatorText("Disclaimer");
    ImGui::PopStyleColor();
    ImGui::TextWrapped(
        "This is an Alpha. Please make note of any odd or unexpected behavior while you are playing. While we are in "
        "the earlier phases of this project, some things you may encounter are:\n- X Check is not shuffled\n- X "
        "Cutscene is not skipped\n- Soft lock when interacting with X\n- Unbeatable seed (glitchless logic)\n\nWe are "
        "aware of some of these, but likely not all. Please compare your findings to our list of known issues, which "
        "is available in the pins of the Rando Alpha Discord thread, or on the Github Issue #211, and let us know if "
        "you encounter any new issues.\n\nExplore the menus for various enhancements and time savers, they are not "
        "enabled by default in Rando.\n\n");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
    ImGui::SeparatorText("Thank You");
    ImGui::PopStyleColor();
    ImGui::TextWrapped("Special thanks to BalloonDude, Eblo, Caladius, Sitton, Dana, our playtesters, everyone who "
                       "contributed to the SoH randomizer, and the creators of the various other randomizer "
                       "implementations that inspired this project. I hope you enjoy it.\n\n");
    ImTextureID swordTextureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
        (const char*)gQuestIconHeartContainer2Tex);
    ImGui::Image(swordTextureId, ImVec2(25.0f, 25.0f));
    ImGui::SameLine();
    ImGui::Text("ProxySaw");
    ImGui::EndChild();
}

static void DrawLogicConditionsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = ImGui::GetContentRegionAvail().y / 3 - (ImGui::GetStyle().ItemSpacing.y * 2);
    ImGui::BeginChild("randoLogicColumn1", ImVec2(columnWidth, halfHeight));
    UIWidgets::CVarCombobox("Logic", Rando::StaticData::Options[RO_LOGIC].cvar, logicOptions);
    UIWidgets::Tooltip(
        "Glitchless - The items are shuffled in a way that guarantees the seed is beatable without "
        "glitches\n\n"
        "No Logic - The items are shuffled completely randomly, this can result in unbeatable seeds, and "
        "will require heavy use of glitches\n\n"
        "Nearly No Logic - The items are shuffled completely randomly, with the following exceptions:\n"
        "- Oath to Order and Remains cannot be placed on the Moon\n"
        "- Deku Mask, Zora Mask, Sonata, and Bossa Nova cannot be placed in their respective Temples or on "
        "the Moon\n\n"
        "French Vanilla - This is an alternative variant to Glitchless, but the items are biased to be "
        "closer to their vanilla locations. Tends to be an more beginner friendly experience.\n\n"
        "Vanilla - The items are not shuffled.");
    ImGui::EndChild();
    ImGui::BeginChild("randoLogicTricks", ImVec2(0, 0));
    ImGui::SeparatorText("Tricks & Glitches");
    ImGui::EndChild();
}

static void DrawLocationsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = ImGui::GetContentRegionAvail().y / 2 - (ImGui::GetStyle().ItemSpacing.y * 2);
    ImGui::SeparatorText("Shuffle Options");
    ImGui::BeginChild("randoLocationsColumn1", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Shuffle Songs", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Shuffle Owl Statues", Rando::StaticData::Options[RO_SHUFFLE_OWL_STATUES].cvar);
    CVarCheckbox("Shuffle Shops", Rando::StaticData::Options[RO_SHUFFLE_SHOPS].cvar);
    CVarCheckbox("Shuffle Boss Remains", Rando::StaticData::Options[RO_SHUFFLE_BOSS_REMAINS].cvar);
    CVarCheckbox("Shuffle Cows", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Shuffle Gold Skulltula Tokens", Rando::StaticData::Options[RO_SHUFFLE_GOLD_SKULLTULAS].cvar);
    CVarSliderInt(
        "Shuffle Gold Skulltula Tokens", "gPlaceholderInt",
        IntSliderOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).Min(1).Max(30).DefaultValue(30));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoLocationsColumn2", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Shuffle Mundane Checks", Rando::StaticData::Options[RO_SHUFFLE_MUNDANE].cvar);
    CVarCheckbox("Shuffle Pot Drops", Rando::StaticData::Options[RO_SHUFFLE_MUNDANE].cvar,
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Crate Drops", Rando::StaticData::Options[RO_SHUFFLE_MUNDANE].cvar,
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Barrel Drops", Rando::StaticData::Options[RO_SHUFFLE_MUNDANE].cvar,
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Hive Drops", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Freestanding Items", Rando::StaticData::Options[RO_SHUFFLE_MUNDANE].cvar,
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Wonder Items", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoLocationsColumn3", ImVec2(columnWidth, halfHeight));
    ImGui::EndChild();
    ImGui::BeginChild("randoLocationsExclusions", ImVec2(0, 0));
    ImGui::SeparatorText("Exclusions");
    ImGui::TextWrapped("These checks will be gauranteed junk items, and marked as skipped in the check tracker.");
    ImGui::EndChild();
}

static void DrawItemsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = ImGui::GetContentRegionAvail().y / 3 - (ImGui::GetStyle().ItemSpacing.y * 2);
    ImGui::BeginChild("randoItemsColumn1", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Bronze Scale", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Deku Stick Bag", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Deku Nut Bag", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Skeleton Key", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Child Wallet", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Infinite Upgrades", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoItemsColumn2", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Boss Souls", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Enemy Souls", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoItemsColumn3", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Song of Double Time", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Inverted Song of Time", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Saria's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Sun's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::BeginChild("randoItemsStarting", ImVec2(0, 0));
    ImGui::SeparatorText("Starting Items");
    CVarSliderInt(
        "Starting Health", "gPlaceholderInt",
        IntSliderOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).Min(3).Max(20).DefaultValue(3));
    ImGui::EndChild();
}

static void DrawHintsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = ImGui::GetContentRegionAvail().y / 2 - (ImGui::GetStyle().ItemSpacing.y * 2);
    ImGui::BeginChild("randoHintsColumn1", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Spider House", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Happy Mask Salesman Purchaseable", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Gossip Stones", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Boss Remains", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Saria's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Song of Soaring", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Hookshot Location", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
}

void Rando::RegisterMenu() {
    mBenMenu->AddSidebarEntry("Rando", "General", 1);
    WidgetPath path = { "Rando", "General", 1 };
    mBenMenu->AddWidget(path, "General", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawGeneralTab(); });
    mBenMenu->AddSidebarEntry("Rando", "Logic/Conditions", 1);
    path.sidebarName = "Logic/Conditions";
    mBenMenu->AddWidget(path, "Logic/Conditions", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        DrawLogicConditionsTab();
    });
    mBenMenu->AddSidebarEntry("Rando", "Locations", 1);
    path.sidebarName = "Locations";
    mBenMenu->AddWidget(path, "Locations", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawLocationsTab(); });
    mBenMenu->AddSidebarEntry("Rando", "Items", 1);
    path.sidebarName = "Items";
    mBenMenu->AddWidget(path, "Items", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawItemsTab(); });
    mBenMenu->AddSidebarEntry("Rando", "Hints", 1);
    path.sidebarName = "Hints";
    mBenMenu->AddWidget(path, "Hints", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawHintsTab(); });
}
