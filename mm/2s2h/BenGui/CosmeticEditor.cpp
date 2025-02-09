#include "2s2h/BenGui/UIWidgets.hpp"
#include "CosmeticEditor.h"
#include "macros.h"

std::vector<const char*> cosmeticEditorParentElements;

extern "C" {
Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);
Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                             s16 r, s16 g, s16 b, s16 a);
Gfx* Gfx_DrawTexRectIA16_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                    s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                    s16 a);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffset(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                         s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                         s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects);
}

CosmeticEditorElement cosmeticEditorElements[COSMETIC_ELEMENT_MAX] = {
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_NORMAL, "Hearts", "Normal", "Hearts.Normal", 255, 70, 50, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_NORMAL_BEATING, "Hearts", "Beating", "Hearts.Beating", 255, 70, 50,
                            255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_BORDER, "Hearts", "Border", "Hearts.Border", 50, 40, 60, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD, "Hearts", "DD", "Hearts.DD", 200, 0, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD_BEATING, "Hearts", "DD Beating", "Hearts.DDBeating", 200, 0, 0,
                            255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD_BORDER, "Hearts", "DD Border", "Hearts.DDBorder", 255, 255, 255,
                            255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_NORMAL, "Magic Meter", "Normal", "Magic.Normal", 0, 200, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_CHATEAU, "Magic Meter", "Chateau", "Magic.Chateau", 0, 0, 200, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_CONSUMED, "Magic Meter", "Active", "Magic.Active", 250, 250, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_BORDER, "Magic Meter", "Border", "Magic.Border", 255, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SMALL_KEY, "HUD", "Small Key", "HUD.SmallKey", 0, 200, 230, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_RUPEE_ICON, "HUD", "Rupee Icon", "HUD.RupeeIcon", 200, 255, 100, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MINIMAP, "HUD", "Minimap", "HUD.Minimap", 0, 255, 255, 160),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SPIN_SLASH_CHARGE, "Effects", "Spin Slash Charge",
                            "Effects.SpinSlashCharge", 170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SPIN_SLASH_BURST, "Effects", "Spin Slash Burst", "Effects.SpinSlashBurst",
                            170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_GREAT_SPIN_CHARGE, "Effects", "Great Spin Charge",
                            "Effects.GreatSpinCharge", 255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_GREAT_SPIN_BURST, "Effects", "Great Spin Burst", "Effects.GreatSpinBurst",
                            255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FIRE_ARROW_PRIMARY, "Effects", "Fire Arrow Primary",
                            "Effects.FireArrowPrim", 255, 200, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FIRE_ARROW_SECONDARY, "Effects", "Fire Arrow Secondary",
                            "Effects.FireArrowSec", 255, 0, 0, 128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_ICE_ARROW_PRIMARY, "Effects", "Ice Arrow Primary", "Effects.IceArrowPrim",
                            170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_ICE_ARROW_SECONDARY, "Effects", "Ice Arrow Secondary",
                            "Effects.IceArrowSec", 0, 0, 255, 128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_LIGHT_ARROW_PRIMARY, "Effects", "Light Arrow Primary",
                            "Effects.LightArrowPrim", 255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_LIGHT_ARROW_SECONDARY, "Effects", "Light Arrow Secondary",
                            "Effects.LightArrowSec", 255, 255, 0, 128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_B_BUTTON, "Buttons", "B", "Buttons.B", 100, 255, 120, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_A_BUTTON, "Buttons", "A", "Buttons.A", 100, 200, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_LEFT_BUTTON, "Buttons", "C Left", "Buttons.CLeft", 255, 240, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_DOWN_BUTTON, "Buttons", "C Down", "Buttons.CDown", 255, 240, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_RIGHT_BUTTON, "Buttons", "C Right", "Buttons.CRight", 255, 240, 0, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_D_PAD_BUTTON, "Buttons", "D Pad", "Buttons.DPad", 255, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_START_BUTTON, "Buttons", "Start", "Buttons.Start", 0xFF, 0x82, 0x3C, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FILE_SELECT_MENU, "Menus", "File Select Window", "Menus.FileWindow", 100,
                            150, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FILE_SELECT_PLATES, "Menus", "File Select Plates", "Menus.FilePlates", 100,
                            150, 255, 255)
};

void CopyFloatArray(CosmeticEditorElementID id, float currentColor[4], bool isChanged) {
    if (isChanged) {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[id].colorCvar, {});
        currentColor[0] = changedColor.r / 255.0f;
        currentColor[1] = changedColor.g / 255.0f;
        currentColor[2] = changedColor.b / 255.0f;
        currentColor[3] = 1.0f;
    } else {
        currentColor[0] = cosmeticEditorElements[id].defaultR / 255.0f;
        currentColor[1] = cosmeticEditorElements[id].defaultG / 255.0f;
        currentColor[2] = cosmeticEditorElements[id].defaultB / 255.0f;
        currentColor[3] = cosmeticEditorElements[id].defaultA / 255.0f;
    }
};

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    CosmeticEditorElement element = cosmeticEditorElements[elementId];

    Color_RGBA8 returnedColor;

    if (CVarGetInteger(element.colorChangedCvar, false)) {
        Color_RGBA8 changedColor = CVarGetColor(element.colorCvar, {});
        returnedColor.r = static_cast<uint8_t>(changedColor.r);
        returnedColor.g = static_cast<uint8_t>(changedColor.g);
        returnedColor.b = static_cast<uint8_t>(changedColor.b);
        returnedColor.a = static_cast<uint8_t>(255);
    } else {
        returnedColor.r = r;
        returnedColor.g = g;
        returnedColor.b = b;
        returnedColor.a = a;
    }

    return returnedColor;
}

extern "C" void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    gDPSetEnvColor(pkt, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    gDPSetPrimColor(pkt, m, l, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                      s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                      u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA8_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                         rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawRect_DropShadow(pkt, rectLeft, rectTop, rectWidth, rectHeight, dsdx, dtdy, setColor.r, setColor.g,
                                   setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                       s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                       s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                       u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA16_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                          rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                            s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                            s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                                            s16 a, s32 masks, s32 rects, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA8_DropShadowOffset(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                               rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a, masks,
                                               rects);
}

void CosmeticEditorRandomizeElement(CosmeticEditorElement element) {
    Color_RGBA8 colorSelected;
    colorSelected.r = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.g = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.b = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.a = static_cast<uint8_t>(255);

    CVarSetColor(element.colorCvar, colorSelected);
    CVarSetInteger(element.colorChangedCvar, true);
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void CosmeticEditorRandomizeAllElements() {
    for (auto& element : cosmeticEditorElements) {
        CosmeticEditorRandomizeElement(element);
    }
}

void CosmeticEditorResetAllElements() {
    for (auto& element : cosmeticEditorElements) {
        CVarClear(element.colorCvar);
        CVarClear(element.colorChangedCvar);
    }
}

void CosmeticEditorDrawColorTab() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    std::string resetAllText = ICON_FA_UNDO;
    resetAllText += " All";
    std::string randomAllText = ICON_FA_RECYCLE;
    randomAllText += " All";
    if (ImGui::Button(resetAllText.c_str())) {
        CosmeticEditorResetAllElements();
    }
    UIWidgets::Tooltip("Resets All Elements to their Defaults");
    ImGui::SameLine();
    if (ImGui::Button(randomAllText.c_str())) {
        CosmeticEditorRandomizeAllElements();
    }
    UIWidgets::Tooltip("Randomizes All Elements");
    for (auto& parent : cosmeticEditorParentElements) {
        ImGui::SeparatorText(parent);
        ImGui::BeginTable(parent, 2);
        ImGui::TableSetupColumn("Element Name", ImGuiTableColumnFlags_WidthStretch, 1.4f);
        ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        for (auto& entry : cosmeticEditorElements) {
            if (parent != entry.parentName) {
                continue;
            }
            float currentColor[4];

            ImGui::PushID(entry.id);
            ImGui::TableNextColumn();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
                                 ((ImGui::GetFrameHeight() - (ImGui::CalcTextSize(entry.name).y)) / 2.0f));
            ImGui::Text("%s", entry.name);
            ImGui::TableNextColumn();
            CopyFloatArray(entry.id, currentColor, CVarGetInteger(entry.colorChangedCvar, false));
            bool colorChanged =
                ImGui::ColorEdit3("Color", currentColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (colorChanged) {
                Color_RGBA8 colorSelected;
                colorSelected.r = static_cast<uint8_t>(currentColor[0] * 255.0f);
                colorSelected.g = static_cast<uint8_t>(currentColor[1] * 255.0f);
                colorSelected.b = static_cast<uint8_t>(currentColor[2] * 255.0f);
                colorSelected.a = static_cast<uint8_t>(255);

                CVarSetColor(entry.colorCvar, colorSelected);
                CVarSetInteger(entry.colorChangedCvar, true);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_UNDO, ImVec2(27.0f, 27.0f))) {
                CVarClear(entry.colorCvar);
                CVarClear(entry.colorChangedCvar);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_RECYCLE, ImVec2(27.0f, 27.0f))) {
                CosmeticEditorRandomizeElement(entry);
            }
            ImGui::SameLine();
            ImGui::TextColored(CVarGetInteger(entry.colorChangedCvar, 0)
                                   ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                   : UIWidgets::ColorValues.at(UIWidgets::Colors::Gray),
                               CVarGetInteger(entry.colorChangedCvar, 0) ? "Modified" : "Default");
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleColor(3);
}

// Tab Bar is unused until other options are available.
void CosmeticEditorDrawTabBar() {
    ImGui::BeginTabBar("Cosmetic Tab Bar");
    if (ImGui::BeginTabItem("Colors")) {
        CosmeticEditorDrawColorTab();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}

void CosmeticEditorWindow::DrawElement() {
    CosmeticEditorDrawColorTab();
}

void CosmeticEditorWindow::InitElement() {
    cosmeticEditorParentElements.clear();
    for (auto& element : cosmeticEditorElements) {
        if (std::find(cosmeticEditorParentElements.begin(), cosmeticEditorParentElements.end(), element.parentName) !=
            cosmeticEditorParentElements.end()) {
            continue;
        }
        cosmeticEditorParentElements.push_back(element.parentName);
    }
}
