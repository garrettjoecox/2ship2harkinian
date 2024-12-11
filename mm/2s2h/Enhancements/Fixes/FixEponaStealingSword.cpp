#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

void RegisterFixEponaStealingSword() {
    REGISTER_VB_SHOULD(VB_CLEAR_B_BUTTON_FOR_HORSEBACK, {
        if (CVarGetInteger("gFixes.FixEponaStealingSword", 0)) {
            *should = false;
        }
    });
}
