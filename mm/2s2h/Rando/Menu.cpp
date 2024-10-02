#include "Rando/Rando.h"
#include "Rando/Spoiler/Spoiler.h"
#include <libultraship/libultraship.h>
#include "2s2h/BenGui/UIWidgets.hpp"
#include "BenPort.h"

// TODO: This block should come from elsewhere, tied to data in Rando::StaticData::Options
std::vector<std::string> logicOptions = { "No Logic", "Vanilla" };

void Rando::DrawMenu() {
    if (UIWidgets::BeginMenu("Rando", UIWidgets::Colors::Green)) {
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

            UIWidgets::CVarCombobox("Logic", Rando::StaticData::Options[RO_LOGIC].cvar, logicOptions);

            if (CVarGetInteger(Rando::StaticData::Options[RO_LOGIC].cvar, RO_LOGIC_NO_LOGIC) != RO_LOGIC_VANILLA) {
                UIWidgets::CVarCheckbox("Shuffle Gold Skulltula Tokens",
                                        Rando::StaticData::Options[RO_SHUFFLE_GOLD_SKULLTULAS].cvar);
                UIWidgets::CVarCheckbox("Shuffle Pots", Rando::StaticData::Options[RO_SHUFFLE_POTS].cvar);
            }
        }

        ImGui::EndMenu();
    }
}
