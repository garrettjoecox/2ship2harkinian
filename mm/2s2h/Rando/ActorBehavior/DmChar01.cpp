#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "overlays/actors/ovl_Dm_Char01/z_dm_char01.h"
}

// This handles opening woodfall temple without needing Sonata of Awakening
void Rando::ActorBehavior::InitDmChar01Behavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] == RO_ACCESS_DUNGEONS_FORM_ONLY;

    COND_VB_SHOULD(VB_OPEN_WOODFALL_FROM_SONG, shouldRegister, {
        DmChar01* dmChar01 = va_arg(args, DmChar01*);
        Player* player = GET_PLAYER(gPlayState);

        *should =
            (player->transformation == PLAYER_FORM_DEKU) && (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE);
        if (*should) {
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
        }
    });
}
