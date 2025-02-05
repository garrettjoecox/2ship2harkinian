#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Tab/z_en_tab.h"

s32 func_80BE0D38(Actor* thisx, PlayState* play); // Standard dialogue mscript callback that does bottle check
}

enum TabDialogueState {
    TAB_D_IDLE,
    TAB_D_INITIAL_DIALOGUE,
    TAB_D_SHOP_DIALOGUE,
};

MsgScript standardDialogueOverrideScript[] = {
    /* 0x0000 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0A),
    /* 0x0003 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0004 0x07 */ MSCRIPT_BRANCH_ON_TEXT_CHOICE(0x006D - 0x000B, 0x0, 0x006D - 0x000B),
    /* 0x000B 0x01 */ MSCRIPT_PLAY_DECIDE(),
    /* 0x000C 0x03 */ MSCRIPT_BRANCH_ON_CALLBACK_2(0x0014 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0E),
    /* 0x0012 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0013 0x01 */ MSCRIPT_DONE(),

    /* 0x0014 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0B),
    /* 0x0017 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0018 0x07 */ MSCRIPT_BRANCH_ON_TEXT_CHOICE(0x0, 0x0031 - 0x001F, 0x0043 - 0x001F),
    /* 0x001F 0x03 */ MSCRIPT_BRANCH_ON_CALLBACK_2(0x0029 - 0x0022),
    /* 0x0022 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0E),
    /* 0x0025 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0026 0x03 */ MSCRIPT_JUMP(0x0048 - 0x0029),
    /* 0x0029 0x05 */ MSCRIPT_BRANCH_ON_RUPEES(0x0014, 0x005E - 0x002E),
    /* 0x002E 0x03 */ MSCRIPT_JUMP(0x0049 - 0x0031),
    /* 0x0031 0x03 */ MSCRIPT_BRANCH_ON_CALLBACK_2(0x003B - 0x0034),
    /* 0x0034 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0E),
    /* 0x0037 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_JUMP(0x0048 - 0x003B),
    /* 0x003B 0x05 */ MSCRIPT_BRANCH_ON_RUPEES(0x00C8, 0x004F - 0x0040),
    /* 0x0040 0x03 */ MSCRIPT_JUMP(0x0049 - 0x0043),
    /* 0x0043 0x01 */ MSCRIPT_PLAY_CANCEL(),
    /* 0x0044 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0D),
    /* 0x0047 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0048 0x01 */ MSCRIPT_DONE(),

    /* 0x0049 0x01 */ MSCRIPT_PLAY_ERROR(),
    /* 0x004A 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0C),
    /* 0x004D 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x004E 0x01 */ MSCRIPT_DONE(),

    /* 0x004F 0x01 */ MSCRIPT_PLAY_DECIDE(),
    /* 0x0050 0x01 */ MSCRIPT_CLOSE_TEXT(),
    /* 0x0051 0x03 */ MSCRIPT_CHANGE_RUPEES(0xFF38),
    /* 0x0054 0x05 */ MSCRIPT_OFFER_ITEM(GI_CHATEAU, 0x0),
    /* 0x0059 0x03 */ MSCRIPT_COLLECT_SET(0x0091),
    /* 0x005C 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x005D 0x01 */ MSCRIPT_DONE(),

    /* 0x005E 0x01 */ MSCRIPT_PLAY_DECIDE(),
    /* 0x005F 0x01 */ MSCRIPT_CLOSE_TEXT(),
    /* 0x0060 0x03 */ MSCRIPT_CHANGE_RUPEES(0xFFEC),
    /* 0x0063 0x05 */ MSCRIPT_OFFER_ITEM(GI_MILK, 0x0),
    /* 0x0068 0x03 */ MSCRIPT_COLLECT_SET(0x0092),
    /* 0x006B 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x006C 0x01 */ MSCRIPT_DONE(),

    /* 0x006D 0x01 */ MSCRIPT_PLAY_DECIDE(),
    /* 0x006E 0x05 */ MSCRIPT_BRANCH_ON_WEEK_EVENT_REG(0x34, 0x01, 0x0078 - 0x0073),
    /* 0x0073 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B0F),
    /* 0x0076 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x0077 0x01 */ MSCRIPT_DONE(),

    /* 0x0078 0x03 */ MSCRIPT_BEGIN_TEXT(0x2B11),
    /* 0x007B 0x01 */ MSCRIPT_AWAIT_TEXT(),
    /* 0x007C 0x01 */ MSCRIPT_DONE(),
};

static TabDialogueState tabDialogueState = TAB_D_IDLE;

int32_t EnTab_OverrideBottleCheckCallback(Actor* thisx, PlayState* play) {
    // Should always return true - as if the player always has an empty bottle!
    // See func_80BE0D38 in z_en_tab.c
    return true;
}

void EnTab_UpdateDialogueState(u16* textId, bool* loadFromMessageTable) {
    switch (*textId) {
        case 0x2B0A:
            tabDialogueState = TAB_D_INITIAL_DIALOGUE;
            break;
        case 0x2B0B:
            tabDialogueState = TAB_D_SHOP_DIALOGUE;
            break;
        default:
            tabDialogueState = TAB_D_IDLE;
    }
}

void EnTab_OnOpenShopText(u16* textId, bool* loadFromMessageTable) {
    RandoSaveCheck milkPurchaseCheck = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK];
    RandoSaveCheck chateauPurchaseCheck = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU];

    RandoItemId riMilkPurchase = Rando::ConvertItem(milkPurchaseCheck.randoItemId, RC_MILK_BAR_PURCHASE_MILK);
    RandoItemId riChateauPurchase = Rando::ConvertItem(chateauPurchaseCheck.randoItemId, RC_MILK_BAR_PURCHASE_CHATEAU);

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 {price1} Rupees\x11"
                "\x02{item2}\x01 {price2} Rupees\x11"
                "\x02Nothing";

    std::string itemName1 = "Milk";
    std::string itemPrice1 = "20";
    if (!milkPurchaseCheck.cycleObtained) {
        itemName1 = Rando::StaticData::Items[riMilkPurchase].name;
        itemPrice1 = std::to_string(milkPurchaseCheck.price);
    }

    std::string itemName2 = "Chateau Romani";
    std::string itemPrice2 = "200";
    if (!chateauPurchaseCheck.cycleObtained) {
        itemName2 = Rando::StaticData::Items[riChateauPurchase].name;
        itemPrice2 = std::to_string(chateauPurchaseCheck.price);
    }

    CustomMessage::Replace(&entry.msg, "{item1}", itemName1);
    CustomMessage::Replace(&entry.msg, "{item2}", itemName2);
    CustomMessage::Replace(&entry.msg, "{price1}", itemPrice1);
    CustomMessage::Replace(&entry.msg, "{price2}", itemPrice2);
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void Rando::ActorBehavior::InitEnTabBehavior() {
    // Give the randomized items instead if they haven't already been purchased
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_TAB) {
            if (gPlayState->msgCtx.choiceIndex == 0) {
                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
                    *should = false;
                    RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].eligible = true;
                    EnTab* enTab = (EnTab*)actor;
                    Player* player = GET_PLAYER(gPlayState);
                    enTab->actor.parent = &player->actor;
                }
            } else {
                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
                    *should = false;
                    RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].eligible = true;
                    EnTab* enTab = (EnTab*)actor;
                    Player* player = GET_PLAYER(gPlayState);
                    enTab->actor.parent = &player->actor;
                }
            }
        }
    });

    // Use the randomized prices for message script branching/game state updates
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        MsgScript* script = va_arg(args, MsgScript*);
        MsgEventCallback* callback = va_arg(args, MsgEventCallback*);
        MsgScript** scriptPtr = va_arg(args, MsgScript**);

        Player* player = GET_PLAYER(gPlayState);
        EnTab* tabActor = (EnTab*)actor;

        // Override script during initial MSCRIPT_BEGIN_TEXT
        if (cmdId == MSCRIPT_CMD_14) {
            u16 textId = MSCRIPT_GET_16(script, 1);
            if (textId == 0x2B0A) {
                *scriptPtr = standardDialogueOverrideScript;
            }
        }

        // Override callback function depending on actor state for MSCRIPT_BRANCH_ON_CALLBACK_2
        if (cmdId == MSCRIPT_CMD_40) {
            if (tabDialogueState != TAB_D_SHOP_DIALOGUE) {
                *callback = EnTab_OverrideBottleCheckCallback;
            } else {
                if (gPlayState->msgCtx.choiceIndex == 0) {
                    if (RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
                        *callback = func_80BE0D38;
                    } else {
                        *callback = EnTab_OverrideBottleCheckCallback;
                    }
                } else if (gPlayState->msgCtx.choiceIndex == 1) {
                    if (RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
                        *callback = func_80BE0D38;
                    } else {
                        *callback = EnTab_OverrideBottleCheckCallback;
                    }
                }
            }
        }

        // Use check prices instead of vanilla for MSCRIPT_BRANCH_ON_RUPEES
        if (cmdId == MSCRIPT_CMD_08) {
            s16 checkPrice;
            if (gPlayState->msgCtx.choiceIndex == 0) {
                checkPrice = 20;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
                    checkPrice = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].price;
                }
            } else {
                checkPrice = 200;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
                    checkPrice = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].price;
                }
            }

            script[1] = checkPrice >> 8;   // upper byte of price
            script[2] = checkPrice & 0xFF; // lower byte of price
        }

        // Charge Link the randomized price when calling MSCRIPT_CHANGE_RUPEES
        if (cmdId == MSCRIPT_CMD_20) {
            s16 rupeeChangeAmt;
            if (gPlayState->msgCtx.choiceIndex == 0) {
                rupeeChangeAmt = -20;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
                    rupeeChangeAmt = -RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].price;
                }
            } else {
                rupeeChangeAmt = -200;

                if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
                    rupeeChangeAmt = -RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].price;
                }
            }

            script[1] = rupeeChangeAmt >> 8;   // upper byte of price
            script[2] = rupeeChangeAmt & 0xFF; // lower byte of price
        }
    });

    COND_ID_HOOK(OnOpenText, 0x2B0B, IS_RANDO, EnTab_OnOpenShopText);

    COND_ID_HOOK(OnOpenText, 0x2B0A, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B0B, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B0C, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B0D, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B0E, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B0F, IS_RANDO, EnTab_UpdateDialogueState);
    COND_ID_HOOK(OnOpenText, 0x2B11, IS_RANDO, EnTab_UpdateDialogueState);
}