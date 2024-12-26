#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// This is kind of a catch-all for things that are simple enough to not need their own file.
void MiscVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case VB_MADAME_AROMA_ASK_FOR_HELP:
            *should = !CHECK_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK);
            break;
        case VB_GIVE_ITEM_FROM_MALON:
            *should = CHECK_QUEST_ITEM(QUEST_SONG_EPONA);
            break;
        // TODO: This should be configurable
        case VB_DOOR_HEALTH_CHECK_FAIL:
            *should = false;
            break;
        case VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI:
        case VB_MSG_SCRIPT_DEL_ITEM:
        case VB_GIVE_NEW_WAVE_BOSSA_NOVA:
            *should = false;
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
    Rando::ActorBehavior::InitEnFuBehavior();
    Rando::ActorBehavior::InitEnGgBehavior();
    Rando::ActorBehavior::InitEnGinkoBehavior();
    Rando::ActorBehavior::InitEnGirlABehavior();
    Rando::ActorBehavior::InitEnGoBehavior();
    Rando::ActorBehavior::InitEnGsBehavior();
    Rando::ActorBehavior::InitEnHgBehavior();
    Rando::ActorBehavior::InitEnItem00Behavior();
    Rando::ActorBehavior::InitEnKgyBehavior();
    Rando::ActorBehavior::InitEnKnightBehavior();
    Rando::ActorBehavior::InitEnKujiyaBehavior();
    Rando::ActorBehavior::InitEnMa4Behavior();
    Rando::ActorBehavior::InitEnMaYtoBehavior();
    Rando::ActorBehavior::InitEnOsnBehavior();
    Rando::ActorBehavior::InitEnRuppecrowBehavior();
    Rando::ActorBehavior::InitEnSellnutsBehavior();
    Rando::ActorBehavior::InitEnSiBehavior();
    Rando::ActorBehavior::InitEnStoneheishiBehavior();
    Rando::ActorBehavior::InitEnSyatekiManBehavior();
    Rando::ActorBehavior::InitEnBabaBehavior();
    Rando::ActorBehavior::InitEnBalBehavior();
    Rando::ActorBehavior::InitEnYbBehavior();
    Rando::ActorBehavior::InitEnRzBehavior();
    Rando::ActorBehavior::InitEnMnkBehavior();
    Rando::ActorBehavior::InitEnZogBehavior();
    Rando::ActorBehavior::InitItemBHeartBehavior();
    Rando::ActorBehavior::InitObjMoonStoneBehavior();
    Rando::ActorBehavior::InitObjTsuboBehavior();
    Rando::ActorBehavior::InitObjWarpstoneBehavior();

    COND_HOOK(ShouldVanillaBehavior, IS_RANDO, MiscVanillaBehaviorHandler);
}
