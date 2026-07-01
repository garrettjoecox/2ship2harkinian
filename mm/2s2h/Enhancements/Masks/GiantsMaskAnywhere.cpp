#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "regs.h"
#include "variables.h"
#include "z64shrink_window.h"
}

#define CVAR_NAME "gEnhancements.Masks.GiantsMaskAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

typedef enum {
    GMA_CS_IDLE = 0,
    GMA_CS_MASK_ON = 1,
    GMA_CS_MASK_ON_SKIPPED = 2,
    GMA_CS_MASK_OFF = 10,
    GMA_CS_MASK_OFF_SKIPPED = 11,
    GMA_CS_DONE = 20,
    GMA_CS_POST = 21,
} GmaCsState;

typedef enum {
    GMA_FLASH_NOT_STARTED = 0,
    GMA_FLASH_STARTED = 1,
    GMA_FLASH_INCREASE_ALPHA = 2,
    GMA_FLASH_DECREASE_ALPHA = 3,
} GmaFlashState;

static GmaCsState sCsState = GMA_CS_IDLE;
static GmaFlashState sFlashState = GMA_FLASH_NOT_STARTED;
static u32 sCsTimer = 0;
static s16 sSubCamId = SUB_CAM_ID_DONE;
static f32 sPlayerScale = 0.01f;
static f32 sNextScaleFactor = 10.0f;
static f32 sSubCamDistZ = 60.0f;
static f32 sSubCamEyeOffsetY = 10.0f;
static f32 sSubCamAtOffsetY = 23.0f;
static f32 sSubCamAtOffsetTargetY = 273.0f;
static f32 sSubCamUpRotZScale = 0.0f;
static f32 sSubCamAtVel = 0.0f;
static s16 sFlashAlpha = 0;
static bool sTransformingToGiant = false;
static bool sHasSeenGrowCutscene = false;
static bool sHasSeenShrinkCutscene = false;
static bool sResetMarked = false;
static bool sPlayerWasDead = false;

static bool IsGiant() {
    return gSaveContext.save.shipSaveInfo.giantsMaskAnywhereIsGiant != 0;
}

static void SetIsGiant(bool isGiant) {
    gSaveContext.save.shipSaveInfo.giantsMaskAnywhereIsGiant = isGiant ? 1 : 0;
}

static bool IsTwinmoldSceneId(s16 sceneId) {
    return sceneId == SCENE_INISIE_BS;
}

static bool IsFeatureScene(PlayState* play) {
    return play != nullptr && !IsTwinmoldSceneId(play->sceneId);
}

static bool ShouldResetForRespawn() {
    return gSaveContext.respawnFlag == -5 || gSaveContext.respawnFlag == 1 || gSaveContext.respawnFlag == -7;
}

static void SetGiantsMaskMagicConsumeTimer() {
    R_MAGIC_CONSUME_TIMER_GIANTS_MASK = KREG(14) + 20;
}

static f32 GetSimpleScaleModifier() {
    return IsGiant() ? 10.0f : 1.0f;
}

static f32 GetSimpleInvertedScaleModifier() {
    return IsGiant() ? 0.1f : 1.0f;
}

static f32 GetMovementSpeedCap(Player* player) {
    if (player != nullptr && player->unk_B50 > 0.0f) {
        return player->unk_B50;
    }
    return R_RUN_SPEED_LIMIT / 100.0f;
}

static f32 GetHeightScaleModifier() {
    return sPlayerScale * 100.0f;
}

static bool ShouldApplyGiantScale(Player* player) {
    return CVAR && IsFeatureScene(gPlayState) && player != nullptr && player->actor.id == ACTOR_PLAYER &&
           (IsGiant() || sCsState != GMA_CS_IDLE);
}

static bool ShouldApplyIdleGiantBehavior(Player* player) {
    return ShouldApplyGiantScale(player) && IsGiant() && sCsState == GMA_CS_IDLE;
}

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

static bool AgePropertiesAreGiant(PlayerAgeProperties* props) {
    return props != nullptr && props->ceilingCheckHeight >= 200.0f;
}

static void GrowRegs() {
    REG(48) *= 10;
    REG(19) *= 10;
    REG(32) /= 10;
    REG(36) /= 10;
    REG(37) /= 10;
    REG(38) /= 10;
    REG(43) *= 10;
    REG(45) *= 10;
    REG(68) *= 10;
    IREG(66) *= 10;
    IREG(69) /= 10;
    MREG(95) /= 10;
}

static void ClearSavedGiantMask() {
    if (gSaveContext.save.equippedMask == PLAYER_MASK_GIANT) {
        gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
    }
}

static void StopCutscene(PlayState* play) {
    if (play != nullptr && sSubCamId != SUB_CAM_ID_DONE) {
        func_80169AFC(play, sSubCamId, 0);
        sSubCamId = SUB_CAM_ID_DONE;
    }

    if (play != nullptr && sCsState != GMA_CS_IDLE) {
        Cutscene_StopManual(play, &play->csCtx);
        Play_DisableMotionBlur();
    }

    R_PLAY_FILL_SCREEN_ON = false;
    sFlashState = GMA_FLASH_NOT_STARTED;
    sFlashAlpha = 0;
}

static void ClearCutsceneState(PlayState* play) {
    StopCutscene(play);
    sCsState = GMA_CS_IDLE;
    sCsTimer = 0;
    sSubCamId = SUB_CAM_ID_DONE;
    sSubCamDistZ = 60.0f;
    sSubCamEyeOffsetY = 10.0f;
    sSubCamAtOffsetY = 23.0f;
    sSubCamAtOffsetTargetY = 273.0f;
    sSubCamUpRotZScale = 0.0f;
    sSubCamAtVel = 0.0f;
    sTransformingToGiant = false;
}

static void ResetPlayerGiantState(Player* player, bool clearMask) {
    if (player == nullptr) {
        return;
    }

    if (AgePropertiesAreGiant(player->ageProperties)) {
        ShrinkAgeProperties(player->ageProperties);
    }

    player->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;
    Actor_SetScale(&player->actor, 0.01f);

    if (clearMask) {
        if (player->currentMask == PLAYER_MASK_GIANT) {
            player->currentMask = PLAYER_MASK_NONE;
        }
        ClearSavedGiantMask();

        if (player->currentBoots == PLAYER_BOOTS_GIANT) {
            player->currentBoots = PLAYER_BOOTS_HYLIAN;
            player->prevBoots = PLAYER_BOOTS_HYLIAN;
            if (gPlayState != nullptr) {
                func_80123140(gPlayState, player);
                Magic_Reset(gPlayState);
            }
        }
    }
}

static void MarkReset() {
    sResetMarked = true;
}

static void TryReset(Player* player) {
    if (!sResetMarked) {
        return;
    }

    SetIsGiant(false);
    sPlayerScale = 0.01f;
    sNextScaleFactor = 10.0f;
    ClearCutsceneState(gPlayState);
    ResetPlayerGiantState(player, true);
    sResetMarked = false;
}

static void StartCutscene(PlayState* play, Player* player, bool transformingToGiant) {
    sTransformingToGiant = transformingToGiant;
    sCsState = transformingToGiant ? GMA_CS_MASK_ON : GMA_CS_MASK_OFF;
    sCsTimer = 0;
    sSubCamAtVel = 0.0f;
    sSubCamUpRotZScale = 0.0f;
    sFlashState = GMA_FLASH_NOT_STARTED;
    sFlashAlpha = 0;
    sPlayerScale = transformingToGiant ? 0.01f : 0.1f;
    sNextScaleFactor = transformingToGiant ? 10.0f : 0.1f;

    Cutscene_StartManual(play, &play->csCtx);
    sSubCamId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
    Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
    Play_EnableMotionBlur(150);

    f32 playerHeight = Player_GetHeight(player);
    if (transformingToGiant) {
        sSubCamEyeOffsetY = 10.0f;
        sSubCamDistZ = 60.0f;
        sSubCamAtOffsetY = playerHeight * 0.53f;
        sSubCamAtOffsetTargetY = playerHeight * 6.2f;
    } else {
        sSubCamEyeOffsetY = 10.0f;
        sSubCamDistZ = 200.0f;
        sSubCamAtOffsetY = playerHeight * 0.62f;
        sSubCamAtOffsetTargetY = playerHeight * 0.053f;
    }
}

static void UpdateFlash() {
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
            [[fallthrough]];

        case GMA_FLASH_INCREASE_ALPHA:
            sFlashAlpha += 40;
            if (sFlashAlpha >= 400) {
                sFlashState = GMA_FLASH_DECREASE_ALPHA;
            }
            alpha = CLAMP_MAX(sFlashAlpha, 255);
            R_PLAY_FILL_SCREEN_ALPHA = alpha;
            break;

        case GMA_FLASH_DECREASE_ALPHA:
            sFlashAlpha -= 40;
            if (sFlashAlpha <= 0) {
                sFlashAlpha = 0;
                sFlashState = GMA_FLASH_NOT_STARTED;
                R_PLAY_FILL_SCREEN_ON = false;
            } else {
                alpha = CLAMP_MAX(sFlashAlpha, 255);
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

    if (sSubCamId == SUB_CAM_ID_DONE) {
        return;
    }

    Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVecZ(sSubCamDistZ, &subCamEyeOffset);

    subCamEye.x = player->actor.world.pos.x + subCamEyeOffset.x;
    subCamEye.y = player->actor.world.pos.y + subCamEyeOffset.y + sSubCamEyeOffsetY;
    subCamEye.z = player->actor.world.pos.z + subCamEyeOffset.z;

    subCamAt.x = player->actor.world.pos.x;
    subCamAt.y = player->actor.world.pos.y + sSubCamAtOffsetY;
    subCamAt.z = player->actor.world.pos.z;

    Matrix_RotateZF(Math_SinS(sCsTimer * 1512) * sSubCamUpRotZScale, MTXMODE_APPLY);
    Matrix_MultVecY(1.0f, &subCamUp);

    Play_SetCameraAtEyeUp(play, sSubCamId, &subCamAt, &subCamEye, &subCamUp);
    ShrinkWindow_Letterbox_SetSizeTarget(27);
}

static void FinishCutscene(PlayState* play, Player* player) {
    player->stateFlags1 &= ~PLAYER_STATE1_100;
    player->meleeWeaponState = PLAYER_MELEE_WEAPON_STATE_0;

    StopCutscene(play);

    SetIsGiant(sTransformingToGiant);
    sPlayerScale = sTransformingToGiant ? 0.1f : 0.01f;
    sNextScaleFactor = sTransformingToGiant ? 0.1f : 10.0f;
    sCsState = GMA_CS_POST;
    sCsTimer = 0;
}

static void UpdateCutscene(PlayState* play, Player* player) {
    bool done = false;
    static constexpr u16 skipButtons = BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT;

    sCsTimer++;
    if (sSubCamId != SUB_CAM_ID_DONE) {
        CutsceneManager_ClearNextCutscenes();
    }

    switch (sCsState) {
        case GMA_CS_MASK_ON:
            if (sCsTimer < 80 && sHasSeenGrowCutscene &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button, skipButtons)) {
                sCsState = GMA_CS_MASK_ON_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer >= 50) {
                if (sCsTimer == (u32)(BREG(43) + 60)) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_GIANT);
                }
                Math_ApproachF(&sSubCamDistZ, 200.0f, 0.1f, sSubCamAtVel * 640.0f);
                Math_ApproachF(&sSubCamAtOffsetY, sSubCamAtOffsetTargetY, 0.1f, sSubCamAtVel * 150.0f);
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
                sHasSeenGrowCutscene = true;
                done = true;
            }
            break;

        case GMA_CS_MASK_ON_SKIPPED:
            Math_ApproachF(&sPlayerScale, 0.1f, 0.5f, 0.05f);
            done = sCsTimer >= 8;
            break;

        case GMA_CS_MASK_OFF:
            if (sCsTimer < 30 && sHasSeenShrinkCutscene &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button, skipButtons)) {
                sCsState = GMA_CS_MASK_OFF_SKIPPED;
                sFlashState = GMA_FLASH_STARTED;
                sCsTimer = 0;
                break;
            }

            if (sCsTimer == (u32)(BREG(44) + 10)) {
                Audio_PlaySfx(NA_SE_PL_TRANSFORM_NORAML);
            }

            Math_ApproachF(&sSubCamDistZ, 60.0f, 0.1f, sSubCamAtVel * 640.0f);
            Math_ApproachF(&sSubCamAtOffsetY, sSubCamAtOffsetTargetY, 0.1f, sSubCamAtVel * 150.0f);
            Math_ApproachF(&sPlayerScale, 0.01f, 0.1f, 0.003f);
            Math_ApproachF(&sSubCamAtVel, 2.0f, 1.0f, 0.01f);

            if (sCsTimer == 42) {
                sFlashState = GMA_FLASH_STARTED;
            }

            if (sCsTimer > 50) {
                sHasSeenShrinkCutscene = true;
                done = true;
            }
            break;

        case GMA_CS_MASK_OFF_SKIPPED:
            Math_ApproachF(&sPlayerScale, 0.01f, 0.5f, 0.005f);
            done = sCsTimer >= 8;
            break;

        default:
            break;
    }

    if (done) {
        FinishCutscene(play, player);
    } else {
        f32 scale = sPlayerScale;
        if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
            scale *= 1.5f;
        }
        Actor_SetScale(&player->actor, scale);
        UpdateSubCamera(play, player);
    }
}

static void MaintainGiantState(PlayState* play, Player* player) {
    if (sCsState == GMA_CS_POST) {
        if (IsGiant()) {
            if (!AgePropertiesAreGiant(player->ageProperties)) {
                GrowAgeProperties(player->ageProperties);
            }
            if (REG(68) > -200) {
                GrowRegs();
            }
        } else {
            ResetPlayerGiantState(player, false);
        }
        sCsState = GMA_CS_IDLE;
        return;
    }

    if (sCsState != GMA_CS_IDLE) {
        return;
    }

    if (!IsGiant()) {
        ResetPlayerGiantState(player, false);
        return;
    }

    if (player->transformation != PLAYER_FORM_HUMAN) {
        MarkReset();
        TryReset(player);
        return;
    }

    player->currentMask = PLAYER_MASK_GIANT;
    gSaveContext.save.equippedMask = PLAYER_MASK_GIANT;

    if (player->currentBoots != PLAYER_BOOTS_GIANT) {
        SetGiantsMaskMagicConsumeTimer();
        Magic_Consume(play, 0, MAGIC_CONSUME_GIANTS_MASK);
        player->currentBoots = PLAYER_BOOTS_GIANT;
        player->prevBoots = PLAYER_BOOTS_GIANT;
        func_80123140(play, player);
    }

    if (!AgePropertiesAreGiant(player->ageProperties)) {
        GrowAgeProperties(player->ageProperties);
    }
    if (REG(68) > -200) {
        GrowRegs();
    }

    player->actor.flags |= ACTOR_FLAG_CAN_PRESS_HEAVY_SWITCHES;

    f32 scale = 0.1f;
    if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
        scale *= 1.5f;
    }
    Actor_SetScale(&player->actor, scale);
    sPlayerScale = 0.1f;
    sNextScaleFactor = 0.1f;
}

static void BeforePlayerUpdate(Actor* actor, bool*) {
    Player* player = (Player*)actor;
    PlayState* play = gPlayState;

    if (!CVAR || !IsFeatureScene(play) || ShouldResetForRespawn()) {
        MarkReset();
        TryReset(player);
        return;
    }

    if ((player->stateFlags1 & PLAYER_STATE1_100) && sCsState == GMA_CS_IDLE) {
        StartCutscene(play, player, !IsGiant());
    }

    if (sCsState != GMA_CS_IDLE) {
        UpdateCutscene(play, player);
    }

    UpdateFlash();
    MaintainGiantState(play, player);
}

static void ResetForSceneState(s16 sceneId) {
    ClearCutsceneState(gPlayState);

    if (!CVAR || IsTwinmoldSceneId(sceneId) || ShouldResetForRespawn()) {
        MarkReset();
        SetIsGiant(false);
        ClearSavedGiantMask();
        sPlayerScale = 0.01f;
        sNextScaleFactor = 10.0f;
        return;
    }

    sPlayerScale = IsGiant() ? 0.1f : 0.01f;
    sNextScaleFactor = IsGiant() ? 0.1f : 10.0f;
}

static void RegisterGiantsMaskAnywhere() {
    if (!CVAR) {
        MarkReset();
        TryReset(gPlayState != nullptr ? GET_PLAYER(gPlayState) : nullptr);
    }

    COND_HOOK(OnSceneInit, true, [](s8 sceneId, s8) { ResetForSceneState(sceneId); });

    COND_HOOK(OnSaveLoad, true, [](s16) {
        if (!CVAR) {
            SetIsGiant(false);
            ClearSavedGiantMask();
        }
        sPlayerScale = IsGiant() ? 0.1f : 0.01f;
        sNextScaleFactor = IsGiant() ? 0.1f : 10.0f;
    });

    COND_HOOK(BeforeMoonCrash, CVAR, []() {
        MarkReset();
        SetIsGiant(false);
        ClearSavedGiantMask();
    });

    COND_HOOK(AfterEndOfCycleSave, CVAR, []() {
        MarkReset();
        SetIsGiant(false);
        ClearSavedGiantMask();
    });

    COND_HOOK(OnGameStateUpdate, CVAR, []() {
        if (gPlayState == nullptr) {
            return;
        }

        Player* player = GET_PLAYER(gPlayState);
        if (player == nullptr) {
            return;
        }

        bool playerIsDead = player->stateFlags1 & PLAYER_STATE1_DEAD;
        if (playerIsDead && !sPlayerWasDead) {
            MarkReset();
            TryReset(player);
        }
        sPlayerWasDead = playerIsDead;
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_PLAYER, CVAR, BeforePlayerUpdate);

    COND_VB_SHOULD(VB_DISABLE_GIANTS_MASK, CVAR, { *should = false; });

    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        Player* player = gPlayState != nullptr ? GET_PLAYER(gPlayState) : nullptr;

        if ((itemId != nullptr) && (*itemId == ITEM_MASK_GIANT) && IsFeatureScene(gPlayState) && (player != nullptr) &&
            (player->transformation == PLAYER_FORM_HUMAN)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_TRANSFORMATION_STATE, CVAR, {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        u32* stateFlags1 = va_arg(args, u32*);

        if ((stateFlags1 != nullptr) && IsFeatureScene(play) && (player != nullptr)) {
            *stateFlags1 |= PLAYER_STATE1_10000000;
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_CLEAR_ON_LOAD, true, {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);

        if (!CVAR || !IsFeatureScene(play) || ShouldResetForRespawn()) {
            MarkReset();
            SetIsGiant(false);
            sPlayerScale = 0.01f;
            sNextScaleFactor = 10.0f;
            return;
        }

        SetIsGiant(true);
        sPlayerScale = 0.1f;
        sNextScaleFactor = 0.1f;
        SetGiantsMaskMagicConsumeTimer();
        gSaveContext.magicState = MAGIC_STATE_CONSUME_GIANTS_MASK;
        if (player != nullptr) {
            player->currentMask = PLAYER_MASK_GIANT;
        }
        *should = false;
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_CEILING_CHECK_HEIGHT, CVAR, {
        va_arg(args, Player*);
        PlayerItemAction itemAction = *va_arg(args, PlayerItemAction*);
        f32* ceilingCheckHeight = va_arg(args, f32*);

        if (itemAction == PLAYER_IA_MASK_GIANT) {
            *ceilingCheckHeight *= sNextScaleFactor;
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_AUTO_REMOVE, CVAR, {
        Player* player = va_arg(args, Player*);
        if (ShouldApplyIdleGiantBehavior(player) && (player->stateFlags1 & PLAYER_STATE1_8000000)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_USE_ITEM_CONSIDER_ITEM_ACTION, CVAR, {
        PlayerItemAction itemAction = *va_arg(args, PlayerItemAction*);
        Player* player = GET_PLAYER(gPlayState);

        if (itemAction == PLAYER_IA_MASK_GIANT && player != nullptr && player->transformation == PLAYER_FORM_HUMAN &&
            IsGiant()) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_DISABLE_ITEM_UNDERWATER, CVAR, {
        s32 item = va_arg(args, s32);
        Player* player = GET_PLAYER(gPlayState);

        if (item == ITEM_MASK_GIANT && player != nullptr && player->transformation == PLAYER_FORM_HUMAN && IsGiant() &&
            Player_GetEnvironmentalHazard(gPlayState) > PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_MELEE_WEAPON_DAMAGE, CVAR, {
        Player* player = va_arg(args, Player*);
        u32* dmgFlags = va_arg(args, u32*);
        va_arg(args, s32*);

        if (ShouldApplyIdleGiantBehavior(player)) {
            *dmgFlags = DMG_POWDER_KEG | DMG_EXPLOSIVES | DMG_GORON_PUNCH | DMG_UNK_0x1E;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_GET_HEIGHT, CVAR, {
        Player* player = va_arg(args, Player*);
        f32* height = va_arg(args, f32*);

        if (ShouldApplyGiantScale(player)) {
            *height *= GetHeightScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_PLAYER_CAN_USE_DOOR, CVAR, {
        va_arg(args, Actor*);
        va_arg(args, f32*);

        if (IsGiant() && IsFeatureScene(gPlayState)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_CAN_BE_GRABBED, CVAR, {
        Player* player = va_arg(args, Player*);

        if (ShouldApplyIdleGiantBehavior(player)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_SHOULD_BE_KNOCKED_OVER, CVAR, {
        va_arg(args, PlayState*);
        Player* player = va_arg(args, Player*);
        va_arg(args, s32);

        if (ShouldApplyGiantScale(player) && IsGiant()) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GYORG_STOP_CATCHING_PLAYER, CVAR, {
        va_arg(args, Actor*);
        Player* player = va_arg(args, Player*);

        if (ShouldApplyIdleGiantBehavior(player)) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_HIT_DISTANCE, CVAR, {
        va_arg(args, Vec3f*);
        Actor* hittingActor = va_arg(args, Actor*);
        f32* hitDistanceSq = va_arg(args, f32*);

        if ((hittingActor != nullptr) && ShouldApplyIdleGiantBehavior((Player*)hittingActor) &&
            (hitDistanceSq != nullptr)) {
            *hitDistanceSq = 0.0f;
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_JUMPSLASH_VELOCITY, CVAR, {
        Player* player = va_arg(args, Player*);
        f32* linearVelocity = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (linearVelocity != nullptr)) {
            *linearVelocity *= GetSimpleScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_SCALE_PLAYER_VALUE, CVAR, {
        Player* player = va_arg(args, Player*);
        f32* value = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (value != nullptr)) {
            *value *= GetSimpleScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_GIANTS_MASK_INVERT_PLAYER_VALUE, CVAR, {
        Player* player = va_arg(args, Player*);
        f32* value = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (value != nullptr)) {
            *value *= GetSimpleInvertedScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR, {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
        f32* speedTarget = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (speedTarget != nullptr)) {
            *speedTarget *= GetSimpleScaleModifier();
            *speedTarget = CLAMP_MAX(*speedTarget, GetMovementSpeedCap(player));
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR, {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
        f32* incrStep = va_arg(args, f32*);
        f32* maxSpeed = va_arg(args, f32*);
        f32* speed = va_arg(args, f32*);
        f32* speedTarget = va_arg(args, f32*);

        (void)incrStep;
        (void)speed;

        if (ShouldApplyIdleGiantBehavior(player) && (maxSpeed != nullptr) && (speedTarget != nullptr)) {
            *speedTarget *= GetSimpleScaleModifier();
            *speedTarget = CLAMP_MAX(*speedTarget, *maxSpeed / 0.8f);
        }
    });

    COND_VB_SHOULD(VB_CLAMP_ANIMATION_SPEED, CVAR, {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
        f32* animationSpeed = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (animationSpeed != nullptr)) {
            *animationSpeed *= GetSimpleInvertedScaleModifier();
            *animationSpeed = CLAMP(*animationSpeed, 1.0f, 2.5f);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SET_CLIMB_SPEED, CVAR, {
        if (IsGiant() && sCsState == GMA_CS_IDLE) {
            f32* direction = va_arg(args, f32*);
            *direction *= GetSimpleScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_APPLY_AIR_CONTROL, CVAR, {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
        f32* speedTarget = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (speedTarget != nullptr)) {
            *speedTarget *= GetSimpleScaleModifier();
            *speedTarget = CLAMP_MAX(*speedTarget, GetMovementSpeedCap(player));
        }
    });

    COND_VB_SHOULD(VB_ZTARGET_SPEED_CHECK, CVAR, {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
        f32* speed = va_arg(args, f32*);

        if (ShouldApplyIdleGiantBehavior(player) && (speed != nullptr)) {
            *speed *= GetSimpleScaleModifier();
            *speed = CLAMP_MAX(*speed, GetMovementSpeedCap(player));
        }
    });

    COND_VB_SHOULD(VB_PLAYER_DIVE_DEPTH_CHECK, CVAR, {
        if (IsGiant() && sCsState == GMA_CS_IDLE) {
            f32* depthThreshold = va_arg(args, f32*);
            *depthThreshold *= GetSimpleScaleModifier();
        }
    });

    COND_VB_SHOULD(VB_PLAYER_LEDGE_CLIMB_FACTOR, CVAR, {
        if (IsGiant() && sCsState == GMA_CS_IDLE) {
            f32* climbDelta = va_arg(args, f32*);
            *climbDelta *= GetSimpleScaleModifier();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGiantsMaskAnywhere, { CVAR_NAME });
