#pragma once
#include <libultraship/libultraship.h>
#include <imgui.h>
#include "ArchipelagoTypes.h"

void ArchipelagoConsole_SendMessage(const char* fmt, ...);
void ArchipelagoConsole_PrintJson(const std::vector<AP_Text::ColoredTextNode> nodes);

class ArchipelagoConsoleWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {
    }
    void DrawElement() override;
    void UpdateElement() override {
    }

  private:
    ImVec4 getColorVal(AP_Text::TextColor color);
};
