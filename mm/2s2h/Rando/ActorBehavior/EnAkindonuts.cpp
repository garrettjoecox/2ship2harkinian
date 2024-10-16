#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Akindonuts/z_en_akindonuts.h"

void func_80BEF360(EnAkindonuts* enAkindonuts, PlayState* play);
void func_80BEEFA8(EnAkindonuts* enAkindonuts, PlayState* play);
}

// This handles the checks for the business scrubs in the Southern Swamp, Goron Village, Zora Hall, and Ikana Canyon.
// TODO: Should the potion & bean refills even be checks? If we want to keep them as checks we need a message indicating
// you have already purchased them, currently attempting to purchase again will not do anything.
void Rando::ActorBehavior::InitEnAkindonutsBehavior() {
    static uint32_t onActorUpdateHookId = 0;
    static uint32_t shouldHookId1 = 0;
    static uint32_t shouldHookId2 = 0;
    static uint32_t shouldHookId3 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onActorUpdateHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId2);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId3);

    onActorUpdateHookId = 0;
    shouldHookId1 = 0;
    shouldHookId2 = 0;
    shouldHookId3 = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorUpdateHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_EN_AKINDONUTS, [](Actor* actor) {
            EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;

            if (enAkindonuts->actionFunc == func_80BEF360) {
                if (enAkindonuts->unk_32C & 0x40) {
                    switch (ENAKINDONUTS_GET_3(&enAkindonuts->actor)) {
                        case 0:
                            if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB)) {
                                Flags_SetRandoInf(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB);
                                Rupees_ChangeBy(enAkindonuts->unk_364);
                            }
                            break;
                        case 1:
                            if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB)) {
                                Flags_SetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB);
                                Rupees_ChangeBy(enAkindonuts->unk_364);
                            }
                            break;
                        case 2:
                            if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB)) {
                                Flags_SetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB);
                                Rupees_ChangeBy(enAkindonuts->unk_364);
                            }
                            break;
                        case 3:
                            if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB)) {
                                Flags_SetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB);
                                Rupees_ChangeBy(enAkindonuts->unk_364);
                            }
                            break;
                    }

                    enAkindonuts->unk_32C &= ~0x40;
                }
                enAkindonuts->unk_2DC(enAkindonuts, gPlayState);
                enAkindonuts->actionFunc = func_80BEEFA8;
            }
        });

    // TODO: Should there be a bomb bag requirement here still?
    shouldHookId1 = REGISTER_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG, { *should = true; });
    shouldHookId2 = REGISTER_VB_SHOULD(VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED, {
        *should = Flags_GetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB);
    });
    shouldHookId3 = REGISTER_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL, { *should = true; });
}
