#include "ActorBehavior.h"

extern "C" {
#include "variables.h"
}

// This is kind of a catch-all for things that are simple enough to not need their own file.
void MiscVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case VB_GIVE_ITEM_FROM_GURUGURU:
        case VB_GIVE_ITEM_FROM_SCRIPT:
        case VB_GIVE_ITEM_FROM_SWORDSMAN:
            *should = false;
            break;
        case VB_HAVE_ROMANI_MASK:
            *should = RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].obtained;
            break;
    }
}

// Entry point for the module, run once on game boot
void Rando::ActorBehavior::Init() {
}

void Rando::ActorBehavior::OnFileLoad() {
    Rando::ActorBehavior::InitDmStkBehavior();
    Rando::ActorBehavior::InitEnAkindonutsBehavior();
    Rando::ActorBehavior::InitEnBoxBehavior();
    Rando::ActorBehavior::InitEnCowBehavior();
    Rando::ActorBehavior::InitEnElfgrpBehavior();
    Rando::ActorBehavior::InitEnElforgBehavior();
    Rando::ActorBehavior::InitEnItem00Behavior();
    Rando::ActorBehavior::InitEnOsnBehavior();
    Rando::ActorBehavior::InitEnSellnutsBehavior();
    Rando::ActorBehavior::InitEnSiBehavior();
    Rando::ActorBehavior::InitItemBHeartBehavior();
    Rando::ActorBehavior::InitObjMoonStoneBehavior();
    Rando::ActorBehavior::InitObjTsuboBehavior();

    static uint32_t onVanillaBehaviorHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::ShouldVanillaBehavior>(onVanillaBehaviorHook);

    onVanillaBehaviorHook = 0;

    if (!IS_RANDO) {
        return;
    }

    onVanillaBehaviorHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldVanillaBehavior>(MiscVanillaBehaviorHandler);
}
