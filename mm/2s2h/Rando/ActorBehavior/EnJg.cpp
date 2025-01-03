#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "overlays/actors/ovl_En_Jg/z_en_jg.h"

extern "C" {
#include "variables.h"

void EnJg_SetupTalk(EnJg* EnJg, PlayState* play);
}

void Rando::ActorBehavior::InitEnJgBehavior() {
    COND_VB_SHOULD(VB_TEACH_GORON_LULLABY_INTRO, IS_RANDO, {
        EnJg* ObjectActor = va_arg(args, EnJg*);
        PlayState* play = va_arg(args, PlayState*);
        Player* player = GET_PLAYER(gPlayState);
        if (player->transformation == PLAYER_FORM_GORON) {
            if (!RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].obtained) {
                RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].eligible = true;
            }
            ObjectActor->textId = 0xDC6;
        } else {
            ObjectActor->textId = 0xDB5;
        }
        Message_StartTextbox(play, ObjectActor->textId, &ObjectActor->actor);
        ObjectActor->actionFunc = EnJg_SetupTalk;
        *should = false;
    });
}