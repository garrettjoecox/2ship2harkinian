#include <libultraship/bridge.h>
#include "Enhancements/GameInteractor/GameInteractor.h"
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "Rando/Rando.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include <functions.h>
#include "assets/objects/object_gi_melody/object_gi_melody.h"
#include "overlays/actors/ovl_En_Osn/z_en_osn.h"

void EnOsn_Talk(EnOsn* enOsn, PlayState* play);
void EnOsn_Idle(EnOsn* enOsn, PlayState* play);
void PlayerCall_Init(Actor* thisx, PlayState* play);
void PlayerCall_Update(Actor* thisx, PlayState* play);
void PlayerCall_Draw(Actor* thisx, PlayState* play);
}

#define OSN_STATE_END_CONVERSATION (1 << 5)

void RegisterSkipLearningSongOfHealing() {
    REGISTER_VB_SHOULD(GI_VB_OSN_TEACH_SONG_OF_HEALING, {
        // TODO: Currently forced on for rando, maybe won't be when you shuffle only song locations (need to override
        // learning mechanism)
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0) || IS_RANDO) {
            EnOsn* enOsn = va_arg(args, EnOsn*);
            Player* player = GET_PLAYER(gPlayState);

            *should = false;

            // Transform the player into human form
            s16 objectId = OBJECT_LINK_NUTS;
            gActorOverlayTable[ACTOR_PLAYER].initInfo->objectId = objectId;
            func_8012F73C(&gPlayState->objectCtx, player->actor.objectSlot, objectId);
            player->actor.objectSlot = Object_GetSlot(&gPlayState->objectCtx, GAMEPLAY_KEEP);
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            s32 objectSlot =
                Object_GetSlot(&gPlayState->objectCtx, gActorOverlayTable[ACTOR_PLAYER].initInfo->objectId);
            player->actor.objectSlot = objectSlot;
            player->actor.shape.rot.z = GET_PLAYER_FORM + 1;
            player->actor.init = PlayerCall_Init;
            player->actor.update = PlayerCall_Update;
            player->actor.draw = PlayerCall_Draw;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;

            TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
            R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
            Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);

            // The last thing that he says after teaching you the song.
            enOsn->textId = 0x1FCD;
            enOsn->stateFlags |= OSN_STATE_END_CONVERSATION;
            Message_StartTextbox(gPlayState, enOsn->textId, &enOsn->actor);
            enOsn->actionFunc = EnOsn_Talk;

            // Wait till the conversation ends, then end the interaction (for some reason without this step you get soft
            // locked)
            static int hookId = 0;
            GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(hookId);
            hookId = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorUpdate>(
                (uintptr_t)enOsn, [](Actor* actor) {
                    EnOsn* enOsn = (EnOsn*)actor;
                    if (enOsn->actionFunc == EnOsn_Idle) {
                        Player_SetCsActionWithHaltedActors(gPlayState, &enOsn->actor, PLAYER_CSACTION_END);

                        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(hookId);
                    }
                });

            if (GameInteractor_Should(GI_VB_GIVE_ITEM_FROM_OSN, true, enOsn)) {
                // Queue up the item gives
                GameInteractor::Instance->events.emplace_back(
                    GIEventGiveItem{ .showGetItemCutscene = true,
                                     .getItemText = "the Song of Healing",
                                     .drawItem = []() { Rando::DrawItem(RI_SONG_OF_HEALING); },
                                     .giveItem = []() { Item_Give(gPlayState, ITEM_SONG_HEALING); } });
                GameInteractor::Instance->events.emplace_back(
                    GIEventGiveItem{ .showGetItemCutscene = true,
                                     .getItemText = "the Deku Mask",
                                     .drawItem = []() { GetItem_Draw(gPlayState, GID_MASK_DEKU); },
                                     .giveItem = []() { Item_Give(gPlayState, ITEM_MASK_DEKU); } });
            }
        }
    });
}
