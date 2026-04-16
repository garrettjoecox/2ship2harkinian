#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/Camera/CameraUtils.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "z64shrink_window.h"
extern CameraSetting sCameraSettings[];
extern f32 Camera_ScaledStepToCeilF(f32 target, f32 cur, f32 stepScale, f32 minDiff);
extern s16 Camera_ScaledStepToCeilS(s16 target, s16 cur, f32 stepScale, s16 minDiff);
}

#define CVAR_NAME "gEnhancements.Masks.GiantsMaskAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Cutscene state machine (matches Boss_02's GIANTS_MASK_CS_STATE enum)
typedef enum {
    GMA_CS_IDLE = 0,
    GMA_CS_MASK_ON = 1,
    GMA_CS_MASK_ON_SKIPPED = 2,
    GMA_CS_MASK_OFF = 10,
    GMA_CS_MASK_OFF_SKIPPED = 11,
    GMA_CS_DONE = 20,
} GmaCsState;

// Flash effect state (matches Boss_02's GIANTS_MASK_CS_FLASH_STATE enum)
typedef enum {
    GMA_FLASH_NOT_STARTED = 0,
    GMA_FLASH_STARTED = 1,
    GMA_FLASH_INCREASE_ALPHA = 2,
    GMA_FLASH_DECREASE_ALPHA = 3,
} GmaFlashState;

// State machine variables
static GmaCsState sCsState = GMA_CS_IDLE;
static u32 sCsTimer = 0;
static s16 sSubCamId = SUB_CAM_ID_DONE;
static f32 sPlayerScale = 0.01f;
static f32 sSubCamDistZ = 60.0f;
static f32 sSubCamEyeOffsetY = 10.0f;
static f32 sSubCamAtOffsetY = 23.0f;
static f32 sSubCamAtOffsetTargetY = 273.0f;
static f32 sSubCamUpRotZScale = 0.0f;
static f32 sSubCamAtVel = 0.0f;
static GmaFlashState sFlashState = GMA_FLASH_NOT_STARTED;
static s16 sFlashAlpha = 0;
static bool sIsGiant = false;
static bool sCanSkipOn = false;
static bool sCanSkipOff = false;
static bool sTransformPending = false;
static bool sIsEquipping = false;

// Scale ageProperties float fields by 10x for giant mode (matching MMR's GiantMask_FormProperties_Grow).
// Skips shadowScale (0x04) and unk_08 (animation scale factor).
static void GrowAgeProperties(PlayerAgeProperties* props) {
    props->ceilingCheckHeight *= 10.0f;
    props->unk_0C *= 10.0f;
    props->unk_10 *= 10.0f;
    props->unk_14 *= 10.0f;
    props->unk_18 *= 10.0f;
    props->unk_1C *= 10.0f;
    props->unk_24 *= 10.0f;
    props->unk_28 *= 10.0f;
    props->unk_2C *= 10.0f;
    props->unk_30 *= 10.0f;
    props->unk_34 *= 10.0f;
    props->wallCheckRadius *= 10.0f;
    props->unk_3C *= 10.0f;
    props->unk_40 *= 10.0f;
}

static void ShrinkAgeProperties(PlayerAgeProperties* props) {
    props->ceilingCheckHeight *= 0.1f;
    props->unk_0C *= 0.1f;
    props->unk_10 *= 0.1f;
    props->unk_14 *= 0.1f;
    props->unk_18 *= 0.1f;
    props->unk_1C *= 0.1f;
    props->unk_24 *= 0.1f;
    props->unk_28 *= 0.1f;
    props->unk_2C *= 0.1f;
    props->unk_30 *= 0.1f;
    props->unk_34 *= 0.1f;
    props->wallCheckRadius *= 0.1f;
    props->unk_3C *= 0.1f;
    props->unk_40 *= 0.1f;
}

// Giant scale factor for physics (10x when giant, 1x normal)
static f32 GetGiantScaleModifier() {
    return sIsGiant ? 10.0f : 1.0f;
}

// Restore giant-scaled physics state back to normal
static void CleanupGiantState() {
    if (sIsGiant && gPlayState != nullptr) {
        Player* player = GET_PLAYER(gPlayState);
        if (player != nullptr) {
            if (player->ageProperties->ceilingCheckHeight >= 200.0f) {
                ShrinkAgeProperties(player->ageProperties);
            }
            player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
            Actor_SetScale(&player->actor, 0.01f);
        }
    }
}

static void ResetState() {
    CleanupGiantState();

    sCsState = GMA_CS_IDLE;
    sCsTimer = 0;
    sSubCamId = SUB_CAM_ID_DONE;
    sPlayerScale = 0.01f;
    sSubCamDistZ = 60.0f;
    sSubCamEyeOffsetY = 10.0f;
    sSubCamAtOffsetY = 23.0f;
    sSubCamAtOffsetTargetY = 273.0f;
    sSubCamUpRotZScale = 0.0f;
    sSubCamAtVel = 0.0f;
    sFlashState = GMA_FLASH_NOT_STARTED;
    sFlashAlpha = 0;
    R_PLAY_FILL_SCREEN_ON = false;
    sIsGiant = false;
    sCanSkipOn = false;
    sCanSkipOff = false;
    sTransformPending = false;
    sIsEquipping = false;
}

static void StartCutscene(PlayState* play, bool equipping) {
    Player* player = GET_PLAYER(play);

    sCsState = equipping ? GMA_CS_MASK_ON : GMA_CS_MASK_OFF;
    sCsTimer = 0;
    sSubCamAtVel = 0.0f;
    sSubCamUpRotZScale = 0.0f;
    sTransformPending = false;

    Cutscene_StartManual(play, &play->csCtx);
    sSubCamId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
    Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
    Play_EnableMotionBlur(150);

    // Compute camera offsets proportional to player height (matching MMR)
    f32 playerHeight = Player_GetHeight(player);

    if (equipping) {
        sSubCamEyeOffsetY = 10.0f;
        sSubCamDistZ = 60.0f;
        sSubCamAtOffsetY = playerHeight * 0.53f;
        sSubCamAtOffsetTargetY = playerHeight * 6.2f;
        sPlayerScale = 0.01f;
    } else {
        sSubCamEyeOffsetY = 10.0f;
        sSubCamDistZ = 200.0f;
        sSubCamAtOffsetY = playerHeight * 0.62f;
        sSubCamAtOffsetTargetY = playerHeight * 0.053f;
        sPlayerScale = 0.1f;
    }
}

static void UpdateFlash(PlayState* play) {
    s16 alpha;

    switch (sFlashState) {
        case GMA_FLASH_NOT_STARTED:
            break;

        case GMA_FLASH_STARTED:
            sFlashAlpha = 0;
            R_PLAY_FILL_SCREEN_ON = true;
            R_PLAY_FILL_SCREEN_R = 255;
            R_PLAY_FILL_SCREEN_G = 255;
            R_PLAY_FILL_SCREEN_B = 255;
            R_PLAY_FILL_SCREEN_ALPHA = 0;
            sFlashState = GMA_FLASH_INCREASE_ALPHA;
            Audio_PlaySfx(NA_SE_SY_TRANSFORM_MASK_FLASH);
            // fallthrough
        case GMA_FLASH_INCREASE_ALPHA:
            sFlashAlpha += 40;
            if (sFlashAlpha >= 400) {
                sFlashState = GMA_FLASH_DECREASE_ALPHA;
            }
            alpha = sFlashAlpha;
            alpha = CLAMP_MAX(alpha, 255);
            R_PLAY_FILL_SCREEN_ALPHA = alpha;
            break;

        case GMA_FLASH_DECREASE_ALPHA:
            sFlashAlpha -= 40;
            if (sFlashAlpha <= 0) {
                sFlashAlpha = 0;
                sFlashState = GMA_FLASH_NOT_STARTED;
                R_PLAY_FILL_SCREEN_ON = false;
            } else {
                alpha = sFlashAlpha;
                alpha = CLAMP_MAX(alpha, 255);
                R_PLAY_FILL_SCREEN_ALPHA = alpha;
            }
            break;
    }
}

static void UpdateSubCamera(PlayState* play, Player* player) {
    Vec3f subCamEyeOffset;
    Vec3f subCamEye;
    Vec3f subCamAt;
    Vec3f subCamUp;
    f32 subCamUpRotZ;

    if (sCsState != GMA_CS_IDLE && sSubCamId != SUB_CAM_ID_DONE) {
        Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(sSubCamDistZ, &subCamEyeOffset);

        subCamEye.x = player->actor.world.pos.x + subCamEyeOffset.x;
        subCamEye.y = player->actor.world.pos.y + subCamEyeOffset.y + sSubCamEyeOffsetY;
        subCamEye.z = player->actor.world.pos.z + subCamEyeOffset.z;

        subCamAt.x = player->actor.world.pos.x;
        subCamAt.y = player->actor.world.pos.y + sSubCamAtOffsetY;
        subCamAt.z = player->actor.world.pos.z;

        subCamUpRotZ = Math_SinS(sCsTimer * 1512) * sSubCamUpRotZScale;
        Matrix_RotateZF(subCamUpRotZ, MTXMODE_APPLY);
        Matrix_MultVecY(1.0f, &subCamUp);

        Play_SetCameraAtEyeUp(play, sSubCamId, &subCamAt, &subCamEye, &subCamUp);
        ShrinkWindow_Letterbox_SetSizeTarget(27);
    }
}

static void FinishCutscene(PlayState* play, Player* player, bool wasEquipping) {
    player->stateFlags1 &= ~PLAYER_STATE1_100;

    if (sSubCamId != SUB_CAM_ID_DONE) {
        func_80169AFC(play, sSubCamId, 0);
        sSubCamId = SUB_CAM_ID_DONE;
    }

    Cutscene_StopManual(play, &play->csCtx);
    Play_DisableMotionBlur();

    if (wasEquipping) {
        sPlayerScale = 0.1f;
        sIsGiant = true;
    } else {
        sPlayerScale = 0.01f;
        sIsGiant = false;
    }

    sCsState = GMA_CS_IDLE;
}

static void UpdateCutscene(PlayState* play, Player* player) {
    bool goDone = false;

    sCsTimer++;

    switch (sCsState) {
        case GMA_CS_MASK_ON:
            // Skip check
            if ((sCsTimer < 80) && sCanSkipOn &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT | BTN_DPAD_EQUIP)) {
                sCsState = GMA_CS_MASK_ON_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer >= 50) {
                if (sCsTimer == 60) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_GIANT);
                }

                // Camera pulls back, player grows
                Math_ApproachF(&sSubCamDistZ, 200.0f, 0.1f, sSubCamAtVel * 640.0f);
                Math_ApproachF(&sSubCamAtOffsetY, sSubCamAtOffsetTargetY, 0.1f, sSubCamAtVel * 150.0f);
                Math_ApproachF(&sPlayerScale, 0.1f, 0.2f, sSubCamAtVel * 0.1f);
                Math_ApproachF(&sSubCamAtVel, 1.0f, 1.0f, 0.001f);
            } else {
                // Camera approaches player
                Math_ApproachF(&sSubCamDistZ, 30.0f, 0.1f, 1.0f);
            }

            // Camera roll
            if (sCsTimer > 50) {
                Math_ApproachZeroF(&sSubCamUpRotZScale, 1.0f, 0.06f);
            } else {
                Math_ApproachF(&sSubCamUpRotZScale, 0.4f, 1.0f, 0.02f);
            }

            if (sCsTimer == 107) {
                sFlashState = GMA_FLASH_STARTED;
            }

            if (sCsTimer > 120) {
                sCanSkipOn = true;
                goDone = true;
            }
            break;

        case GMA_CS_MASK_ON_SKIPPED:
            // Snap scale to final value during skip
            Math_ApproachF(&sPlayerScale, 0.1f, 0.5f, 0.05f);
            if (sCsTimer >= 8) {
                goDone = true;
            }
            break;

        case GMA_CS_MASK_OFF:
            // Skip check
            if ((sCsTimer < 30) && sCanSkipOff &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT | BTN_DPAD_EQUIP)) {
                sCsState = GMA_CS_MASK_OFF_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer != 0) {
                if (sCsTimer == 10) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_NORAML);
                }

                // Camera approaches, player shrinks
                Math_ApproachF(&sSubCamDistZ, 60.0f, 0.1f, sSubCamAtVel * 640.0f);
                Math_ApproachF(&sSubCamAtOffsetY, sSubCamAtOffsetTargetY, 0.1f, sSubCamAtVel * 150.0f);
                Math_ApproachF(&sPlayerScale, 0.01f, 0.1f, 0.003f);
                Math_ApproachF(&sSubCamAtVel, 2.0f, 1.0f, 0.01f);
            }

            if (sCsTimer == 42) {
                sFlashState = GMA_FLASH_STARTED;
            }

            if (sCsTimer > 50) {
                sCanSkipOff = true;
                goDone = true;
            }
            break;

        case GMA_CS_MASK_OFF_SKIPPED:
            // Snap scale to final value during skip
            Math_ApproachF(&sPlayerScale, 0.01f, 0.5f, 0.005f);
            if (sCsTimer >= 8) {
                goDone = true;
            }
            break;

        default:
            break;
    }

    if (goDone) {
        bool wasEquipping = (sCsState == GMA_CS_MASK_ON || sCsState == GMA_CS_MASK_ON_SKIPPED);
        FinishCutscene(play, player, wasEquipping);
    }

    // Apply scale every frame during cutscene (with Fierce Deity multiplier)
    f32 scale = sPlayerScale;
    if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
        scale *= 1.5f;
    }
    Actor_SetScale(&player->actor, scale);

    // Update sub-camera
    UpdateSubCamera(play, player);
}

void RegisterGiantsMaskAnywhere() {
    // Cleanup if CVar is being disabled while giant
    if (!CVAR && sIsGiant && gPlayState != nullptr) {
        ResetState();
    }

    // Enable the Giant's Mask button outside Twinmold's Lair
    COND_VB_SHOULD(VB_DISABLE_GIANTS_MASK, CVAR, { *should = false; });

    // Detect when a Giant's Mask transformation starts
    // Does NOT modify *should — the vanilla PLAYER_STATE1_100 flag is still set
    COND_VB_SHOULD(VB_PLAY_GIANTS_MASK_CS, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        sTransformPending = true;
        sIsEquipping = (player->currentMask == PLAYER_MASK_GIANT);
    });

    // Per-frame update — runs cutscene state machine and maintains giant scale/physics
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        Player* player = (Player*)actor;
        PlayState* play = gPlayState;

        // In Twinmold's Lair, Boss_02 handles the transformation cutscene
        if (play->sceneId == SCENE_INISIE_BS) {
            if (sIsGiant || sCsState != GMA_CS_IDLE) {
                ResetState();
            }
            return;
        }

        // Start cutscene when VB hook signals a transformation
        if (sTransformPending && sCsState == GMA_CS_IDLE) {
            StartCutscene(play, sIsEquipping);
        }
        sTransformPending = false;

        // Run cutscene state machine
        if (sCsState != GMA_CS_IDLE) {
            UpdateCutscene(play, player);
        }

        // Run flash effect independently of cutscene state
        if (sFlashState != GMA_FLASH_NOT_STARTED) {
            UpdateFlash(play);
        }

        // Per-frame giant mode maintenance (outside of cutscene)
        if (sIsGiant && sCsState == GMA_CS_IDLE) {
            // Scale ageProperties if not already scaled (threshold guard)
            if (player->ageProperties->ceilingCheckHeight < 200.0f) {
                GrowAgeProperties(player->ageProperties);
                player->actor.flags |= ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
            }

            // Apply giant scale (with Fierce Deity multiplier)
            f32 scale = 0.1f;
            if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                scale *= 1.5f;
            }
            Actor_SetScale(&player->actor, scale);
        } else if (!sIsGiant && sCsState == GMA_CS_IDLE) {
            // Restore ageProperties if still scaled (e.g. after mask removal)
            if (player->ageProperties->ceilingCheckHeight >= 200.0f) {
                ShrinkAgeProperties(player->ageProperties);
            }
            player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
        }

        // Detect mask removal from scene change (game auto-clears Giant's Mask)
        if (sIsGiant && player->currentMask != PLAYER_MASK_GIANT && sCsState == GMA_CS_IDLE) {
            sIsGiant = false;
            if (player->ageProperties->ceilingCheckHeight >= 200.0f) {
                ShrinkAgeProperties(player->ageProperties);
            }
            player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
            Actor_SetScale(&player->actor, 0.01f);
        }
    });

    // Override camera for giant Link (VB_USE_CUSTOM_CAMERA pattern from FreeLook.cpp)
    COND_VB_SHOULD(VB_USE_CUSTOM_CAMERA, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }

        Camera* camera = va_arg(args, Camera*);
        PlayState* play = gPlayState;

        if (play == nullptr || play->sceneId == SCENE_INISIE_BS) {
            return;
        }

        if (camera != Play_GetCamera(play, CAM_ID_MAIN)) {
            return;
        }

        // Only override normal gameplay camera modes (match FreeLook pattern)
        switch (sCameraSettings[camera->setting].cameraModes[camera->mode].funcId) {
            case CAM_FUNC_NORMAL0:
            case CAM_FUNC_NORMAL1:
            case CAM_FUNC_NORMAL3:
            case CAM_FUNC_NORMAL4:
            case CAM_FUNC_JUMP2:
            case CAM_FUNC_JUMP3:
                break;
            default:
                return;
        }

        Player* player = GET_PLAYER(play);
        f32 scaleFactor = sPlayerScale / 0.01f; // 10.0f when giant
        f32 giantHeight = Player_GetHeight(player) * scaleFactor;

        // Compute look-at target: giant Link's upper body
        Vec3f atTarget;
        atTarget.x = player->actor.world.pos.x;
        atTarget.y = player->actor.world.pos.y + giantHeight * 0.6f;
        atTarget.z = player->actor.world.pos.z;

        // Use current camera yaw/pitch to preserve user-controlled rotation
        VecGeo eyeDir = OLib_Vec3fDiffToVecGeo(&camera->at, &camera->eye);

        // Compute eye position at proportional distance behind player
        VecGeo eyeOffset;
        eyeOffset.yaw = eyeDir.yaw;
        eyeOffset.pitch = eyeDir.pitch;
        eyeOffset.r = giantHeight * 2.5f;

        Vec3f eyeTarget = OLib_AddVecGeoToVec3f(&atTarget, &eyeOffset);

        // Smooth approach to targets for non-jarring transitions
        Math_ApproachF(&camera->at.x, atTarget.x, 0.3f, 50.0f);
        Math_ApproachF(&camera->at.y, atTarget.y, 0.3f, 50.0f);
        Math_ApproachF(&camera->at.z, atTarget.z, 0.3f, 50.0f);

        Math_ApproachF(&camera->eye.x, eyeTarget.x, 0.3f, 50.0f);
        Math_ApproachF(&camera->eye.y, eyeTarget.y, 0.3f, 50.0f);
        Math_ApproachF(&camera->eye.z, eyeTarget.z, 0.3f, 50.0f);

        camera->eyeNext = camera->eye;
        camera->dist = eyeOffset.r;
        camera->fov = Camera_ScaledStepToCeilF(60.0f, camera->fov, camera->fovUpdateRate, 0.1f);
        camera->roll = Camera_ScaledStepToCeilS(0, camera->roll, 0.5f, 5);

        *should = false;
    });

    // Scale walking speed for giant mode
    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* speedTarget = va_arg(args, f32*);
        *speedTarget *= GetGiantScaleModifier();
    });

    // Scale swimming speed for giant mode
    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* incrStep = va_arg(args, f32*);
        f32* maxSpeed = va_arg(args, f32*);
        f32* speed = va_arg(args, f32*);
        f32* speedTarget = va_arg(args, f32*);
        f32 mod = GetGiantScaleModifier();
        *maxSpeed *= mod;
        Math_AsymStepToF(speed, *speedTarget * 0.8f * mod, *incrStep, (fabsf(*speed) * 0.02f) + 0.05f);
        *should = false;
    });

    // Scale climbing speed for giant mode
    COND_VB_SHOULD(VB_SET_CLIMB_SPEED, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* direction = va_arg(args, f32*);
        *direction *= GetGiantScaleModifier();
    });

    // Scale air control for giant mode
    COND_VB_SHOULD(VB_APPLY_AIR_CONTROL, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* speedTarget = va_arg(args, f32*);
        *speedTarget *= GetGiantScaleModifier();
    });

    // Scale Z-target speed for giant mode
    COND_VB_SHOULD(VB_ZTARGET_SPEED_CHECK, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* speed = va_arg(args, f32*);
        *speed *= GetGiantScaleModifier();
    });

    // Scale dive depth threshold for giant mode (fixes infinite resurface loop)
    COND_VB_SHOULD(VB_PLAYER_DIVE_DEPTH_CHECK, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* depthThreshold = va_arg(args, f32*);
        *depthThreshold *= GetGiantScaleModifier();
    });

    // Scale ledge climb height deltas for giant mode
    COND_VB_SHOULD(VB_PLAYER_LEDGE_CLIMB_FACTOR, CVAR, {
        if (!sIsGiant || sCsState != GMA_CS_IDLE) {
            return;
        }
        f32* climbDelta = va_arg(args, f32*);
        *climbDelta *= GetGiantScaleModifier();
    });
}

static RegisterShipInitFunc initFunc(RegisterGiantsMaskAnywhere, { CVAR_NAME });
