#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Akindonuts/z_en_akindonuts.h"

void func_80BEF360(EnAkindonuts* enAkindonuts, PlayState* play);
}

void Rando::ActorBehavior::InitEnAkindonutsBehavior() {
    static uint32_t onActorUpdateHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onActorUpdateHookId);

    onActorUpdateHookId = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorUpdateHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_EN_AKINDONUTS, [](Actor* actor) {
            EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;

            if (enAkindonuts->actionFunc == func_80BEF360) {
                // TODO
            }
        });
}
