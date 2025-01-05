#include <libultraship/libultraship.h>

#define TOTAL_GAMEPLAY_TIME ((GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100)

void DisplayOverlayInitSettings();

class DisplayOverlayWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};