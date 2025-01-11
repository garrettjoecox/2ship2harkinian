#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Fsn/z_en_fsn.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include "overlays/actors/ovl_En_Ossan/z_en_ossan.h"
#include "overlays/actors/ovl_En_Sob1/z_en_sob1.h"
#include "overlays/actors/ovl_En_Trt/z_en_trt.h"

void EnGirlA_Update2(EnGirlA* enGirlA, PlayState* play);
void EnGirlA_DoNothing(EnGirlA* enGirlA, PlayState* play);
void EnGirlA_SetupAction(EnGirlA* enGirlA, EnGirlAActionFunc action);
}

#define RANDO_DESC_TEXT_ID 0x083F
#define RANDO_CHOICE_TEXT_ID 0x0840
#define RANDO_BOMB_SHOP_NEW_STOCK_HINT_TEXT_ID 0x648
#define RANDO_BOMB_SHOP_GOT_NEW_STOCK_1_TEXT_ID 0x649
#define RANDO_BOMB_SHOP_GOT_NEW_STOCK_2_TEXT_ID 0x65A
#define RANDO_BOMB_SHOP_GOT_NEW_STOCK_3_TEXT_ID 0x65C
#define RANDO_BOMB_SHOP_GOT_NEW_STOCK_4_TEXT_ID 0x65E
#define RANDO_BOMB_SHOP_MISSED_NEW_STOCK_1_TEXT_ID 0x64A
#define RANDO_BOMB_SHOP_MISSED_NEW_STOCK_2_TEXT_ID 0x660
#define RANDO_BOMB_SHOP_MISSED_NEW_STOCK_3_TEXT_ID 0x661
#define RANDO_BOMB_SHOP_MISSED_NEW_STOCK_4_TEXT_ID 0x65B // Same contents as above, but a different ID
#define RANDO_CURIOSITY_SHOP_BARGAIN_1_TEXT_ID 0x29D4
#define RANDO_CURIOSITY_SHOP_BARGAIN_2_TEXT_ID 0x29D8 // Same contents as above, but a different ID
#define RANDO_CURIOSITY_SHOP_STOLEN_BOMB_SHOP_1_TEXT_ID 0x29D3
#define RANDO_CURIOSITY_SHOP_STOLEN_BOMB_SHOP_2_TEXT_ID 0x29D7 // Same contents as above, but a different ID

static const std::vector<std::string> flavorTexts = {
    "Buy it, you won't regret it!",   "A must-have for any adventurer!", "A great gift for a friend!",
    "One of a kind, don't miss out!", "A great deal for the price!",     "On sale for a limited time!",
    "Get it while it's hot!",         "Don't miss out on this deal!",
};

void EnGirlA_RandoDrawFunc(Actor* actor, PlayState* play) {
    EnGirlA* enGirlA = (EnGirlA*)actor;

    auto randoSaveCheck = RANDO_SAVE_CHECKS[actor->world.rot.z];

    Matrix_RotateYS(enGirlA->rotY, MTXMODE_APPLY);

    Rando::DrawItem(randoSaveCheck.randoItemId, actor);
}

void EnGirlA_RandoBought(PlayState* play, EnGirlA* enGirlA) {
    enGirlA->isOutOfStock = true;
    enGirlA->actor.draw = NULL;
}

void EnGirlA_RandoRestock(PlayState* play, EnGirlA* enGirlA) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];

    if (Rando::IsItemObtainable(randoSaveCheck.randoItemId, (RandoCheckId)enGirlA->actor.world.rot.z)) {
        enGirlA->isOutOfStock = false;
        enGirlA->actor.draw = EnGirlA_RandoDrawFunc;
    }
}

s32 EnGirlA_RandoCanBuyFunc(PlayState* play, EnGirlA* enGirlA) {
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];

    if (!Rando::IsItemObtainable(randoSaveCheck.randoItemId, (RandoCheckId)enGirlA->actor.world.rot.z)) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }

    return CANBUY_RESULT_SUCCESS_2;
}

void EnGirlA_RandoBuyFunc(PlayState* play, EnGirlA* enGirlA) {
    auto& randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];
    RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)enGirlA->actor.world.rot.z);
    randoSaveCheck.obtained = true;
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
    Rando::GiveItem(randoItemId);
}

void EnGirlA_RandoBuyFanfareFunc(PlayState* play, EnGirlA* enGirlA) {
    // No-op, if we made it here something went wrong
}

void EnGirlA_RandoInit(EnGirlA* enGirlA, PlayState* play) {
    enGirlA->actor.flags &= ~ACTOR_FLAG_10;
    enGirlA->actor.textId = RANDO_DESC_TEXT_ID;
    enGirlA->choiceTextId = RANDO_CHOICE_TEXT_ID;

    enGirlA->boughtFunc = EnGirlA_RandoBought;
    enGirlA->restockFunc = EnGirlA_RandoRestock;
    enGirlA->canBuyFunc = EnGirlA_RandoCanBuyFunc;
    enGirlA->buyFunc = EnGirlA_RandoBuyFunc;
    enGirlA->buyFanfareFunc = EnGirlA_RandoBuyFanfareFunc;

    enGirlA->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    Actor_SetScale(&enGirlA->actor, 0.25f);
    enGirlA->actor.shape.yOffset = 24.0f;
    enGirlA->actor.shape.shadowScale = 4.0f;
    enGirlA->actor.floorHeight = enGirlA->actor.home.pos.y;
    enGirlA->actor.gravity = 0.0f;
    EnGirlA_SetupAction(enGirlA, EnGirlA_DoNothing);
    enGirlA->isInitialized = true;
    enGirlA->mainActionFunc = EnGirlA_Update2;
    enGirlA->isSelected = false;
    enGirlA->rotY = 0;
    enGirlA->initialRotY = enGirlA->actor.shape.rot.y;

    auto randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];

    if (!Rando::IsItemObtainable(randoSaveCheck.randoItemId, (RandoCheckId)enGirlA->actor.world.rot.z) &&
        randoSaveCheck.obtained) {
        enGirlA->isOutOfStock = true;
        enGirlA->actor.draw = NULL;
    } else {
        enGirlA->isOutOfStock = false;
        enGirlA->actor.draw = EnGirlA_RandoDrawFunc;
    }
}

void renameBombBags(u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;
    CustomMessage::Replace(&entry.msg, "bomb bags",
                           std::string(randoStaticItem.article) + " " + std::string(randoStaticItem.name));
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void renameBigBombBags(u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;
    CustomMessage::Replace(&entry.msg, "Big Bomb Bags",
                           std::string(randoStaticItem.article) + " " + std::string(randoStaticItem.name));
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void renameStolenBombBag(u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.msg = "Tonight's special, stolen from the Bomb Shop: %r" + std::string(randoStaticItem.name) +
                "%w. Check it out!\x19\xA8";
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void renameSpecialBargain(u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CURIOSITY_SHOP_SPECIAL_ITEM];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.msg = "Tonight's bargain: %r" + std::string(randoStaticItem.name) + "%w. Check it out!\x19\xA8";
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void ReplaceCannotBuyMessage(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "Sorry, you can't buy this right now.\xE0",
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

RandoCheckId IdentifyShopItem(Actor* actor) {
    switch (gPlayState->sceneId) {
        case SCENE_8ITEMSHOP:
            switch (actor->params) {
                case 10:
                case 18:
                    return RC_TRADING_POST_SHOP_ITEM_01;
                case 5:
                case 14:
                    return RC_TRADING_POST_SHOP_ITEM_02;
                case 6:
                case 17:
                    return RC_TRADING_POST_SHOP_ITEM_03;
                case 3:
                case 11:
                    return RC_TRADING_POST_SHOP_ITEM_04;
                case 7:
                case 16:
                    return RC_TRADING_POST_SHOP_ITEM_05;
                case 8:
                case 12:
                    return RC_TRADING_POST_SHOP_ITEM_06;
                case 9:
                case 15:
                    return RC_TRADING_POST_SHOP_ITEM_07;
                case 4:
                case 13:
                    return RC_TRADING_POST_SHOP_ITEM_08;
            }
            break;
        case SCENE_BOMYA:
            switch (actor->params) {
                case 26:
                    return RC_BOMB_SHOP_ITEM_01;
                case 25:
                    return RC_BOMB_SHOP_ITEM_02;
                case 24: // After saving Bomb Shop lady
                    return RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM;
                case 23:
                    return RC_BOMB_SHOP_ITEM_03;
            }
            break;
        case SCENE_WITCH_SHOP:
            switch (actor->params) {
                case 2:
                    return RC_HAGS_POTION_SHOP_ITEM_01;
                case 1:
                    return RC_HAGS_POTION_SHOP_ITEM_02;
                case 0:
                    return RC_HAGS_POTION_SHOP_ITEM_03;
            }
            break;
        case SCENE_GORONSHOP:
            switch (actor->params) {
                case 30:
                    return RC_GORON_SHOP_ITEM_01;
                case 31:
                    return RC_GORON_SHOP_ITEM_02;
                case 32:
                    return RC_GORON_SHOP_ITEM_03;
            }
            break;
        case SCENE_BANDROOM:
            switch (actor->params) {
                case 27:
                    return RC_ZORA_SHOP_ITEM_01;
                case 28:
                    return RC_ZORA_SHOP_ITEM_02;
                case 29:
                    return RC_ZORA_SHOP_ITEM_03;
            }
            break;
        case SCENE_AYASHIISHOP:
            switch (actor->params) {
                case 19: // Saved Bomb Shop lady and recovered big bomb bag, so a new item is in stock
                    return RC_CURIOSITY_SHOP_SPECIAL_ITEM;
                case 21: // Sakon stole the bomb bag, so the bomb shop check is in stock
                    return RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM;
            }
            break;
    }

    return RC_UNKNOWN;
}

RandoCheckId IdentifyActiveShopItem() {
    RandoCheckId randoCheckId = RC_UNKNOWN;

    if (gPlayState->msgCtx.talkActor == nullptr) {
        return RC_UNKNOWN;
    }

    if (gPlayState->msgCtx.talkActor->id == ACTOR_EN_TRT) {
        EnTrt* enTrt = (EnTrt*)gPlayState->msgCtx.talkActor;
        if (enTrt->items[enTrt->cursorIndex] != nullptr) {
            randoCheckId = (RandoCheckId)enTrt->items[enTrt->cursorIndex]->actor.world.rot.z;
        }
    } else if (gPlayState->msgCtx.talkActor->id == ACTOR_EN_OSSAN) {
        if (gPlayState->msgCtx.talkActor->params == 0 || gPlayState->msgCtx.talkActor->params == 1) {
            EnOssan* enOssan = (EnOssan*)gPlayState->msgCtx.talkActor;
            if (enOssan->items[enOssan->cursorIndex] != nullptr) {
                randoCheckId = (RandoCheckId)enOssan->items[enOssan->cursorIndex]->actor.world.rot.z;
            }
        } else {
            EnSob1* enSob1 = (EnSob1*)gPlayState->msgCtx.talkActor;
            if (enSob1->items[enSob1->cursorIndex] != nullptr) {
                randoCheckId = (RandoCheckId)enSob1->items[enSob1->cursorIndex]->actor.world.rot.z;
            }
        }
    } else if (gPlayState->msgCtx.talkActor->id == ACTOR_EN_FSN) {
        EnFsn* enFsn = (EnFsn*)gPlayState->msgCtx.talkActor;
        if (enFsn->items[enFsn->cursorIndex] != nullptr) {
            randoCheckId = (RandoCheckId)enFsn->items[enFsn->cursorIndex]->actor.world.rot.z;
        }
    }

    return randoCheckId;
}

void Rando::ActorBehavior::InitEnGirlABehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GIRLA, IS_RANDO, [](Actor* actor) {
        EnGirlA* enGirlA = (EnGirlA*)actor;

        RandoCheckId randoCheckId = IdentifyShopItem(actor);
        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            enGirlA->actor.world.rot.z = randoCheckId;
            enGirlA->mainActionFunc = EnGirlA_RandoInit;
        }
    });

    COND_ID_HOOK(OnOpenText, RANDO_DESC_TEXT_ID, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = IdentifyActiveShopItem();

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;

        CustomMessage::Replace(&entry.msg, "Red Potion: 20 Rupees",
                               std::string(randoStaticItem.name) + ": " + std::to_string(randoSaveCheck.price) +
                                   " Rupees");

        if (!Rando::IsItemObtainable(randoSaveCheck.randoItemId, randoCheckId) && randoSaveCheck.obtained) {
            CustomMessage::Replace(&entry.msg, "Recover your energy in one gulp!", "Out of Stock");
        } else {
            CustomMessage::Replace(&entry.msg, "Recover your energy in one gulp!",
                                   flavorTexts[rand() % flavorTexts.size()]);
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, RANDO_CHOICE_TEXT_ID, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = IdentifyActiveShopItem();

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        entry.firstItemCost = randoSaveCheck.price;

        CustomMessage::Replace(&entry.msg, "Red Potion: 20 Rupees",
                               std::string(randoStaticItem.name) + ": " + std::to_string(randoSaveCheck.price) +
                                   " Rupees");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Magic Potion Shop Hag "I can't get the ingredients for this"
    COND_ID_HOOK(OnOpenText, 0x880, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = IdentifyActiveShopItem();

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        entry.firstItemCost = randoSaveCheck.price;

        CustomMessage::Replace(&entry.msg, "Blue Potion: 60 Rupees",
                               std::string(randoStaticItem.name) + ": " + std::to_string(randoSaveCheck.price) +
                                   " Rupees");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Magic Potion Shop Hag "Well, I can use this to make something, come back later"
    COND_ID_HOOK(OnOpenText, 0x884, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = RC_HAGS_POTION_SHOP_ITEM_01;
        auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        if (!randoSaveCheck.shuffled || randoSaveCheck.eligible) {
            return;
        }

        auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;

        CustomMessage::Replace(&entry.msg, "something, so come back later.",
                               std::string(randoStaticItem.name) + ", take it!");

        // Mark the item as eligible for purchase
        randoSaveCheck.eligible = true;
        // Set flag that is normally set in the return experience that this skips
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FREE_BLUE_POTION);
        // Mark the item as out of stock
        EnTrt* enTrt =
            (EnTrt*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_TRT, ACTORCAT_NPC, 100.0f);
        if (enTrt != nullptr) {
            EnGirlA* enGirlA = enTrt->items[2];
            enGirlA->isOutOfStock = true;
            enGirlA->actor.draw = NULL;
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Bomb Shop "We're expecting new stock" (hint)
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_NEW_STOCK_HINT_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) {
                     auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
                     auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

                     auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
                     entry.msg = "We're expecting to get our %r" + std::string(randoStaticItem.name) +
                                 "%w back in stock pretty soon...\x19\xA8";

                     CustomMessage::LoadCustomMessageIntoFont(entry);
                     *loadFromMessageTable = false;
                 });

    // Bomb Shop "We should have had..."
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_MISSED_NEW_STOCK_1_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) {
                     auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
                     auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

                     auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
                     entry.msg = "Actually, we should have had " + std::string(randoStaticItem.article) + " %r" +
                                 std::string(randoStaticItem.name) +
                                 "%w in stock, but there was an accident getting it here to the store.\x19\xA8";

                     CustomMessage::LoadCustomMessageIntoFont(entry);
                     *loadFromMessageTable = false;
                 });

    // Bomb Shop "I thought we could finally sell"
    COND_ID_HOOK(
        OnOpenText, RANDO_BOMB_SHOP_MISSED_NEW_STOCK_2_TEXT_ID, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
            auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
            auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.msg = "It's such a shame... I thought we could finally sell " + std::string(randoStaticItem.article) +
                        " %r" + std::string(randoStaticItem.name) + "%w...\x19\xA8";

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });

    // Bomb Shop "Don't go picking up bomb bags"
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_MISSED_NEW_STOCK_3_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameBombBags(textId, loadFromMessageTable); });

    // Ditto, duplicate textbox
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_MISSED_NEW_STOCK_4_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameBombBags(textId, loadFromMessageTable); });

    // Bomb Shop "We just got a larger bomb bag in stock"
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_GOT_NEW_STOCK_1_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) {
                     auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
                     auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

                     auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
                     entry.msg = "We just got " + std::string(randoStaticItem.article) + " %r" +
                                 std::string(randoStaticItem.name) + "%w in stock.\x19\xA8";

                     CustomMessage::LoadCustomMessageIntoFont(entry);
                     *loadFromMessageTable = false;
                 });

    // Bomb Shop "We can finally sell Big Bomb Bags!"
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_GOT_NEW_STOCK_2_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameBigBombBags(textId, loadFromMessageTable); });

    // Bomb Shop "Our lifelong dream to sell Big Bomb Bags"
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_GOT_NEW_STOCK_3_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameBigBombBags(textId, loadFromMessageTable); });

    // Bomb Shop "Please try our Big Bomb Bag"
    COND_ID_HOOK(OnOpenText, RANDO_BOMB_SHOP_GOT_NEW_STOCK_4_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) {
                     auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM];
                     auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];
                     auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
                     entry.autoFormat = false;
                     CustomMessage::Replace(&entry.msg, "Big Bomb Bag", std::string(randoStaticItem.name));
                     CustomMessage::LoadCustomMessageIntoFont(entry);
                     *loadFromMessageTable = false;
                 });

    // Curiosity Shop "Tonight's special was stolen" (check that is shared with Bomb Shop)
    COND_ID_HOOK(OnOpenText, RANDO_CURIOSITY_SHOP_STOLEN_BOMB_SHOP_1_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameStolenBombBag(textId, loadFromMessageTable); });

    COND_ID_HOOK(OnOpenText, RANDO_CURIOSITY_SHOP_STOLEN_BOMB_SHOP_2_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameStolenBombBag(textId, loadFromMessageTable); });

    // Curiosity Shop "Tonight's bargain is" (special item check)
    COND_ID_HOOK(OnOpenText, RANDO_CURIOSITY_SHOP_BARGAIN_1_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameSpecialBargain(textId, loadFromMessageTable); });

    COND_ID_HOOK(OnOpenText, RANDO_CURIOSITY_SHOP_BARGAIN_2_TEXT_ID, IS_RANDO,
                 [](u16* textId, bool* loadFromMessageTable) { renameSpecialBargain(textId, loadFromMessageTable); });

    // Magic Potion Shop Hag CANBUY_RESULT_CANNOT_GET_NOW (this text ID does not exist and just softlocks)
    COND_ID_HOOK(OnOpenText, 0x643, IS_RANDO, ReplaceCannotBuyMessage);
    // Goron Shop CANBUY_RESULT_CANNOT_GET_NOW
    COND_ID_HOOK(OnOpenText, 0xBD2, IS_RANDO, ReplaceCannotBuyMessage);
    // Bomb Shop CANBUY_RESULT_CANNOT_GET_NOW
    COND_ID_HOOK(OnOpenText, 0x645, IS_RANDO, ReplaceCannotBuyMessage);
    // Trading Post CANBUY_RESULT_CANNOT_GET_NOW
    COND_ID_HOOK(OnOpenText, 0x6BE, IS_RANDO, ReplaceCannotBuyMessage);
    COND_ID_HOOK(OnOpenText, 0x6DB, IS_RANDO, ReplaceCannotBuyMessage);
    // Zora Shop CANBUY_RESULT_CANNOT_GET_NOW (this text ID does not exist and just softlocks)
    COND_ID_HOOK(OnOpenText, 0x12E1, IS_RANDO, ReplaceCannotBuyMessage);
}
