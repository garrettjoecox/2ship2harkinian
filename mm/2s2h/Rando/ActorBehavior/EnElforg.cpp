#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"

void EnElforg_SpawnSparkles(EnElforg* thisx, PlayState* play, s32 life);
}

void EnElforg_DrawCustom(Actor* thisx, PlayState* play) {
    EnElforg* enElforg = (EnElforg*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    RandoCheckId randoCheckId = RC_UNKNOWN;

    if (STRAY_FAIRY_TYPE(thisx) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
        randoCheckId = RC_CLOCK_TOWN_STRAY_FAIRY;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_COLLECTIBLE) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_COLLECTIBLE, STRAY_FAIRY_GET_FLAG(&enElforg->actor), play->sceneId);
        randoCheckId = randoStaticCheck.randoCheckId;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_FREE_FLOATING ||
               STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_ENEMY ||
               STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_BUBBLE) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_SWITCH, STRAY_FAIRY_GET_FLAG(&enElforg->actor), play->sceneId);
        randoCheckId = randoStaticCheck.randoCheckId;
    }

    if (randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    EnElforg_SpawnSparkles(enElforg, play, 16);
    thisx->shape.rot.y = thisx->shape.rot.y + 960;

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

// This handles the Stray fairy checks, as well as overriding the draw function for the Stray Fairies
void Rando::ActorBehavior::InitEnElforgBehavior() {
    static uint32_t onActorInitHookId = 0;
    static uint32_t shouldHook1Id = 0;
    static uint32_t shouldHook2Id = 0;
    static uint32_t shouldHook3Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInitHookId);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook2Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook3Id);

    onActorInitHookId = 0;
    shouldHook1Id = 0;
    shouldHook2Id = 0;
    shouldHook3Id = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInitHookId =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_ELFORG, [](Actor* actor) {
            EnElforg* enElforg = (EnElforg*)actor;
            RandoCheckId randoCheckId = RC_UNKNOWN;

            if (STRAY_FAIRY_TYPE(actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
                randoCheckId = RC_CLOCK_TOWN_STRAY_FAIRY;
            } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_COLLECTIBLE) {
                auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
                    FLAG_CYCL_SCENE_COLLECTIBLE, STRAY_FAIRY_GET_FLAG(&enElforg->actor), gPlayState->sceneId);
                randoCheckId = randoStaticCheck.randoCheckId;
            } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_FREE_FLOATING ||
                       STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_BUBBLE) {
                auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
                    FLAG_CYCL_SCENE_SWITCH, STRAY_FAIRY_GET_FLAG(&enElforg->actor), gPlayState->sceneId);
                randoCheckId = randoStaticCheck.randoCheckId;
            }

            if (randoCheckId == RC_UNKNOWN) {
                return;
            }

            auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

            // Set up custom draw function
            enElforg->actor.draw = EnElforg_DrawCustom;

            // Override area, as it's used in the sparkle effect
            switch (randoSaveCheck.randoItemId) {
                case RI_WOODFALL_STRAY_FAIRY:
                    enElforg->area = STRAY_FAIRY_AREA_WOODFALL;
                    break;
                case RI_SNOWHEAD_STRAY_FAIRY:
                    enElforg->area = STRAY_FAIRY_AREA_SNOWHEAD;
                    break;
                case RI_GREAT_BAY_STRAY_FAIRY:
                    enElforg->area = STRAY_FAIRY_AREA_GREAT_BAY;
                    break;
                case RI_STONE_TOWER_STRAY_FAIRY:
                    enElforg->area = STRAY_FAIRY_AREA_STONE_TOWER;
                    break;
                default:
                    enElforg->area = STRAY_FAIRY_AREA_CLOCK_TOWN;
                    break;
            }
        });

    shouldHook1Id = REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_ELFORG, {
        *should = false;
        Actor* actor = va_arg(args, Actor*);

        if (STRAY_FAIRY_TYPE(actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_STRAY_FAIRY];
            randoSaveCheck.eligible = true;
        }
    });

    shouldHook2Id = REGISTER_VB_SHOULD(GI_VB_KILL_CLOCK_TOWN_STRAY_FAIRY, {
        auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_STRAY_FAIRY];
        *should = randoSaveCheck.eligible;
    });

    // Stray fairies that are trapped by enemies have their draw func set later on, so we need to override that as well
    shouldHook2Id = REGISTER_VB_SHOULD(GI_VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY, {
        *should = false;

        Actor* actor = va_arg(args, Actor*);
        EnElforg* enElforg = (EnElforg*)actor;
        RandoCheckId randoCheckId = RC_UNKNOWN;

        if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_ENEMY) {
            auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
                FLAG_CYCL_SCENE_SWITCH, STRAY_FAIRY_GET_FLAG(&enElforg->actor), gPlayState->sceneId);
            randoCheckId = randoStaticCheck.randoCheckId;
        }

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        // Set up custom draw function
        enElforg->actor.draw = EnElforg_DrawCustom;

        // Override area, as it's used in the sparkle effect
        switch (randoSaveCheck.randoItemId) {
            case RI_WOODFALL_STRAY_FAIRY:
                enElforg->area = STRAY_FAIRY_AREA_WOODFALL;
                break;
            case RI_SNOWHEAD_STRAY_FAIRY:
                enElforg->area = STRAY_FAIRY_AREA_SNOWHEAD;
                break;
            case RI_GREAT_BAY_STRAY_FAIRY:
                enElforg->area = STRAY_FAIRY_AREA_GREAT_BAY;
                break;
            case RI_STONE_TOWER_STRAY_FAIRY:
                enElforg->area = STRAY_FAIRY_AREA_STONE_TOWER;
                break;
            default:
                enElforg->area = STRAY_FAIRY_AREA_CLOCK_TOWN;
                break;
        }
    });
}
