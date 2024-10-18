#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include <variables.h>
}

SaveContext saveContextCopy;

void Rando::MiscBehavior::BeforeEndOfCycleSave() {
    memcpy(&saveContextCopy, &gSaveContext, sizeof(SaveContext));
}

void Rando::MiscBehavior::AfterEndOfCycleSave() {
    for (int i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.inventory.dungeonItems[i] = saveContextCopy.save.saveInfo.inventory.dungeonItems[i];
        gSaveContext.save.saveInfo.inventory.dungeonKeys[i] = saveContextCopy.save.saveInfo.inventory.dungeonKeys[i];
        gSaveContext.save.saveInfo.inventory.strayFairies[i] = saveContextCopy.save.saveInfo.inventory.strayFairies[i];
    }

    // Naively persist all cycle scene flags, we'll likely need to be more selective.
    for (int i = 0; i < SCENE_MAX; i++) {
        gSaveContext.cycleSceneFlags[i].chest = saveContextCopy.cycleSceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].collectible = saveContextCopy.cycleSceneFlags[i].collectible;
        gSaveContext.cycleSceneFlags[i].switch0 = saveContextCopy.cycleSceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 = saveContextCopy.cycleSceneFlags[i].switch1;
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS]) {
        gSaveContext.save.saveInfo.skullTokenCount = saveContextCopy.save.saveInfo.skullTokenCount;
    }
}
