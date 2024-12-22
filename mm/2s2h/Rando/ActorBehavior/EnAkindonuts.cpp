#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Akindonuts/z_en_akindonuts.h"
void Flags_SetRandoInf(s32 flag);
void func_80BEF360(EnAkindonuts* enAkindonuts, PlayState* play);
void func_80BEEFA8(EnAkindonuts* enAkindonuts, PlayState* play);
}

// This handles the checks for the business scrubs in the Southern Swamp, Goron Village, Zora Hall, and Ikana Canyon.
// Also sets flags for inf bean access and inf blue potion access.
void Rando::ActorBehavior::InitEnAkindonutsBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_AKINDONUTS, IS_RANDO, [](Actor* actor) {
        EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;
        switch (ENAKINDONUTS_GET_3(&enAkindonuts->actor)) {
            case 0:
                if (!RANDO_ACCESS[RANDO_ACCESS_BEANS_REFILL]) {
                    RANDO_ACCESS[RANDO_ACCESS_BEANS_REFILL]++;
                }
                break;
            case 3:
                if (!RANDO_ACCESS[RANDO_ACCESS_BLUE_POTION_REFILL]) {
                    RANDO_ACCESS[RANDO_ACCESS_BLUE_POTION_REFILL]++;
                }
                break;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_AKINDONUTS, IS_RANDO, [](Actor* actor) {
        EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;

        if (enAkindonuts->actionFunc == func_80BEF360) {
            if (enAkindonuts->unk_32C & 0x40) {
                bool triggered_check = false;
                switch (ENAKINDONUTS_GET_3(&enAkindonuts->actor)) {
                    case 0:
                        if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB)) {
                            Flags_SetRandoInf(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB);
                            Rupees_ChangeBy(enAkindonuts->unk_364);
                            triggered_check = true;
                        }
                        break;
                    case 1:
                        if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB)) {
                            Flags_SetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB);
                            Rupees_ChangeBy(enAkindonuts->unk_364);
                            triggered_check = true;
                        }
                        break;
                    case 2:
                        if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB)) {
                            Flags_SetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB);
                            Rupees_ChangeBy(enAkindonuts->unk_364);
                            triggered_check = true;
                        }
                        break;
                    case 3:
                        if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB)) {
                            Flags_SetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB);
                            Rupees_ChangeBy(enAkindonuts->unk_364);
                            triggered_check = true;
                        }
                        break;
                }
                if (triggered_check) {
                    // Had to seperate this to only treat it as a randomized check when a flag is set above.
                    enAkindonuts->unk_32C &= ~0x40;
                    enAkindonuts->unk_2DC(enAkindonuts, gPlayState);
                    enAkindonuts->actionFunc = func_80BEEFA8;
                }
            } else {
                // Had to add this to allow it to work as before with Trade items.
                enAkindonuts->unk_32C &= ~0x40;
                enAkindonuts->unk_2DC(enAkindonuts, gPlayState);
                enAkindonuts->actionFunc = func_80BEEFA8;
            }
        }
    });

    // TODO: Should there be a bomb bag requirement here still?
    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG, IS_RANDO, { *should = true; });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED, IS_RANDO,
                   { *should = Flags_GetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB); });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL, IS_RANDO, {
        EnAkindonuts* got_actor = va_arg(args, EnAkindonuts*);
        switch (ENAKINDONUTS_GET_3(&got_actor->actor)) {
            case 2:
                if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB)) {
                    *should = true;
                } else {
                    *should = Inventory_HasEmptyBottle();
                }
                break;
            case 3:
                if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB)) {
                    *should = true;
                } else {
                    *should = Inventory_HasEmptyBottle();
                }
                break;
        }
    });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BEAN_REFILL, IS_RANDO, {
        EnAkindonuts* got_actor = va_arg(args, EnAkindonuts*);
        if (!Flags_GetRandoInf(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB)) {
            *should = false;
        } else {
            *should = ((u32)INV_CONTENT(ITEM_MAGIC_BEANS) != ITEM_MAGIC_BEANS);
        }
    });
}
