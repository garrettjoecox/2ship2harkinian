#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// This is kind of a catch-all for things that are simple enough to not need their own file.
void MiscVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI:
            *should = false;
            break;
        case VB_MADAME_AROMA_ASK_FOR_HELP:
            *should = !CHECK_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK);
            break;
    }
}

// Entry point for the module, run once on game boot
void Rando::ActorBehavior::Init() {
}

void Rando::ActorBehavior::OnFileLoad() {
    Rando::ActorBehavior::InitDoorWarp1VBehavior();
    Rando::ActorBehavior::InitDmChar05Behavior();
    Rando::ActorBehavior::InitDmHinaBehavior();
    Rando::ActorBehavior::InitDmStkBehavior();
    Rando::ActorBehavior::InitEnAkindonutsBehavior();
    Rando::ActorBehavior::InitEnBoxBehavior();
    Rando::ActorBehavior::InitEnCowBehavior();
    Rando::ActorBehavior::InitEnElfgrpBehavior();
    Rando::ActorBehavior::InitEnElforgBehavior();
    Rando::ActorBehavior::InitEnGgBehavior();
    Rando::ActorBehavior::InitEnGirlABehavior();
    Rando::ActorBehavior::InitEnGoBehavior();
    Rando::ActorBehavior::InitEnItem00Behavior();
    Rando::ActorBehavior::InitEnKgyBehavior();
    Rando::ActorBehavior::InitEnKnightBehavior();
    Rando::ActorBehavior::InitEnMaYtoBehavior();
    Rando::ActorBehavior::InitEnOsnBehavior();
    Rando::ActorBehavior::InitEnSellnutsBehavior();
    Rando::ActorBehavior::InitEnSiBehavior();
    Rando::ActorBehavior::InitEnStoneheishiBehavior();
    Rando::ActorBehavior::InitEnBabaBehavior();
    Rando::ActorBehavior::InitEnYbBehavior();
    Rando::ActorBehavior::InitEnRzBehavior();
    Rando::ActorBehavior::InitEnMnkBehavior();
    Rando::ActorBehavior::InitEnZogBehavior();
    Rando::ActorBehavior::InitItemBHeartBehavior();
    Rando::ActorBehavior::InitObjMoonStoneBehavior();
    Rando::ActorBehavior::InitObjTsuboBehavior();

    COND_HOOK(ShouldVanillaBehavior, IS_RANDO, MiscVanillaBehaviorHandler);
}
