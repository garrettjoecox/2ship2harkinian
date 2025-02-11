#include "DrawFuncs.h"
#include <libultraship/libultraship.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "BenPort.h"

extern "C" {
#include <functions.h>
#include "objects/gameplay_keep/gameplay_keep.h"

// clang-format off
// Boss Includes
/* Goht */      #include "objects/object_boss_hakugin/object_boss_hakugin.h"
/* Gyorg */     #include "objects/object_boss03/object_boss03.h"
/* Odolwa */    #include "objects/object_boss01/object_boss01.h"
/* Twinmold */  #include "objects/object_boss02/object_boss02.h"
/* Majora */    #include "objects/object_boss07/object_boss07.h"
// clang-format on

// Soul Effects
#include "src/overlays/actors/ovl_Obj_Moon_Stone/z_obj_moon_stone.h"
#include "assets/objects/object_gi_reserve00/object_gi_reserve00.h"
}

// Boss Souls
extern void DrawGoht() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[33];
    static Vec3s otherTable[33];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gGohtSkel, (AnimationHeader*)&gGohtRunAnim,
                           jointTable, otherTable, 33);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern void DrawGyorg() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[15];
    static Vec3s otherTable[15];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gGyorgSkel,
                           (AnimationHeader*)&gGyorgGentleSwimmingAnim, jointTable, otherTable, 15);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern void DrawOdolwa() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[52];
    static Vec3s otherTable[52];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gOdolwaSkel,
                           (AnimationHeader*)&gOdolwaReadyAnim, jointTable, otherTable, 52);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern void DrawTwinmold() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[13];
    static Vec3s otherTable[13];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gTwinmoldHeadSkel,
                           (AnimationHeader*)&gTwinmoldHeadFlyAnim, jointTable, otherTable, 13);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gTwinmoldBlueSkinTex);
    SkelAnime_DrawOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern void DrawMajora() {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[MAJORAS_MASK_LIMB_MAX];
    static Vec3s morphTable[MAJORAS_MASK_LIMB_MAX];
    static u32 lastUpdate = 0;

    OPEN_DISPS(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Scale(0.04f, 0.04f, 0.04f, MTXMODE_APPLY);
    Matrix_Translate(0, 0, 0, MTXMODE_APPLY);

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(gPlayState, &skelAnime, (SkeletonHeader*)&gMajorasMaskSkel,
                       (AnimationHeader*)&gMajorasMaskFloatingAnim, jointTable, morphTable, MAJORAS_MASK_LIMB_MAX);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    gSPSegment(POLY_OPA_DISP++, 8, (uintptr_t)Lib_SegmentedToVirtual((void*)gMajorasMaskWithNormalEyesTex));
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gMajorasMaskTentacleMaterialDL);
    SkelAnime_DrawOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}