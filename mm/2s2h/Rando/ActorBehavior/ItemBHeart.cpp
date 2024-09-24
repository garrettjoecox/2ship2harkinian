#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
void ItemBHeart_UpdateModel(ItemBHeart* itemBHeart, PlayState* play);
}

void ItemBHeart_DrawCustom(Actor* thisx, PlayState* play) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, 0x1F, gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void ItemBHeart_UpdateCustom(Actor* thisx, PlayState* play) {
    ItemBHeart* itemBHeart = (ItemBHeart*)thisx;

    ItemBHeart_UpdateModel(itemBHeart, play);

    if (!(itemBHeart->baseScale < BHEART_SCALE_MIN_COLLECTIBLE)) {
        if ((thisx->xzDistToPlayer <= 30.0f) && (fabsf(thisx->playerHeightRel) <= fabsf(80.0f))) {
            Flags_SetCollectible(play, 0x1F);
            Actor_Kill(&itemBHeart->actor);
            return;
        }
    }
}

void Rando::ActorBehavior::InitItemBHeartBehavior(bool isRando) {
    static uint32_t onActorInitHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInitHookId);

    onActorInitHookId = 0;

    if (!isRando) {
        return;
    }

    onActorInitHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(
        ACTOR_ITEM_B_HEART, [](Actor* actor) {
            ItemBHeart* itemBHeart = (ItemBHeart*)actor;

            auto randoStaticCheck =
                Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, 0x1F, gPlayState->sceneId);
            if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                return;
            }

            auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

            if (randoSaveCheck.obtained) {
                Actor_Kill(&itemBHeart->actor);
                return;
            }

            actor->draw = ItemBHeart_DrawCustom;
            actor->update = ItemBHeart_UpdateCustom;
        });
}
