#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "overlays/actors/ovl_En_Dt/z_en_dt.h"
void func_80BEA394(EnDt* enDt, PlayState* play);
}

void mayorSceneNoCouplesMask() {
    if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
        EnDt* enDt =
            (EnDt*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_DT, ACTORCAT_NPC, 99999.9f);
        if (enDt != nullptr) {
            enDt->actionFunc = func_80BEA394;
            enDt->unk_26E = 26;
            enDt->unk_270 = 2;
            enDt->unk_256 = 8;
        }
    }
}

// These skips still allow the first textboxes to display, but they do ignore most of the scene
void RegisterSkipMayorsOfficeCutscene() {
    // "Most townsfolk have already taken shelter..." (no mask, first interaction)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x2ABD, [](u16* textId, bool* loadFromMessageTable) { mayorSceneNoCouplesMask(); });

    // "All must take refuge!" (no mask, subsequent interactions)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x2ABB, [](u16* textId, bool* loadFromMessageTable) { mayorSceneNoCouplesMask(); });

    // "Ah!" (wearing Couple's Mask)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x2AC6, [](u16* textId, bool* loadFromMessageTable) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
                EnDt* enDt = (EnDt*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_DT,
                                                     ACTORCAT_NPC, 99999.9f);
                if (enDt != nullptr) {
                    // Set flags to trigger scene transition and reward
                    enDt->actionFunc = func_80BEA394;
                    enDt->unk_244 = 0;
                    enDt->unk_256 = 20;
                    enDt->unk_270 = 0;
                }
            }
        });
}
