#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "overlays/actors/ovl_En_Dai/z_en_dai.h"
}

// This handles opening Snowhead Temple without needing the Goron Lullaby
void Rando::ActorBehavior::InitEnDaiBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_ACCESS_DUNGEONS] == RO_ACCESS_DUNGEONS_FORM_ONLY;

    COND_VB_SHOULD(VB_OPEN_SNOWHEAD_FROM_SONG, shouldRegister, {
        EnDai* enDai = va_arg(args, EnDai*);
        Player* player = GET_PLAYER(gPlayState);

        *should =
            (player->transformation == PLAYER_FORM_GORON) && (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE);
        if (*should) {
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
        }
    });
}
