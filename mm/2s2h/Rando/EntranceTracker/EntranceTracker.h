#ifndef RANDO_ENTRANCE_TRACKER_H
#define RANDO_ENTRANCE_TRACKER_H

#include "Rando/Rando.h"
#include <ship/window/gui/GuiWindow.h>

namespace Rando {

namespace EntranceTracker {

void Init();
void OnFileLoad();

// Get human-readable name for an entrance
std::string GetEntranceName(s32 entranceId);

// Discovery tracking
void SetEntranceDiscovered(s32 entranceId);
bool IsEntranceDiscovered(s32 entranceId);

class EntranceTrackerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override{};
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};

class EntranceTrackerSettingsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};

} // namespace EntranceTracker

} // namespace Rando

#endif // RANDO_ENTRANCE_TRACKER_H
