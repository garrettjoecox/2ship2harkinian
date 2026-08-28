#pragma once
#include <libultraship/libultraship.h>

class ArchipelagoStatusWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {
    }
    void DrawElement() override;
    void UpdateElement() override {
    }
    void Draw() override;

  private:
    float mConnectedTime = 0.0f;
    int mLastStatus = 0;
};
