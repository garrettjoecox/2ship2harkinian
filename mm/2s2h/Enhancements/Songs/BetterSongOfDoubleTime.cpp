#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "assets/interface/week_static/week_static.h"
#include "objects/gameplay_keep/gameplay_keep.h"

Gfx* Gfx_DrawTexRect4b(Gfx* gfx, TexturePtr texture, s32 fmt, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                       s16 rectTop, s16 rectWidth, s16 rectHeight, s32 cms, s32 masks, s32 rects, u16 dsdx, u16 dtdy);
}

#define CVAR_NAME "gEnhancements.Songs.BetterSongOfDoubleTime"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static bool activelyChangingTime = false;
static u16 originalTime = CLOCK_TIME(0, 0);
static s32 originalDay = 0;

extern void UpdateGameTime(u16 gameTime);

static const char* sDoWeekTableCopy[] = {
    gClockDay1stTex,
    gClockDay2ndTex,
    gClockDayFinalTex,
};

static HOOK_ID onPlayerUpdateHookId = 0;

void OnPlayerUpdate(Actor* actor) {
    if (!activelyChangingTime) {
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHookId);
        onPlayerUpdateHookId = 0;
        return;
    }

    gPlayState->interfaceCtx.bAlpha = 255;

    Input* input = &gPlayState->state.input[0];

    // Pressing B should cancel the song
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySfx_MessageCancel();
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
        activelyChangingTime = false;

        gSaveContext.save.day = originalDay;
        UpdateGameTime(originalTime);
        Interface_NewDay(gPlayState, CURRENT_DAY);
        // This may have happened in the UpdateGameTime function, if there was a day/night difference, but
        // we need to ensure it happens regardless because the day may have changed even if the time is the same
        gPlayState->numSetupActors = ABS(gPlayState->numSetupActors);
        // Load environment values for new day
        func_800FEAF4(&gPlayState->envCtx);
    }

    // Pressing A should confirm the song
    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Audio_PlaySfx_MessageDecide();
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
        activelyChangingTime = false;

        gSaveContext.save.eventDayCount = CURRENT_DAY;
    }

    u16 INTERVAL = (CLOCK_TIME_MINUTE * 30);
    if (CHECK_BTN_ALL(input->cur.button, BTN_Z)) {
        INTERVAL = (CLOCK_TIME_MINUTE * 5);
    }

    // Analog stick should change the time
    if (input->cur.stick_x > 0) { // Advance time
        u16 newTime = CLAMP(gSaveContext.save.time + INTERVAL, -INT_MAX,
                            (gSaveContext.save.day == 3 && gSaveContext.save.time < CLOCK_TIME(6, 0))
                                ? (CLOCK_TIME(6, 0) - CLOCK_TIME_HOUR)
                                : INT_MAX);
        if (newTime > CLOCK_TIME(6, 0) && gSaveContext.save.time < CLOCK_TIME(6, 0)) {
            gSaveContext.save.day = CLAMP(gSaveContext.save.day + 1, originalDay, 3);
            Interface_NewDay(gPlayState, CURRENT_DAY);
            func_800FEAF4(&gPlayState->envCtx);
        }
        UpdateGameTime(newTime);
    } else if (input->cur.stick_x < 0) { // Reverse time
        u16 newTime = CLAMP(gSaveContext.save.time - INTERVAL,
                            (gSaveContext.save.day == originalDay &&
                             ((gSaveContext.save.time > CLOCK_TIME(6, 0) && originalTime > CLOCK_TIME(6, 0)) ||
                              (gSaveContext.save.time < CLOCK_TIME(6, 0) && originalTime < CLOCK_TIME(6, 0))))
                                ? originalTime
                                : -INT_MAX,
                            INT_MAX);
        if (newTime < CLOCK_TIME(6, 0) && gSaveContext.save.time > CLOCK_TIME(6, 0)) {
            gSaveContext.save.day = CLAMP(gSaveContext.save.day - 1, originalDay, 3);
            Interface_NewDay(gPlayState, CURRENT_DAY);
            func_800FEAF4(&gPlayState->envCtx);
        }
        UpdateGameTime(newTime);
    }
}

void DrawTextRec(f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx, f32 dy) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);

    f32 w = 8.0f * z;
    s32 ulx = (x - w) * 4.0f;
    s32 lrx = (x + w) * 4.0f;

    f32 h = 12.0f * z;
    s32 uly = (y - h) * 4.0f;
    s32 lry = (y + h) * 4.0f;

    f32 unk = 1024 * (1.0f / z);
    s32 dsdx = unk * dx;
    s32 dtdy = dy * unk;

    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void RegisterBetterSongOfDoubleTime() {
    COND_HOOK(OnSceneInit, CVAR, [](s8 sceneId, s8 spawnNum) {
        // In case we didn't properly reset this variable
        activelyChangingTime = false;
        originalTime = CLOCK_TIME(0, 0);
        originalDay = 0;
    });

    COND_VB_SHOULD(VB_DISPLAY_SONG_OF_DOUBLE_TIME_PROMPT, CVAR, {
        *should = false;
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_DOUBLE_TIME;
        activelyChangingTime = true;
        originalTime = gSaveContext.save.time;
        originalDay = gSaveContext.save.day;

        onPlayerUpdateHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
            ACTOR_PLAYER, OnPlayerUpdate);
    });

    COND_VB_SHOULD(VB_PREVENT_CLOCK_DISPLAY, CVAR, {
        if (!activelyChangingTime) {
            return;
        }

        OPEN_DISPS(gPlayState->state.gfxCtx);
        Gfx_SetupDL39_Overlay(gPlayState->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        DrawTextRec(53.0f, 191.0f, 1.0f, 0, 0, -1.0f, 1.0f);
        DrawTextRec(270.0f, 191.0f, 1.0f, 0, 0, 1.0f, 1.0f);
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        DrawTextRec(69.0f, 195.0f, 1.0f, 0, 0, -1.0f, 1.0f);
        DrawTextRec(254.0f, 195.0f, 1.0f, 0, 0, 1.0f, 1.0f);
        CLOSE_DISPS(gPlayState->state.gfxCtx);
    });

    COND_VB_SHOULD(VB_ALLOW_SONG_DOUBLE_TIME_ON_FINAL_NIGHT, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterBetterSongOfDoubleTime, { CVAR_NAME });
