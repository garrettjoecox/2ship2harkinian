#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "overlays/actors/ovl_Dm_Char08/z_dm_char08.h"
}

// This handles opening great bay temple without needing the New Wave Bossa Nova
void Rando::ActorBehavior::InitDmChar08Behavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] == RO_ACCESS_DUNGEONS_FORM_ONLY;

    COND_VB_SHOULD(VB_OPEN_GREAT_BAY_FROM_SONG, shouldRegister, {
        DmChar08* dmChar08 = va_arg(args, DmChar08*);
        Player* player = GET_PLAYER(gPlayState);

        *should =
            (player->transformation == PLAYER_FORM_ZORA) && (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE);
        if (*should) {
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
        }
    });
}
