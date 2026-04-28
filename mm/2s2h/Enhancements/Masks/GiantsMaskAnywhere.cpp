#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "z64shrink_window.h"
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

static void ScaleRegsForGiant() {
    // Assign Hylian-boot values scaled ×10 (or ÷10) directly.
    // The vanilla game switches to PLAYER_BOOTS_GIANT (with lower base values
    // tuned for the boss room) when the mask is equipped. Guard-based multipliers
    // would scale those reduced values and produce wrong results, so we hardcode
    // the Hylian-derived targets. Called every frame to counteract any
    // Player_SetBootData resets (room transitions, water, etc.).
    REG(19) = 2000;           // Hylian 200  × 10: run acceleration
    R_DECELERATE_RATE = 8000; // Hylian 800  × 10: idle deceleration
    R_RUN_SPEED_LIMIT = 5500; // Hylian 550  × 10: max run speed
    REG(48) = 3700;           // default 370 × 10: backwalk threshold
    REG(68) = -100;          // Hylian -100 × 10: gravity
    IREG(66) = 5400;          // Hylian 540  × 10: baby-jump threshold
    IREG(68) = 125;           // Hylian 125  (not scaled, but BOOTS_GIANT sets 60)
    REG(32) = 30;             // Hylian 300  / 10: sidewalk anim multiplier
    REG(36) = 40;             // Hylian 400  / 10: backwalk anim multiplier
    REG(37) = 80;             // Hylian 800  / 10: walk threshold
    REG(38) = 40;             // Hylian 400  / 10: walk anim multiplier
    IREG(69) = 40;            // Hylian 400  / 10: baby-jump speed multiplier
    MREG(95) = 20;            // Hylian 200  / 10: bow sidewalk anim
}

static void GiantMask_Reg_Grow() {
    //REG(27);        // turning circle
    REG(48) *= 10; // slow backwalk animation threshold
    REG(19) *= 10;  // run acceleration // deceleration needs hook 806F9FE8
    // REG(30) /= 10; // base sidewalk animation speed
    REG(32) /= 10; // sidewalk animation speed multiplier
    // REG(34) *= 10; // ? unused ?
    // REG(35) *= 10; // base slow backwalk
    REG(36) /= 10; // slow backwalk animation speed multiplier
    REG(37) /= 10; // walk speed threshold
    REG(38) /= 10; // walk animation speed multiplier
    // REG(39) *= 10; // base walk animation speed
    REG(43) *= 10;  // idle deceleration
    REG(45) *= 10;  // running speed
    REG(68) *= 10;  // gravity
    // REG(69) *= 10;  // jump strength
    IREG(66) *= 10; // baby jump threshold
    // IREG(67) *= 10; // normal jump speed
    // IREG(68) *= 10; // baby jump base speed
    IREG(69) /= 10; // baby jump speed multiplier
    MREG(95) /= 10; // bow sidewalk animation?
}

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
    GiantMask_Reg_Grow();
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

    // Scale movement REGs every frame while giant to counteract Player_SetBootData resets.
    // if (sIsGiant) {
    //     ScaleRegsForGiant();
    // }

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

    // Scale the player's effective height so the camera tracks at the correct
    // height and distance for a giant player. Camera_GetFocalActorHeight calls
    // Player_GetHeight, and Camera_Normal0 also scales camera distances by it.
    COND_VB_SHOULD(VB_PLAYER_HEIGHT, CVAR, {
        Player* playerArg = va_arg(args, Player*);
        f32* heightArg = va_arg(args, f32*);
        if (sIsGiant) {
            *heightArg *= playerArg->actor.scale.y / 0.01f;
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
