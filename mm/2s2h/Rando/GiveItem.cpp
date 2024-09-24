#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
}

void Rando::GiveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
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
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
            gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
            gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            break;
        // Don't love this because it doesn't hit GameInteractor_ExecuteOnItemGive()
        // but not sure how else to account for collecting outside of dungeon
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_MAP:
        case RI_WOODFALL_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_INDEX_WOODFALL_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_WOODFALL_TEMPLE) = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_WOODFALL_TEMPLE)++;
            }
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_MAP:
        case RI_SNOWHEAD_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_INDEX_SNOWHEAD_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_SNOWHEAD_TEMPLE) = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_SNOWHEAD_TEMPLE)++;
            }
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_MAP:
        case RI_GREAT_BAY_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_INDEX_GREAT_BAY_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_GREAT_BAY_TEMPLE) = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_GREAT_BAY_TEMPLE)++;
            }
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_MAP:
        case RI_STONE_TOWER_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_INDEX_STONE_TOWER_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_STONE_TOWER_TEMPLE) = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_INDEX_STONE_TOWER_TEMPLE)++;
            }
            break;
        // Technically these should never be used, but leaving them here just in case
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
            Rando::GiveItem(Rando::ConvertItem(randoItemId));
            break;
        default:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
    }
}
