#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
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

void EnGirlA_RandoBought(PlayState* play, EnGirlA* enGirlA) {
    enGirlA->isOutOfStock = true;
    enGirlA->actor.draw = NULL;
}

void EnGirlA_RandoRestock(PlayState* play, EnGirlA* enGirlA) {
    // Maybe in the future we'll restock ammo items / refills? for now nothing
}

s32 EnGirlA_RandoCanBuyFunc(PlayState* play, EnGirlA* enGirlA) {
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];

    // Probably need to have a more robust check here but this effectively checks if the player can receive the item for
    // now
    if (randoSaveCheck.randoItemId != RI_RUPEE_BLUE &&
        Rando::ConvertItem(randoSaveCheck.randoItemId) == RI_RUPEE_BLUE) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }

    return CANBUY_RESULT_SUCCESS_2;
}

void EnGirlA_RandoBuyFunc(PlayState* play, EnGirlA* enGirlA) {
    auto& randoSaveCheck = RANDO_SAVE_CHECKS[enGirlA->actor.world.rot.z];

    randoSaveCheck.eligible = true;
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_RandoBuyFanfareFunc(PlayState* play, EnGirlA* enGirlA) {
    // No-op, if we made it here something went wrong
}

void EnGirlA_RandoDrawFunc(Actor* actor, PlayState* play) {
    EnGirlA* enGirlA = (EnGirlA*)actor;

    auto randoSaveCheck = RANDO_SAVE_CHECKS[actor->world.rot.z];

    Matrix_RotateYS(enGirlA->rotY, MTXMODE_APPLY);

    Rando::DrawItem(randoSaveCheck.randoItemId);
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

    if (randoSaveCheck.eligible) {
        enGirlA->isOutOfStock = true;
        enGirlA->actor.draw = NULL;
    } else {
        enGirlA->isOutOfStock = false;
        enGirlA->actor.draw = EnGirlA_RandoDrawFunc;
    }
}

RandoCheckId IdentifyShopItem(Actor* actor) {
    switch (gPlayState->sceneId) {
        case SCENE_8ITEMSHOP:
            switch (actor->params) {
                case 10:
                    return RC_TRADING_POST_SHOP_ITEM_1;
                case 5:
                    return RC_TRADING_POST_SHOP_ITEM_2;
                case 6:
                    return RC_TRADING_POST_SHOP_ITEM_3;
                case 3:
                    return RC_TRADING_POST_SHOP_ITEM_4;
                case 7:
                    return RC_TRADING_POST_SHOP_ITEM_5;
                case 8:
                    return RC_TRADING_POST_SHOP_ITEM_6;
                case 9:
                    return RC_TRADING_POST_SHOP_ITEM_7;
                case 4:
                    return RC_TRADING_POST_SHOP_ITEM_8;
            }
            break;
        case SCENE_BOMYA:
            switch (actor->params) {
                case 26:
                    return RC_BOMB_SHOP_ITEM_1;
                case 25:
                    return RC_BOMB_SHOP_ITEM_2;
                case 23:
                    return RC_BOMB_SHOP_ITEM_3;
            }
            break;
        case SCENE_WITCH_SHOP:
            switch (actor->params) {
                case 2:
                    return RC_HAGS_POTION_SHOP_ITEM_1;
                case 1:
                    return RC_HAGS_POTION_SHOP_ITEM_2;
                case 0:
                    return RC_HAGS_POTION_SHOP_ITEM_3;
            }
            break;
        case SCENE_GORONSHOP:
            switch (actor->params) {
                case 30:
                    return RC_GORON_SHOP_ITEM_1;
                case 31:
                    return RC_GORON_SHOP_ITEM_2;
                case 32:
                    return RC_GORON_SHOP_ITEM_3;
            }
            break;
        case SCENE_BANDROOM:
            switch (actor->params) {
                case 27:
                    return RC_ZORA_SHOP_ITEM_1;
                case 28:
                    return RC_ZORA_SHOP_ITEM_2;
                case 29:
                    return RC_ZORA_SHOP_ITEM_3;
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
        if (gPlayState->msgCtx.talkActor->params == 0) {
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
    }

    return randoCheckId;
}

void Rando::ActorBehavior::InitEnGirlABehavior() {
    static uint32_t shouldHookId1 = 0;
    static uint32_t shouldHookId2 = 0;
    static uint32_t onActorInit = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId2);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInit);

    shouldHookId1 = 0;
    shouldHookId2 = 0;
    onActorInit = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInit =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_GIRLA, [](Actor* actor) {
            EnGirlA* enGirlA = (EnGirlA*)actor;

            RandoCheckId randoCheckId = IdentifyShopItem(actor);
            if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
                enGirlA->actor.world.rot.z = randoCheckId;
                enGirlA->mainActionFunc = EnGirlA_RandoInit;
            }
        });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        RANDO_DESC_TEXT_ID, [](u16* textId, bool* loadFromMessageTable) {
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

            if (randoSaveCheck.eligible) {
                CustomMessage::Replace(&entry.msg, "Recover your energy in one gulp!", "Out of Stock");
            } else {
                CustomMessage::Replace(&entry.msg, "Recover your energy in one gulp!", "Buy it, you won't regret it!");
            }

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        RANDO_CHOICE_TEXT_ID, [](u16* textId, bool* loadFromMessageTable) {
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
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x880, [](u16* textId, bool* loadFromMessageTable) {
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
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(0x884, [](u16* textId,
                                                                                          bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = RC_HAGS_POTION_SHOP_ITEM_1;
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
}
