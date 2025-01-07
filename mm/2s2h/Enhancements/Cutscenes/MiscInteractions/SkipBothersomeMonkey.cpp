#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Mnk/z_en_mnk.h"
void EnMnk_Monkey_SetupRunAfterTalk(EnMnk* thisx, PlayState* play);
void EnMnk_Monkey_ApproachPlayer(EnMnk* thisx, PlayState* play);
void EnMnk_Monkey_WaitOutsideWoods(EnMnk* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBothersomeMonkey() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_MNK, CVAR, [](Actor* actor) {
        if (MONKEY_GET_TYPE(actor) == MONKEY_OUTSIDEWOODS) {
            EnMnk* monkey = (EnMnk*)actor;
            PlayState* play = gPlayState;

            if (monkey->actionFunc == EnMnk_Monkey_WaitOutsideWoods) {
                // Trigger monkey approach
                SET_EVENTINF(EVENTINF_26);
                EnMnk_Monkey_ApproachPlayer(monkey, play);
            } else if (monkey->actionFunc == EnMnk_Monkey_ApproachPlayer) {
                // Skip dialogue and make monkey run
                SET_EVENTINF(EVENTINF_25);
                SET_WEEKEVENTREG(WEEKEVENTREG_79_02);
                EnMnk_Monkey_SetupRunAfterTalk(monkey, play);
                Flags_SetSwitch(play, MONKEY_GET_SWITCH_FLAG(&monkey->picto.actor));
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBothersomeMonkey, { CVAR_NAME });
