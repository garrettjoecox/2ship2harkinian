#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Minigames.SkipBalladOfWindfish"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBalladOfWindfish() {

    COND_HOOK(OnFlagSet, CVAR, [](FlagType flagType, u32 flag) {
        if (INV_CONTENT(ITEM_MASK_DEKU) != ITEM_MASK_DEKU || INV_CONTENT(ITEM_MASK_GORON) != ITEM_MASK_GORON ||
            INV_CONTENT(ITEM_MASK_ZORA) != ITEM_MASK_ZORA) {
            return;
        }

        if (flagType == FLAG_WEEK_EVENT_REG) {
            if (flag == WEEKEVENTREG_56_10 || flag == WEEKEVENTREG_56_20 || flag == WEEKEVENTREG_56_40 ||
                flag == WEEKEVENTREG_56_80) {
                SET_WEEKEVENTREG(WEEKEVENTREG_56_10); // Played Ballad as Human
                SET_WEEKEVENTREG(WEEKEVENTREG_56_20); // Played Ballad as Deku
                SET_WEEKEVENTREG(WEEKEVENTREG_56_40); // Played Ballad as Zora
                SET_WEEKEVENTREG(WEEKEVENTREG_56_80); // Played Ballad as Goron
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBalladOfWindfish, { CVAR_NAME });
