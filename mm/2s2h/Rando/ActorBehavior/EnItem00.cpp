#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "functions.h"
#include "variables.h"
}

void EnItem00_DrawCustomForFreestanding(Actor* thisx, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);

    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, enItem00->collectibleFlag,
                                                                gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void Rando::ActorBehavior::InitEnItem00Behavior(bool isRando) {
    static uint32_t onActorInitHookId = 0;
    static uint32_t shouldHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInitHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId);

    onActorInitHookId = 0;
    shouldHookId = 0;

    if (!isRando) {
        return;
    }

    onActorInitHookId =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_ITEM00, [](Actor* actor) {
            EnItem00* item00 = (EnItem00*)actor;

            // If it's one of our items ignore it
            if (item00->actor.params == ITEM00_NOTHING || item00->actor.params == (ITEM00_NOTHING | 0x8000)) {
                return;
            }

            auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                                        item00->collectibleFlag, gPlayState->sceneId);
            if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                return;
            }

            auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

            if (randoSaveCheck.obtained) {
                Actor_Kill(&item00->actor);
                return;
            }

            actor->draw = EnItem00_DrawCustomForFreestanding;
        });

    shouldHookId = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_ITEM00, {
        EnItem00* item00 = static_cast<EnItem00*>(opt);

        // If it's one of our items ignore it
        if (item00->actor.params == ITEM00_NOTHING || item00->actor.params == (ITEM00_NOTHING | 0x8000)) {
            return;
        }

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, item00->collectibleFlag, gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        Flags_SetCollectible(gPlayState, item00->collectibleFlag);
        Actor_Kill(&item00->actor);
        *should = false;
    });
}
