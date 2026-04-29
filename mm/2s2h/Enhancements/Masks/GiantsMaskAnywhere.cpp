#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/Camera/CameraUtils.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "z64shrink_window.h"
extern CameraSetting sCameraSettings[];
}

#define CVAR_NAME "gEnhancements.Masks.GiantsMaskAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

typedef enum {
    GMA_FLASH_NONE = 0,
    GMA_FLASH_STARTED,
    GMA_FLASH_UP,
    GMA_FLASH_DOWN,
} GmaFlashState;

typedef enum {
    GMA_CS_WAITING = 0,
    GMA_CS_GROW,
    GMA_CS_GROW_SKIPPED,
    GMA_CS_SHRINK,
    GMA_CS_SHRINK_SKIPPED,
    GMA_CS_DONE,
} GmaCsState;

static bool sIsGiant = false;
static bool sCanSkipGrow = false;
static bool sCanSkipShrink = false;

static GmaCsState sCsState = GMA_CS_WAITING;
static s16 sSubCamId = SUB_CAM_ID_DONE;
static u32 sCsTimer = 0;
static f32 sPlayerScale = 0.01f;
static f32 sSubCamDistZ = 60.0f;
static f32 sSubCamAtOffsetY = 23.0f;
static f32 sSubCamEyeOffsetY = 10.0f;
static f32 sSubCamAtVel = 0.0f;
static f32 sSubCamUpRotZScale = 0.0f;
static Vec3f sSubCamEye;
static Vec3f sSubCamAt;
static Vec3f sSubCamUp;

static GmaFlashState sFlashState = GMA_FLASH_NONE;
static s16 sFlashAlpha = 0;


static void ScaleAgeProperties(Player* player, f32 factor) {
    PlayerAgeProperties* props = player->ageProperties;
    props->ceilingCheckHeight *= factor;
    props->unk_0C *= factor;
    props->unk_10 *= factor;
    props->unk_14 *= factor;
    props->unk_18 *= factor;
    props->unk_1C *= factor;
    props->unk_24 *= factor;
    props->unk_28 *= factor;
    props->unk_2C *= factor;
    props->unk_30 *= factor;
    props->unk_34 *= factor;
    props->wallCheckRadius *= factor;
    props->unk_3C *= factor;
    props->unk_40 *= factor;
}

static void CutsceneDone(Player* player, PlayState* play) {
    sCsState = GMA_CS_WAITING;
    func_80169AFC(play, sSubCamId, 0);
    sSubCamId = SUB_CAM_ID_DONE;
    player->stateFlags1 &= ~PLAYER_STATE1_100;
    sPlayerScale = 0.01f;
    Play_DisableMotionBlur();
}

static void HandleCutscene(Player* player, PlayState* play) {
    bool applyGrow = false;
    bool applyShrink = false;

    sCsTimer++;

    switch (sCsState) {
        case GMA_CS_WAITING:
            if (player->stateFlags1 & PLAYER_STATE1_100) {
                sSubCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
                Play_EnableMotionBlur(150);
                sCsTimer = 0;
                sSubCamAtVel = 0.0f;
                sSubCamUpRotZScale = 0.0f;
                if (!sIsGiant) {
                    sCsState = GMA_CS_GROW;
                    sSubCamEyeOffsetY = 10.0f;
                    sSubCamDistZ = 60.0f;
                    sSubCamAtOffsetY = 23.0f;
                    sPlayerScale = 0.01f;
                } else {
                    sCsState = GMA_CS_SHRINK;
                    sSubCamEyeOffsetY = 10.0f;
                    sSubCamDistZ = 200.0f;
                    sSubCamAtOffsetY = 273.0f;
                    sPlayerScale = 0.1f;
                }
            }
            break;

        case GMA_CS_GROW:
            // Allow skip only after timer > 0 so the very first frame always runs normally.
            if (sCsTimer > 0 && sCsTimer < 80 && sCanSkipGrow &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
                sCsState = GMA_CS_GROW_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer >= 50) {
                if (sCsTimer == 60) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_GIANT);
                }
                Math_ApproachF(&sSubCamDistZ, 200.0f, 0.1f, sSubCamAtVel * 640.0f);
                Math_ApproachF(&sSubCamAtOffsetY, 273.0f, 0.1f, sSubCamAtVel * 150.0f);
                Math_ApproachF(&sPlayerScale, 0.1f, 0.2f, sSubCamAtVel * 0.1f);
                Math_ApproachF(&sSubCamAtVel, 1.0f, 1.0f, 0.001f);
            } else {
                Math_ApproachF(&sSubCamDistZ, 30.0f, 0.1f, 1.0f);
            }

            if (sCsTimer > 50) {
                Math_ApproachZeroF(&sSubCamUpRotZScale, 1.0f, 0.06f);
            } else {
                Math_ApproachF(&sSubCamUpRotZScale, 0.4f, 1.0f, 0.02f);
            }

            if (sCsTimer == 107) {
                sFlashState = GMA_FLASH_STARTED;
            }

            if (sCsTimer > 120) {
                sCanSkipGrow = true;
                applyGrow = true;
                CutsceneDone(player, play);
            }
            break;

        case GMA_CS_GROW_SKIPPED:
            if (sCsTimer >= 8) {
                applyGrow = true;
                CutsceneDone(player, play);
            }
            break;

        case GMA_CS_SHRINK:
            // Allow skip only after timer > 0 so the very first frame always runs normally.
            if (sCsTimer > 0 && sCsTimer < 30 && sCanSkipShrink &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)) {
                sCsState = GMA_CS_SHRINK_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer != 0) {
                if (sCsTimer == 10) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_NORAML);
                }
                Math_ApproachF(&sSubCamDistZ, 60.0f, 0.1f, sSubCamAtVel * 640.0f);
                Math_ApproachF(&sSubCamAtOffsetY, 23.0f, 0.1f, sSubCamAtVel * 150.0f);
                Math_ApproachF(&sPlayerScale, 0.01f, 0.1f, 0.003f);
                Math_ApproachF(&sSubCamAtVel, 2.0f, 1.0f, 0.01f);
            }

            if (sCsTimer == 42) {
                sFlashState = GMA_FLASH_STARTED;
            }

            if (sCsTimer > 50) {
                sCanSkipShrink = true;
                applyShrink = true;
                CutsceneDone(player, play);
            }
            break;

        case GMA_CS_SHRINK_SKIPPED:
            if (sCsTimer >= 8) {
                applyShrink = true;
                CutsceneDone(player, play);
            }
            break;

        case GMA_CS_DONE:
            CutsceneDone(player, play);
            break;
    }

    // Apply physics scaling at the moment the transformation completes.
    if (applyGrow) {
        ScaleAgeProperties(player, 10.0f);
        player->actor.flags |= ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
        sIsGiant = true;
    } else if (applyShrink) {
        ScaleAgeProperties(player, 0.1f);
        Actor_SetScale(&player->actor, 0.01f);
        player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
        sIsGiant = false;
    }

    // Drive player scale: during cutscene via sPlayerScale, or every frame while giant.
    if (sCsState != GMA_CS_WAITING) {
        f32 scale = player->transformation == PLAYER_FORM_FIERCE_DEITY ? sPlayerScale * 1.5f : sPlayerScale;
        Actor_SetScale(&player->actor, scale);
    } else if (sIsGiant) {
        f32 scale = player->transformation == PLAYER_FORM_FIERCE_DEITY ? 0.15f : 0.1f;
        Actor_SetScale(&player->actor, scale);
    }

    // Screen flash effect.
    switch (sFlashState) {
        case GMA_FLASH_NONE:
            break;
        case GMA_FLASH_STARTED:
            sFlashAlpha = 0;
            R_PLAY_FILL_SCREEN_ON = true;
            R_PLAY_FILL_SCREEN_R = 255;
            R_PLAY_FILL_SCREEN_G = 255;
            R_PLAY_FILL_SCREEN_B = 255;
            R_PLAY_FILL_SCREEN_ALPHA = 0;
            sFlashState = GMA_FLASH_UP;
            Audio_PlaySfx(NA_SE_SY_TRANSFORM_MASK_FLASH);
            [[fallthrough]];
        case GMA_FLASH_UP:
            sFlashAlpha += 40;
            if (sFlashAlpha >= 400) {
                sFlashState = GMA_FLASH_DOWN;
            }
            R_PLAY_FILL_SCREEN_ALPHA = (s16)CLAMP_MAX(sFlashAlpha, 255);
            break;
        case GMA_FLASH_DOWN:
            sFlashAlpha -= 40;
            if (sFlashAlpha <= 0) {
                sFlashAlpha = 0;
                sFlashState = GMA_FLASH_NONE;
                R_PLAY_FILL_SCREEN_ON = false;
            } else {
                R_PLAY_FILL_SCREEN_ALPHA = (s16)CLAMP_MAX(sFlashAlpha, 255);
            }
            break;
    }

    // Drive subcamera during cutscene.
    if (sCsState != GMA_CS_WAITING && sSubCamId != SUB_CAM_ID_DONE) {
        Vec3f eyeOffset;
        Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(sSubCamDistZ, &eyeOffset);

        sSubCamEye.x = player->actor.world.pos.x + eyeOffset.x;
        sSubCamEye.y = player->actor.world.pos.y + eyeOffset.y + sSubCamEyeOffsetY;
        sSubCamEye.z = player->actor.world.pos.z + eyeOffset.z;

        sSubCamAt.x = player->actor.world.pos.x;
        sSubCamAt.y = player->actor.world.pos.y + sSubCamAtOffsetY;
        sSubCamAt.z = player->actor.world.pos.z;

        f32 upRotZ = Math_SinS((s16)(sCsTimer * 1512)) * sSubCamUpRotZScale;
        Matrix_RotateZF(upRotZ, MTXMODE_APPLY);
        Matrix_MultVecY(1.0f, &sSubCamUp);
        Play_SetCameraAtEyeUp(play, sSubCamId, &sSubCamAt, &sSubCamEye, &sSubCamUp);
        ShrinkWindow_Letterbox_SetSizeTarget(27);
    }
}

void RegisterGiantsMaskAnywhere() {
    // Allow the Giant's Mask button to be usable outside of Twinmold's Lair.
    COND_VB_SHOULD(VB_DISABLE_GIANT_MASK, CVAR, { *should = false; });

    // Scale the player's effective height so Camera_Normal0 tracks at the right
    // height/distance. Also feeds into the VB_USE_CUSTOM_CAMERA calculation below.
    COND_VB_SHOULD(VB_PLAYER_HEIGHT, CVAR, {
        Player* playerArg = va_arg(args, Player*);
        f32* heightArg = va_arg(args, f32*);
        if (sIsGiant) {
            *heightArg *= playerArg->actor.scale.y / 0.01f;
        }
    });

    // Directly position the main camera while giant and idle so it frames the
    // full giant-Link body without relying on Camera_Normal0's distance heuristics.
    // COND_VB_SHOULD(VB_USE_CUSTOM_CAMERA, CVAR, {
    //     if (sIsGiant && sCsState == GMA_CS_WAITING) {
    //         Camera* camera = va_arg(args, Camera*);
    //         s16 funcId = sCameraSettings[camera->setting].cameraModes[camera->mode].funcId;
    //         if (camera->camId == CAM_ID_MAIN &&
    //             (funcId == CAM_FUNC_NORMAL0 || funcId == CAM_FUNC_NORMAL1 ||
    //              funcId == CAM_FUNC_NORMAL3 || funcId == CAM_FUNC_NORMAL4 ||
    //              funcId == CAM_FUNC_JUMP2   || funcId == CAM_FUNC_JUMP3)) {
    //             Player* player = GET_PLAYER(gPlayState);
    //             f32 giantHeight = Player_GetHeight(player);
    //             Vec3f atTarget;
    //             atTarget.x = player->actor.world.pos.x;
    //             atTarget.y = player->actor.world.pos.y + giantHeight * 0.6f;
    //             atTarget.z = player->actor.world.pos.z;
    //             VecGeo geo = OLib_Vec3fDiffToVecGeo(&camera->eye, &camera->at);
    //             geo.r = giantHeight * 2.5f;
    //             Vec3f eyeTarget = OLib_AddVecGeoToVec3f(&atTarget, &geo);
    //             Math_ApproachF(&camera->at.x, atTarget.x, 0.3f, 50.0f);
    //             Math_ApproachF(&camera->at.y, atTarget.y, 0.3f, 50.0f);
    //             Math_ApproachF(&camera->at.z, atTarget.z, 0.3f, 50.0f);
    //             Math_ApproachF(&camera->eye.x, eyeTarget.x, 0.3f, 50.0f);
    //             Math_ApproachF(&camera->eye.y, eyeTarget.y, 0.3f, 50.0f);
    //             Math_ApproachF(&camera->eye.z, eyeTarget.z, 0.3f, 50.0f);
    //             camera->dist = giantHeight * 2.5f;
    //             if (camera->fov > 60.0f) {
    //                 camera->fov = 60.0f;
    //             }
    //             camera->roll = 0;
    //             *should = false;
    //         }
    //     }
    // });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR, {
        if (sIsGiant) {
            f32* speedTarget = va_arg(args, f32*);
            *speedTarget *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR, {
        if (sIsGiant) {
            f32* incrStep = va_arg(args, f32*);
            f32* maxSpeed = va_arg(args, f32*);
            va_arg(args, f32*); // skip current speed
            f32* speedTarget = va_arg(args, f32*);
            *incrStep *= 10.0f;
            *maxSpeed *= 10.0f;
            *speedTarget *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_APPLY_AIR_CONTROL, CVAR, {
        if (sIsGiant) {
            f32* speedTarget = va_arg(args, f32*);
            *speedTarget *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_SET_CLIMB_SPEED, CVAR, {
        if (sIsGiant) {
            f32* climbSpeed = va_arg(args, f32*);
            *climbSpeed *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_ZTARGET_SPEED_CHECK, CVAR, {
        if (sIsGiant) {
            f32* speed = va_arg(args, f32*);
            *speed *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_DIVE_DEPTH_CHECK, CVAR, {
        if (sIsGiant) {
            f32* threshold = va_arg(args, f32*);
            *threshold *= 10.0f;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_LEDGE_CLIMB_FACTOR, CVAR, {
        if (sIsGiant) {
            f32* delta = va_arg(args, f32*);
            *delta *= 10.0f;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);

        // The boss room handles Giant's Mask scaling itself.
        if (gPlayState->sceneId == SCENE_INISIE_BS) {
            if (sIsGiant) {
                // Entered boss room while giant - restore physics so the boss can take over cleanly.
                ScaleAgeProperties(player, 0.1f);
                player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
                sIsGiant = false;
            }
            return;
        }

        HandleCutscene(player, gPlayState);
    });
}

static RegisterShipInitFunc initFunc(RegisterGiantsMaskAnywhere, { CVAR_NAME });
