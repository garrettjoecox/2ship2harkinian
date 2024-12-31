#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "z64interface.h"
#include "variables.h"
#include "functions.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include <macros.h>
}

// Currently this enables a simple "press a" to cycle through the available trade items
void Rando::MiscBehavior::InitKaleidoItemPage() {
    COND_HOOK(OnKaleidoUpdate, IS_RANDO, [](PauseContext* pauseCtx) {
        if ((pauseCtx->state != PAUSE_STATE_MAIN) || (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE) ||
            (pauseCtx->pageIndex != PAUSE_ITEM)) {
            return;
        }

        u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];
        ItemId itemId = (ItemId)pauseCtx->cursorItem[PAUSE_ITEM];
        if (slot != SLOT_TRADE_COUPLE && slot != SLOT_TRADE_DEED && slot != SLOT_TRADE_KEY_MAMA) {
            return;
        }

        if (!CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_A)) {
            return;
        }

        // Build list of available items
        std::vector<u8> availableItems;
        switch (slot) {
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

        if (availableItems.size() == 0) {
            return;
        }

        if (itemId == PAUSE_ITEM_NONE) {
            INV_CONTENT(availableItems.at(0)) = availableItems.at(0);
            return;
        }

        // get current index
        int index = -1;
        for (int i = 0; i < availableItems.size(); i++) {
            if (availableItems[i] == itemId) {
                index = i;
                break;
            }
        }

        // Display the item
        Inventory_ReplaceItem(gPlayState, (u8)itemId, availableItems[(index + 1) % availableItems.size()]);
    });

    COND_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, IS_RANDO, {
        PauseContext* pauseCtx = &gPlayState->pauseCtx;
        u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];

        if (slot != SLOT_TRADE_COUPLE && slot != SLOT_TRADE_DEED && slot != SLOT_TRADE_KEY_MAMA) {
            return;
        }

        *should = false;
    });
}
