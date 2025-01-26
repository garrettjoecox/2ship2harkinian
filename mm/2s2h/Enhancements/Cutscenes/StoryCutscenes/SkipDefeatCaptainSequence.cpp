#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Bsb/z_en_bsb.h"
extern void func_80C0D9B4(EnBsb*, PlayState*);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void SkipDefeatCaptainTextbox(EnBsb* captain) {
    gPlayState->nextEntrance = Entrance_CreateFromSpawn(5);
    gSaveContext.nextCutsceneIndex = 0;
    gPlayState->transitionTrigger = 0x14;
    gPlayState->transitionType = 2;
    gSaveContext.nextTransitionType = 3;
    captain->unk_111A = 0;
}

// todo: this triggers the transition but the textbox stays on screen
// void SkipDefeatCaptainTextbox() {
//     auto blarg = 3;
//     gPlayState->msgCtx.msgMode = MSGMODE_NONE;
//     gPlayState->nextEntrance = ENTRANCE(IKANA_GRAVEYARD, 5);
//     gSaveContext.nextCutsceneIndex = 0;
//     gPlayState->transitionTrigger = 0x14;
//     gPlayState->transitionType = 2;
//     gSaveContext.nextTransitionType = 3;
// }

void SkipDefeatCaptainCutscene() {
    Player* player = GET_PLAYER(gPlayState);

    // set link's position/rotation to what it is at the end of the cutscene
    // player->actor.world = { { -100.0f, 474.0f, -2330.0f }, { 0, -16384, 0 } }; 

    // todo: not sure how to make link not walk forward, setting pos/rot to make him not walk off the ledge
    player->actor.world = { { 64.0f, 488.0f, -2332.0f }, { 0, -16384, 0 } }; 
    
    // get rid of the fire from the chest
    Flags_SetSwitch(gPlayState, 0x0B);
}

void RegisterSkipDefeatCaptainSequence() {
    // todo: couldn't figure out how to make the textbox not show up using this
    // COND_ID_HOOK(OnOpenText, 0x1535, CVAR || IS_RANDO, [](u16* textId, bool* loadFromMessageTable){
    //     SkipDefeatCaptainTextbox();
    // });

    COND_VB_SHOULD(VB_PLAY_DEFEAT_CAPTAIN_SEQUENCE, CVAR || IS_RANDO, {
        *should = false;
        EnBsb* captain = (EnBsb*)va_arg(args, EnBsb*);
        SkipDefeatCaptainTextbox(captain);
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_BOTI && *csId == 20) {
            *should = false;
            SkipDefeatCaptainCutscene();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipDefeatCaptainSequence, { CVAR_NAME, "IS_RANDO" });