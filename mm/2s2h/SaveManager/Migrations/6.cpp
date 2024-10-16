#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

void SaveManager_Migration_6(nlohmann::json& j) {
    // if saveType doesn't exist, create it and set it to vanilla
    if (!j["save"]["shipSaveInfo"].contains("saveType")) {
        j["save"]["shipSaveInfo"]["saveType"] = SAVETYPE_VANILLA;
    }
}
