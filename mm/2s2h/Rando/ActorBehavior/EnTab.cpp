#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Tab/z_en_tab.h"
}

void EnTab_OnOpenText(u16* textId, bool* loadFromMessageTable) {
    RandoItemId randoItemId1 = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].randoItemId;
    RandoItemId randoItemId2 = RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].randoItemId;

    randoItemId1 = Rando::ConvertItem(randoItemId1, RC_MILK_BAR_PURCHASE_MILK);
    randoItemId2 = Rando::ConvertItem(randoItemId2, RC_MILK_BAR_PURCHASE_CHATEAU);

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 {price1} Rupees\x11"
                "\x02{item2}\x01 {price2} Rupees\x11"
                "\x02Nothing";

    std::string itemName1 = "Milk";
    std::string itemPrice1 = "20";
    if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].cycleObtained) {
        itemName1 = Rando::StaticData::Items[randoItemId1].name;
        itemPrice1 = std::to_string(RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_MILK].price);
    }

    std::string itemName2 = "Chateau Romani";
    std::string itemPrice2 = "200";
    if (!RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].cycleObtained) {
        itemName2 = Rando::StaticData::Items[randoItemId2].name;
        itemPrice2 = std::to_string(RANDO_SAVE_CHECKS[RC_MILK_BAR_PURCHASE_CHATEAU].price);
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
        Player* player = GET_PLAYER(gPlayState);

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

    COND_ID_HOOK(OnOpenText, 0x2B0B, IS_RANDO, EnTab_OnOpenText);
}