#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
}

void Rando::RemoveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_SNOWHEAD_TEMPLE]--;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_GREAT_BAY_TEMPLE]--;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_STONE_TOWER_TEMPLE]--;
            break;
        case RI_GREAT_SPIN_ATTACK:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = false;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
            break;
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = false;
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
            break;
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_TEMPLE_MAP:
        case RI_WOODFALL_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_WOODFALL_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_TEMPLE_MAP:
        case RI_SNOWHEAD_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_SNOWHEAD_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_TEMPLE_MAP:
        case RI_GREAT_BAY_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_GREAT_BAY_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_TEMPLE_MAP:
        case RI_STONE_TOWER_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_STONE_TOWER_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
            // TODO
            break;
        case RI_GS_TOKEN_SWAMP: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINSTA1);
            gSaveContext.save.saveInfo.skullTokenCount =
                ((int)(skullTokenCount & 0xFFFF) << 0x10) | (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
            break;
        }
        case RI_GS_TOKEN_OCEAN: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINDAN2);
            gSaveContext.save.saveInfo.skullTokenCount =
                (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) | (skullTokenCount & 0xFFFF);
            break;
        }
        case RI_MOONS_TEAR:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
            break;
        case RI_DEED_LAND:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
            break;
        case RI_DEED_SWAMP:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
            break;
        case RI_DEED_MOUNTAIN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
            break;
        case RI_DEED_OCEAN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
            break;
        case RI_ROOM_KEY:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
            break;
        case RI_LETTER_TO_MAMA:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
            break;
        case RI_LETTER_TO_KAFEI:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
            break;
        case RI_PENDANT_OF_MEMORIES:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
            break;
        default:
            // TODO: Handle equipment
            Inventory_DeleteItem(Rando::StaticData::Items[randoItemId].itemId,
                                 SLOT(Rando::StaticData::Items[randoItemId].itemId));
            break;
    }
}
