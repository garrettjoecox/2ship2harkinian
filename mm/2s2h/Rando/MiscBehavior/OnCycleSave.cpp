#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

// Can probably get use struct from z_sram_NES.c
typedef struct PersistentCycleSceneFlags {
    /* 0x0 */ u32 switch0;
    /* 0x4 */ u32 switch1;
    /* 0x8 */ u32 chest;
    /* 0xC */ u32 collectible;
} PersistentCycleSceneFlags;

extern "C" {
#include <variables.h>
extern PersistentCycleSceneFlags sPersistentCycleSceneFlags[SCENE_MAX];
}

SaveContext saveContextCopy;

void Rando::MiscBehavior::BeforeEndOfCycleSave() {
    memcpy(&saveContextCopy, &gSaveContext, sizeof(SaveContext));

    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        // If the item's repeat flag is NO, the flags will persist (only if the flag is chest, switch0, switch1, or
        // collectible) eg. RC_CLOCK_TOWN_STRAY_FAIRY is in switch3 so is untouched by this logic
        if (Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId].repeatFlag == REPEAT_NO) {
            switch (randoStaticCheck.flagType) {
                case FLAG_CYCL_SCENE_CHEST:
                    sPersistentCycleSceneFlags[randoStaticCheck.sceneId].chest |= (1 << randoStaticCheck.flag);
                    break;
                case FLAG_CYCL_SCENE_SWITCH:
                    if ((randoStaticCheck.flag & ~0x1F) >> 5 == 0) {
                        sPersistentCycleSceneFlags[randoStaticCheck.sceneId].switch0 |=
                            (1 << (randoStaticCheck.flag & 0x1F));
                    } else if ((randoStaticCheck.flag & ~0x1F) >> 5 == 1) {
                        sPersistentCycleSceneFlags[randoStaticCheck.sceneId].switch1 |=
                            (1 << (randoStaticCheck.flag & 0x1F));
                    }
                    break;
                case FLAG_CYCL_SCENE_COLLECTIBLE:
                    sPersistentCycleSceneFlags[randoStaticCheck.sceneId].collectible |= (1 << randoStaticCheck.flag);
                    break;
            }
            // might need to reset RANDO_SAVE_CHECKS[randoCheckId].eligible/obtained
        } else if (Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId].repeatFlag == REPEAT_YES) {
            switch (randoStaticCheck.flagType) {
                case FLAG_CYCL_SCENE_CHEST:
                    sPersistentCycleSceneFlags[randoStaticCheck.sceneId].chest &= ~(1 << randoStaticCheck.flag);
                    break;
                case FLAG_CYCL_SCENE_SWITCH:
                    if ((randoStaticCheck.flag & ~0x1F) >> 5 == 0) {
                        sPersistentCycleSceneFlags[randoStaticCheck.sceneId].switch0 &=
                            ~(1 << (randoStaticCheck.flag & 0x1F));
                    } else if ((randoStaticCheck.flag & ~0x1F) >> 5 == 1) {
                        sPersistentCycleSceneFlags[randoStaticCheck.sceneId].switch1 &=
                            ~(1 << (randoStaticCheck.flag & 0x1F));
                    }
                    break;
                case FLAG_CYCL_SCENE_COLLECTIBLE:
                    sPersistentCycleSceneFlags[randoStaticCheck.sceneId].collectible &= ~(1 << randoStaticCheck.flag);
                    break;
            }
        }
    }
}

void Rando::MiscBehavior::AfterEndOfCycleSave() {
    // for (int i = 0; i < 8; i++) {
    //     gSaveContext.save.saveInfo.inventory.dungeonItems[i] =
    //     saveContextCopy.save.saveInfo.inventory.dungeonItems[i]; gSaveContext.save.saveInfo.inventory.dungeonKeys[i]
    //     = saveContextCopy.save.saveInfo.inventory.dungeonKeys[i];
    //     gSaveContext.save.saveInfo.inventory.strayFairies[i] =
    //     saveContextCopy.save.saveInfo.inventory.strayFairies[i];
    // }

    // // Naively persist all cycle scene flags, we'll likely need to be more selective.
    // for (int i = 0; i < SCENE_MAX; i++) {
    //     gSaveContext.cycleSceneFlags[i].chest = saveContextCopy.cycleSceneFlags[i].chest;
    //     gSaveContext.cycleSceneFlags[i].collectible = saveContextCopy.cycleSceneFlags[i].collectible;
    //     gSaveContext.cycleSceneFlags[i].switch0 = saveContextCopy.cycleSceneFlags[i].switch0;
    //     gSaveContext.cycleSceneFlags[i].switch1 = saveContextCopy.cycleSceneFlags[i].switch1;
    // }

    // if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS]) {
    //     gSaveContext.save.saveInfo.skullTokenCount = saveContextCopy.save.saveInfo.skullTokenCount;
    // }
}
