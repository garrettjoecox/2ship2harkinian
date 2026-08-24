#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Bean/z_obj_bean.h"

// Starts the mask transformation cutscene (Player_Action_86) from Player's current form.
void func_808388B8(PlayState* play, Player* player, PlayerTransformation playerForm);
// The "step back out of the white flash" action the transformation cutscene normally reaches by way of
// reloading the Player actor.
void Player_Action_87(Player* player, PlayState* play);
s32 Player_SetAction(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
}

#define CVAR_CUTSCENE_NAME "gEnhancements.Masks.AllMasksCutscene"
#define CVAR_CUTSCENE CVarGetInteger(CVAR_CUTSCENE_NAME, 0)
#define CVAR_BEANS_NAME "gEnhancements.Masks.CircusLeaderWatersBeans"
#define CVAR_BEANS CVarGetInteger(CVAR_BEANS_NAME, 0)
#define CVAR_TINY_NAME "gEnhancements.Masks.AllNightShrinksLink"
#define CVAR_TINY CVarGetInteger(CVAR_TINY_NAME, 0)
#define CVAR_FREEZE_NAME "gEnhancements.Masks.CouplesMaskFreezesWorld"
#define CVAR_FREEZE CVarGetInteger(CVAR_FREEZE_NAME, 0)

// Scale Player is drawn at while the All-Night Mask is on, against the usual 0.01f.
#define TINY_LINK_SCALE 0.004f

static PlayerMask sTrackedMask = PLAYER_MASK_NONE;
static bool sMaskCutsceneActive = false;
static bool sTinyScaleApplied = false;
static bool sWorldFrozen = false;

static bool IsRegularMask(PlayerMask mask) {
    return (mask >= PLAYER_MASK_TRUTH) && (mask <= PLAYER_MASK_SCENTS);
}

static Player* GetPlayer() {
    return (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;
}

// #region All masks play the transformation cutscene

/**
 * Mirrors the bail-out at the top of Player_ActionHandler_13, so a mask only earns a cutscene in the states
 * vanilla is willing to start one from. Anywhere else the mask just goes on instantly, as it always has.
 */
static bool PlayerCanStartMaskCutscene(Player* player) {
    if ((player == nullptr) || (gPlayState == nullptr)) {
        return false;
    }

    if ((player->actor.id != ACTOR_PLAYER) || (player->transformation != PLAYER_FORM_HUMAN)) {
        return false;
    }

    // Already transforming, talking, or otherwise not in control of ourselves
    if ((player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_TALKING)) || sMaskCutsceneActive ||
        Player_InBlockingCsMode(gPlayState, player) || (gPlayState->pauseCtx.state != PAUSE_STATE_OFF)) {
        return false;
    }

    return (player->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) ||
           (player->stateFlags1 & (PLAYER_STATE1_8000000 | PLAYER_STATE1_800000)) ||
           (player->stateFlags3 & PLAYER_STATE3_8) || (player->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT);
}

/**
 * Player_Action_86 draws whatever is in `currentMask`, so the mask is already equipped by the time we get
 * here and the cutscene simply plays over the top of it. Passing PLAYER_FORM_HUMAN while human leaves
 * `playerForm` untouched, which is what keeps this from becoming a real transformation.
 */
static void StartMaskCutscene(Player* player) {
    sMaskCutsceneActive = true;
    func_808388B8(gPlayState, player, PLAYER_FORM_HUMAN);
}

/**
 * Stands in for the Player actor reload the cutscene normally ends with. There is no new form to load, so we
 * drop straight into the action that fades the white flash back out and hands control over.
 */
static void EndMaskCutscene(Player* player) {
    sMaskCutsceneActive = false;

    // Player_Action_87 replays the mask-off animation when prevMask is set; we want the plain return to idle
    player->prevMask = PLAYER_MASK_NONE;
    Player_SetAction(gPlayState, player, Player_Action_87, 0);
    player->stateFlags1 |= (PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000);
    player->stateFlags3 |= PLAYER_STATE3_20000;
    // Player_SetAction zeroes unk_B10, so this has to follow it
    player->unk_B10[5] = 3.0f;
}

static void UpdateMaskCutscene(Player* player) {
    PlayerMask currentMask = static_cast<PlayerMask>(player->currentMask);

    // func_808388B8 sets PLAYER_STATE1_2 for the length of the cutscene. If it is gone while we still think
    // one is running, something else pulled Player out of it, so stop waiting for a takeover that never comes.
    if (sMaskCutsceneActive && !(player->stateFlags1 & PLAYER_STATE1_2)) {
        sMaskCutsceneActive = false;
    }

    if (currentMask == sTrackedMask) {
        return;
    }

    sTrackedMask = currentMask;

    // Only putting a mask on earns the cutscene; taking one off stays instant
    if (IsRegularMask(currentMask) && PlayerCanStartMaskCutscene(player)) {
        StartMaskCutscene(player);
    }
}

// #endregion

// #region Silly mask effects

static void UpdateTinyLink(Player* player) {
    // Hold off until the transformation cutscene has finished, so Link pops small as the flash clears
    bool shouldBeTiny =
        CVAR_TINY && (player->currentMask == PLAYER_MASK_ALL_NIGHT) && !(player->stateFlags1 & PLAYER_STATE1_2);

    if (shouldBeTiny) {
        Actor_SetScale(&player->actor, TINY_LINK_SCALE);
        sTinyScaleApplied = true;
    } else if (sTinyScaleApplied) {
        // Same scales func_80123140 hands out
        Actor_SetScale(&player->actor, (player->transformation == PLAYER_FORM_FIERCE_DEITY) ? 0.015f : 0.01f);
        sTinyScaleApplied = false;
    }
}

static void UpdateCouplesMaskFreeze() {
    Player* player = GetPlayer();

    // Never leave the world frozen once the mask is off
    if ((player == nullptr) || (player->currentMask != PLAYER_MASK_COUPLE)) {
        sWorldFrozen = false;
        return;
    }

    // L doubles as a kaleido page control, so only listen while the pause menu is closed
    if ((gPlayState->pauseCtx.state == PAUSE_STATE_OFF) &&
        CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_L)) {
        sWorldFrozen = !sWorldFrozen;
        Audio_PlaySfx(sWorldFrozen ? NA_SE_SY_DECIDE : NA_SE_SY_CANCEL);
    }
}

// #endregion

static void OnPlayerInit(Actor* actor) {
    Player* player = (Player*)actor;

    // Player is re-inited on every scene load and form change, and comes back wearing whatever the save says.
    // Resync so that isn't mistaken for the player equipping something.
    sTrackedMask = static_cast<PlayerMask>(player->currentMask);
    sMaskCutsceneActive = false;
    sTinyScaleApplied = false;
}

static void OnPlayerUpdate(Actor* actor) {
    Player* player = GetPlayer();

    if ((player == nullptr) || (actor != &player->actor)) {
        return;
    }

    if (CVAR_CUTSCENE) {
        UpdateMaskCutscene(player);
    }

    UpdateTinyLink(player);
}

void RegisterMaskEffects() {
    Player* player = GetPlayer();

    // Whatever is already on Link's face is not a mask he just equipped
    sTrackedMask = (player != nullptr) ? static_cast<PlayerMask>(player->currentMask) : PLAYER_MASK_NONE;

    if (!CVAR_CUTSCENE) {
        sMaskCutsceneActive = false;
    }
    if (!CVAR_TINY) {
        if ((player != nullptr) && sTinyScaleApplied) {
            Actor_SetScale(&player->actor, (player->transformation == PLAYER_FORM_FIERCE_DEITY) ? 0.015f : 0.01f);
        }
        sTinyScaleApplied = false;
    }
    if (!CVAR_FREEZE) {
        sWorldFrozen = false;
    }

    COND_ID_HOOK(OnActorInit, ACTOR_PLAYER, CVAR_CUTSCENE || CVAR_TINY, OnPlayerInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR_CUTSCENE || CVAR_TINY, OnPlayerUpdate);

    // Take over the end of the transformation cutscene when there is no form to swap to
    COND_VB_SHOULD(VB_MASK_TRANSFORMATION_SWAP_FORM, CVAR_CUTSCENE, {
        Player* player = va_arg(args, Player*);

        if (sMaskCutsceneActive && (player != nullptr)) {
            *should = false;
            EndMaskCutscene(player);
        }
    });

    // Circus Leader's Mask waters a planted bean, standing in for spring water or rain
    COND_VB_SHOULD(VB_BEAN_PLANT_BE_WATERED, CVAR_BEANS, {
        ObjBean* bean = va_arg(args, ObjBean*);
        Player* player = GetPlayer();

        if (*should || (bean == nullptr) || (player == nullptr) || (player->currentMask != PLAYER_MASK_CIRCUS_LEADER)) {
            return;
        }

        // The same conditions the rain path asks for: soft soil, a bean planted in it, player stood nearby
        if ((bean->unk_1FF != 0) && (bean->unk_1FE & 4) && (bean->dyna.actor.xzDistToPlayer < 300.0f)) {
            *should = true;
        }
    });

    // Couple's Mask freezes every actor but Link on L
    COND_HOOK(OnGameStateMainStart, CVAR_FREEZE, []() {
        if (gPlayState == NULL) {
            sWorldFrozen = false;
            return;
        }

        UpdateCouplesMaskFreeze();
    });

    // There is no COND_ macro for filtered hooks, so this mirrors RegisterPreventActorInitHooks
    static HOOK_ID sFreezeHookId = 0;
    if (sFreezeHookId != 0) {
        GameInteractor::Instance->UnregisterGameHookForFilter<GameInteractor::ShouldActorUpdate>(sFreezeHookId);
        sFreezeHookId = 0;
    }
    if (CVAR_FREEZE) {
        sFreezeHookId = GameInteractor::Instance->RegisterGameHookForFilter<GameInteractor::ShouldActorUpdate>(
            GameInteractor::HookFilter::SActorNotPlayer, [](Actor* actor, bool* should) {
                if (sWorldFrozen) {
                    *should = false;
                }
            });
    }
}

static RegisterShipInitFunc initFunc(RegisterMaskEffects,
                                     { CVAR_CUTSCENE_NAME, CVAR_BEANS_NAME, CVAR_TINY_NAME, CVAR_FREEZE_NAME });
