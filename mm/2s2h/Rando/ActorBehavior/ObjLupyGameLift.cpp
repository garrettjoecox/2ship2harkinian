#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Lift_Nuts/z_en_lift_nuts.h"
#include "overlays/actors/ovl_Obj_Lupygamelift/z_obj_lupygamelift.h"
#include "overlays/actors/ovl_En_Gamelupy/z_en_gamelupy.h"
}

int32_t DetermineLupyIndex() {
    int32_t index = 0;

    ActorListEntry item00List = gPlayState->actorCtx.actorLists[ACTORCAT_MISC];
    Actor* currentActor = item00List.first;
    for (int i = 0; i <= item00List.length; i++) {
        if (currentActor == nullptr) {
            continue;
        }
        if (currentActor->id == ACTOR_EN_ITEM00) {
            index++;
        }
        currentActor->next;
    }

    return index;
}

RandoCheckId IdentifyGameLupy() {
    RandoCheckId randoCheckId = RC_UNKNOWN;

    switch (DetermineLupyIndex()) {
        case 0:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_1;
            break;
        case 2:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_2;
            break;
        case 3:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_3;
            break;
        case 4:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_4;
            break;
        case 5:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_5;
            break;
        case 6:
            randoCheckId = RC_DEKU_PLAYGROUND_RUPEE_6;
            break;
        default:
            break;
    }

    return randoCheckId;
}

void Rando::ActorBehavior::InitObjLupyGameLiftBehavior() {
    COND_VB_SHOULD(VB_SPAWN_GAMELUPY, IS_RANDO, {
        ObjLupygamelift* refActor = va_arg(args, ObjLupygamelift*);

        RandoCheckId randoCheckId = IdentifyGameLupy();

        EnItem00* gamelupy = CustomItem::Spawn(
            refActor->dyna.actor.world.pos.x, refActor->dyna.actor.world.pos.y, refActor->dyna.actor.world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::APPLY_GRAVITY, randoCheckId,
            [](Actor* actor, PlayState* play) { RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true; },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM));
            });

        *should = false;
    });

    COND_ID_HOOK(OnActorKill, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor) {
        if (gPlayState->sceneId != SCENE_DEKUTES) {
            return;
        }

        ActorListEntry npcList = gPlayState->actorCtx.actorLists[ACTORCAT_NPC];
        Actor* currentActor = npcList.first;
        for (int i = 0; i <= npcList.length; i++) {
            if (currentActor == nullptr) {
                continue;
            }
            if (currentActor->id == ACTOR_EN_LIFT_NUTS && currentActor->params == 1791) {
                EnLiftNuts* refActor = (EnLiftNuts*)currentActor;
                *refActor->minigameScore = (ENGAMELUPY_POINTS * 6);
                break;
            }
            currentActor->next;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor) {
        if (gPlayState->sceneId != SCENE_DEKUTES) {
            return;
        }

        Actor* liftActor = Actor_FindNearby(gPlayState, actor, ACTOR_OBJ_LUPYGAMELIFT, ACTORCAT_BG, 100.0f);

        if (liftActor != nullptr) {
            ObjLupygamelift* refActor = (ObjLupygamelift*)liftActor;
            actor->world.pos.y = refActor->dyna.actor.world.pos.y + 10.0f;
        }
    });
}
