#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Si/z_en_si.h"
}

void EnSi_DrawCustom(Actor* thisx, PlayState* play) {
    EnSi* enSi = (EnSi*)thisx;

    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_CHEST,
                                                                ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void Rando::ActorBehavior::InitEnSiBehavior() {
    static uint32_t onActorInitHookId = 0;
    static uint32_t shouldHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInitHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId);

    onActorInitHookId = 0;
    shouldHookId = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInitHookId =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_SI, [](Actor* actor) {
            EnSi* enSi = (EnSi*)actor;

            auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
                FLAG_CYCL_SCENE_CHEST, ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
            if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                return;
            }

            auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

            if (randoSaveCheck.obtained) {
                Actor_Kill(&enSi->actor);
                return;
            }

            actor->draw = EnSi_DrawCustom;
        });

    shouldHookId = REGISTER_VB_SHOULD(VB_GIVE_ITEM_FROM_SI, {
        EnSi* enSi = va_arg(args, EnSi*);

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_CHEST, ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        *should = false;
    });
}
