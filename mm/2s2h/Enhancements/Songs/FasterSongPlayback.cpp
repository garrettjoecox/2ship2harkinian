#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"

extern u8 sPlaybackState;
}

#define CVAR_NAME "gEnhancements.Songs.FasterSongPlayback"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFasterSongPlayback() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        if (gPlayState->msgCtx.msgMode >= MSGMODE_SONG_PLAYED && gPlayState->msgCtx.msgMode <= MSGMODE_17) {
            if (gPlayState->msgCtx.stateTimer > 1) {
                gPlayState->msgCtx.stateTimer = 1;
            }
            gPlayState->msgCtx.ocarinaSongEffectActive = 0;
            sPlaybackState = 0;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterSongPlayback, { CVAR_NAME });
