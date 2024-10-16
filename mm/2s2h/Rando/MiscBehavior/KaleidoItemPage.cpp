#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "z64interface.h"
#include "variables.h"
#include "functions.h"
#include <macros.h>
}

// Currently this enables a simple "press a" to cycle through the available trade items
void Rando::MiscBehavior::InitKaleidoItemPage() {
    static uint32_t shouldHook1 = 0;
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::ShouldVanillaBehavior>(shouldHook1);
    shouldHook1 = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHook1 = REGISTER_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, {
        ItemId* itemId = va_arg(args, ItemId*);

        if (SLOT(*itemId) != SLOT_TRADE_COUPLE && SLOT(*itemId) != SLOT_TRADE_DEED &&
            SLOT(*itemId) != SLOT_TRADE_KEY_MAMA) {
            return;
        }

        *should = false;

        // Build list of available items
        std::vector<u8> availableItems;
        switch (SLOT(*itemId)) {
            case SLOT_TRADE_COUPLE:
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES)) {
                    availableItems.push_back(ITEM_PENDANT_OF_MEMORIES);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)) {
                    availableItems.push_back(ITEM_LETTER_TO_KAFEI);
                }
                break;
            case SLOT_TRADE_DEED:
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) {
                    availableItems.push_back(ITEM_MOONS_TEAR);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)) {
                    availableItems.push_back(ITEM_DEED_LAND);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP)) {
                    availableItems.push_back(ITEM_DEED_SWAMP);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN)) {
                    availableItems.push_back(ITEM_DEED_MOUNTAIN);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN)) {
                    availableItems.push_back(ITEM_DEED_OCEAN);
                }
                break;
            case SLOT_TRADE_KEY_MAMA:
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)) {
                    availableItems.push_back(ITEM_ROOM_KEY);
                }
                if (Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA)) {
                    availableItems.push_back(ITEM_LETTER_MAMA);
                }
                break;
        }

        // get current index
        int index = -1;
        for (int i = 0; i < availableItems.size(); i++) {
            if (availableItems[i] == *itemId) {
                index = i;
                break;
            }
        }

        if (index == -1 || availableItems.size() == 0) {
            return;
        }

        // Display the item
        Inventory_ReplaceItem(gPlayState, (u8)*itemId, availableItems[(index + 1) % availableItems.size()]);
    });
}
