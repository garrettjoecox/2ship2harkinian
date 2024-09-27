#include "Rando/Rando.h"
#include <libultraship/libultraship.h>
#include "2s2h/BenGui/UIWidgets.hpp"
#include "spdlog/spdlog.h"

void Rando::DrawMenu() {
    if (UIWidgets::BeginMenu("Rando", UIWidgets::Colors::Green)) {
        UIWidgets::CVarCheckbox("Randomize File On Creation", "gRando.Enabled");
        UIWidgets::PushStyleSlider();
        static char seed[256];
        std::string stringSeed = CVarGetString("gRando.InputSeed", "");
        strcpy(seed, stringSeed.c_str());
        ImGui::InputText("Seed", seed, sizeof(seed), ImGuiInputTextFlags_CallbackAlways, [](ImGuiInputTextCallbackData* data) {
            CVarSetString("gRando.InputSeed", data->Buf);
            return 0;
        });
        UIWidgets::PopStyleSlider();

        ImGui::EndMenu();
    }
}
