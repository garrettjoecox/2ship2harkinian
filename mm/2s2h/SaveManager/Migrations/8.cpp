#include "2s2h/SaveManager/SaveManager.h"

// Add persisted Giants Mask Anywhere state.
void SaveManager_Migration_8(nlohmann::json& j) {
    if (!j["save"]["shipSaveInfo"].contains("giantsMaskAnywhereIsGiant")) {
        j["save"]["shipSaveInfo"]["giantsMaskAnywhereIsGiant"] = 0;
    }
}
