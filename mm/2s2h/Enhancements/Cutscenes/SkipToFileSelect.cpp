#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipToFileSelect"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipToFileSelect() {
    COND_HOOK(OnConsoleLogoUpdate, CVAR, []() {
        ConsoleLogoState* consoleLogoState = (ConsoleLogoState*)gGameState;

        // Wait for the console logo to fade out
        if (consoleLogoState->exit) {
            // Normally the PRNG seed is set at least once from the title opening running Play_Init
            // We need to call it manually before file select creates RNG values for new saves
            Rand_Seed(osGetTime());
            // Normally called on console logo screen
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
            gSaveContext.gameMode = GAMEMODE_TITLE_SCREEN;

            STOP_GAMESTATE(gGameState);
            SET_NEXT_GAMESTATE(gGameState, FileSelect_Init, sizeof(FileSelectState));
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipToFileSelect, { CVAR_NAME });
