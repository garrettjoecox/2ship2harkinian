#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ma_Yto/z_en_ma_yto.h"

void EnMaYto_PostMilkRunEnd(EnMaYto* enMaYto, PlayState* play);
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnMaYtoBehavior() {
    static uint32_t shouldHook1Id = 0;
    static uint32_t shouldHook2Id = 0;
    static uint32_t shouldHook3Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook2Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook3Id);

    shouldHook1Id = 0;
    shouldHook2Id = 0;
    shouldHook3Id = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHook1Id = REGISTER_VB_SHOULD(VB_GIVE_ROMANI_MASK, {
        EnMaYto* enMaYto = va_arg(args, EnMaYto*);
        *should = false;
        enMaYto->unk310 = 3;
        enMaYto->actionFunc = EnMaYto_PostMilkRunEnd;
    });

    shouldHook2Id =
        REGISTER_VB_SHOULD(VB_HAVE_ROMANI_MASK, { *should = RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].obtained; });

    shouldHook3Id = REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (gSaveContext.save.cutsceneIndex == 0x0 && gSaveContext.save.entrance == ENTRANCE(TERMINA_FIELD, 13) &&
            !RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].obtained) {
            RANDO_SAVE_CHECKS[RC_CREMIA_ESCORT].eligible = true;
            SET_WEEKEVENTREG(WEEKEVENTREG_14_01);
            Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROMANIS_MASK);
            Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_ESCORTED_CREMIA);
            Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_MET_CREMIA);
        }
    });
}
