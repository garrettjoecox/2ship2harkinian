#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Ma_Yto/z_en_ma_yto.h"

void EnMaYto_AfterMilkRunInit(EnMaYto* enMaYto, PlayState* play);
void EnMaYto_PostMilkRunEnd(EnMaYto* enMaYto, PlayState* play);
}

void Rando::ActorBehavior::InitEnMaYtoBehavior() {
    static uint32_t onActorUpdateHookId = 0;
    static uint32_t shouldHookId1 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onActorUpdateHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);

    onActorUpdateHookId = 0;
    shouldHookId1 = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorUpdateHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_EN_MA_YTO, [](Actor* actor) {
            EnMaYto* enMaYto = (EnMaYto*)actor;

            if (enMaYto->actionFunc == EnMaYto_AfterMilkRunInit) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
                    if (!RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible) {
                        RANDO_SAVE_CHECKS[RC_MILK_RUN_REWARD].eligible = true;
                    }
                    enMaYto->unk310 = 3;
                    enMaYto->actionFunc = EnMaYto_PostMilkRunEnd;
                }
            }
        });

    shouldHookId1 = REGISTER_VB_SHOULD(VB_CREMIA_CUTSCENE_CHANCE, { *should = false; });
}
