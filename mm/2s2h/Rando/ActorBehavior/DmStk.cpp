#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Dm_Char02/z_dm_char02.h"

void DmChar02_PlaySfxForCutscenes(DmChar02* dmChar02, PlayState* play);
}

void DmChar02_UpdateCustom(Actor* actor, PlayState* play) {
    DmChar02* dmChar02 = (DmChar02*)actor;

    SkelAnime_Update(&dmChar02->skelAnime);
    dmChar02->unk_2F0 = dmChar02->unk_2F0;
    dmChar02->actionFunc(dmChar02, play);
    if ((actor->xzDistToPlayer <= 30.0f) && (fabsf(actor->playerHeightRel) <= fabsf(80.0f))) {
        Actor_Kill(&dmChar02->actor);

        auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        randoSaveCheck.eligible = true;
        auto& randoSaveCheck2 = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_SONG_OF_TIME];
        randoSaveCheck2.eligible = true;
    }

    DmChar02_PlaySfxForCutscenes(dmChar02, play);
}

// This handles the two checks for the Clock Tower Roof, the Ocarina and Song of Time checks. It also handles
// overriding the drawing of the Ocarina in the hand of the Skull Kid.
void Rando::ActorBehavior::InitDmStkBehavior() {
    static uint32_t onActorInit = 0;
    static uint32_t shouldHookId1 = 0;
    static uint32_t shouldHookId2 = 0;
    static uint32_t shouldHookId3 = 0;
    static uint32_t shouldHookId4 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldActorInit>(onActorInit);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId2);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId3);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId4);

    onActorInit = 0;
    shouldHookId1 = 0;
    shouldHookId2 = 0;
    shouldHookId3 = 0;
    shouldHookId4 = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInit = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_DM_CHAR02, [](Actor* actor, bool* should) { actor->update = DmChar02_UpdateCustom; });

    shouldHookId1 = REGISTER_VB_SHOULD(GI_VB_DRAW_OCARINA_IN_STK_HAND, {
        if (*should) {
            *should = false;

            Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
            Vec3s rot;
            rot.x = -19737;
            rot.y = 31100;
            rot.z = 8674;
            Vec3f pos;
            pos.x = -26.5f;
            pos.y = -50.8f;
            pos.z = 10.9f;
            Matrix_TranslateRotateZYX(&pos, &rot);

            auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
            Rando::DrawItem(randoSaveCheck.randoItemId);
        }
    });

    shouldHookId2 = REGISTER_VB_SHOULD(GI_VB_OVERRIDE_CHAR02_LIMB, {
        Gfx** dList = va_arg(args, Gfx**);

        *dList = NULL;
    });

    shouldHookId3 = REGISTER_VB_SHOULD(GI_VB_POST_CHAR02_LIMB, {
        Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
        Vec3s rot;
        rot.x = -11554;
        Vec3f pos;
        pos.x = 10.4f;
        pos.y = -26.5f;
        pos.z = 13.2f;
        Matrix_TranslateRotateZYX(&pos, &rot);

        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        Rando::DrawItem(randoSaveCheck.randoItemId);
    });

    shouldHookId4 = REGISTER_VB_SHOULD(GI_VB_STK_HAVE_OCARINA, {
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        *should = !randoSaveCheck.eligible;
    });
}
