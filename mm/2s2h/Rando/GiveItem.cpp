#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
}

void Rando::GiveItem(RandoItem item) {
    switch (item) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_WOODFALL_TEMPLE]++;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_SNOWHEAD_TEMPLE]++;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_GREAT_BAY_TEMPLE]++;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_STONE_TOWER_TEMPLE]++;
            break;
        case RI_GREAT_SPIN_ATTACK:
            SET_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = true;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
            break;
        case RI_PROGRESSIVE_MAGIC:
            if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
                gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            } else {
                gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
                gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
                gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            }
            break;
        // probably case here for dungeon items map/compass/keys
        default:
            Item_Give(gPlayState, Rando::StaticData::Items[item].itemId);
            break;
    }
}
