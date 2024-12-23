#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ruppecrow/z_en_ruppecrow.h"
}

void Rando::ActorBehavior::InitEnRuppecrowBehavior() {
    COND_VB_SHOULD(VB_GUAY_DROP_RUPEE, IS_RANDO, {
        EnRuppecrow* refActor = va_arg(args, EnRuppecrow*);

        uint32_t rupeeIndex = refActor->rupeeIndex;

        EnItem00* rupee = CustomItem::Spawn(
            refActor->actor.world.pos.x, refActor->actor.world.pos.y, refActor->actor.world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, (RC_TERMINA_FIELD_GUAY_RUPEE_DROP_1 + rupeeIndex),
            [](Actor* actor, PlayState* play) { RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true; },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM));
            });

        Actor_PlaySfx(&refActor->actor, NA_SE_EV_RUPY_FALL);

        *should = false;
    });
}