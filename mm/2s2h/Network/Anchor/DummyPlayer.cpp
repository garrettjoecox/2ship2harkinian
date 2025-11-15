#include "Anchor.h"
#include "2s2h/NameTag/NameTag.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "z64malloc.h"
extern PlayState* gPlayState;

// void Player_UseItem(PlayState* play, Player* player, s32 item);
// void Player_Draw(Actor* actor, PlayState* play);

#include "objects/object_test3/object_test3.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];
extern TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX];
extern EffectBlureInit2 D_8085D30C;
extern EffectTireMarkInit D_8085D330;
void Player_DrawGameplay(PlayState* play, Player* player, s32 lod, Gfx* cullDList,
                         OverrideLimbDrawFlex overrideLimbDraw);
void Player_Anim_PlayOnceMorph(PlayState* play, Player* player, PlayerAnimationHeader* anim);
PlayerAnimationHeader* Player_GetIdleAnim(Player* player);
}

// Hijacking player->zTargetActiveTimer (unused s32 for the dummy) to store the clientId for convenience
#define DUMMY_CLIENT_ID player->zTargetActiveTimer

// static DamageTable DummyPlayerDamageTable = {
//     /* Deku nut      */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
//     /* Deku stick    */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Slingshot     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Explosive     */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Boomerang     */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
//     /* Normal arrow  */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Hammer swing  */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL),
//     /* Hookshot      */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
//     /* Kokiri sword  */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Master sword  */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Giant's Knife */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Fire arrow    */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_FIRE),
//     /* Ice arrow     */ DMG_ENTRY(4, PLAYER_HIT_RESPONSE_ICE_TRAP),
//     /* Light arrow   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
//     /* Unk arrow 1   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
//     /* Unk arrow 2   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
//     /* Unk arrow 3   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
//     /* Fire magic    */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_FIRE),
//     /* Ice magic     */ DMG_ENTRY(3, PLAYER_HIT_RESPONSE_ICE_TRAP),
//     /* Light magic   */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
//     /* Shield        */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
//     /* Mirror Ray    */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
//     /* Kokiri spin   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Giant spin    */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Master spin   */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Kokiri jump   */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Giant jump    */ DMG_ENTRY(8, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Master jump   */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
//     /* Unknown 1     */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
//     /* Unblockable   */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
//     /* Hammer jump   */ DMG_ENTRY(4, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
//     /* Unknown 2     */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
// };

void DummyPlayer_Init(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    DUMMY_CLIENT_ID = clientId;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    // Hack to account for usage of gSaveContext in Player_Init
    // s32 originalAge = gSaveContext.linkAge;
    // gSaveContext.linkAge = client.linkAge;

    // #region modeled after EnTorch2_Init and Player_Init
    // actor->room = -1;
    // player->itemAction = player->heldItemAction = -1;
    // player->heldItemId = ITEM_NONE;
    // Player_UseItem(play, player, ITEM_NONE);
    // Player_SetModelGroup(player, Player_ActionToModelGroup(player, player->heldItemAction));
    // play->playerInit(player, play, gPlayerSkelHeaders[client.linkAge]);

    // play->func_11D54(player, play);
    // #endregion

    // player->cylinder.base.acFlags = AC_ON | AC_TYPE_PLAYER;
    // player->cylinder.base.ocFlags2 = OC2_TYPE_1;
    // player->cylinder.info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_HITMARK;
    // player->actor.flags |= ACTOR_FLAG_DRAGGED_BY_HOOKSHOT;
    // player->cylinder.dim.radius = 30;
    // player->actor.colChkInfo.damageTable = &DummyPlayerDamageTable;

    // gSaveContext.linkAge = originalAge;

    // Primarily modeled after EnTest3_Init and Player_Init
    player->csId = CS_ID_NONE;
    player->transformation = client.transformation;
    player->ageProperties = &sPlayerAgeProperties[player->transformation];
    player->heldItemAction = PLAYER_IA_NONE;
    player->heldItemId = ITEM_OCARINA_OF_TIME;

    Player_SetModelGroup(player, PLAYER_MODELGROUP_DEFAULT);
    play->playerInit(player, play, gPlayerSkeletons[player->transformation]);

    // Prop Hunt
    // SkelAnime_InitFlex(play, &skelAnime, propSkeletons[propIndex].first, NULL, jointTable, morphTable,
    // propSkeletons[propIndex].second);

    player->maskObjectSegment = ZeldaArena_Malloc(0x3800);
    // play->func_18780(player, play);
    Player_Anim_PlayOnceMorph(play, player, Player_GetIdleAnim(player));
    player->yaw = player->actor.shape.rot.y;

    // Ensures the actor is always updating/drawing even when culled/out of distance
    actor->flags =
        ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_INSIDE_CULLING_VOLUME;
    // Will ensure the actor is always updating even when in a separate room than the player
    actor->room = -1;

    NameTag_RegisterForActorWithOptions(actor, client.name.c_str(), { .yOffset = 30 });
}

// Update the actor with new data from the client
void DummyPlayer_Update(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    if (client.sceneId != gPlayState->sceneId || !client.online || !client.isSaveLoaded) {
        actor->world.pos.x = -9999.0f;
        actor->world.pos.y = -9999.0f;
        actor->world.pos.z = -9999.0f;
        actor->shape.shadowAlpha = 0;
        return;
    }

    actor->shape.shadowAlpha = 255;
    Math_Vec3s_Copy(&actor->shape.rot, &client.posRot.rot);
    Math_Vec3f_Copy(&actor->world.pos, &client.posRot.pos);
    memcpy(&player->jointTableBuffer, &client.jointTable, 159);
    memcpy(&player->jointTableUpperBuffer, &client.upperJointTable, 159);
    player->maskObjectLoadState = 0;
    player->maskId = player->currentMask;
    player->currentMask = client.currentMask;
    player->rightHandType = client.rightHandType;
    player->leftHandType = client.leftHandType;
    player->currentShield = client.currentShield;
    player->sheathType = client.sheathType;
    player->heldItemAction = client.heldItemAction;
    player->heldItemId = client.heldItemId;
    player->itemAction = client.itemAction;
    player->stateFlags1 = client.stateFlags1;
    player->stateFlags2 = client.stateFlags2;
    player->stateFlags3 = client.stateFlags3;
    player->unk_B0C = client.unk_B0C;
    player->unk_B28 = client.unk_B28;
    player->unk_ACC = client.unk_ACC;
    Player_SetModels(player, Player_ActionToModelGroup(player, (PlayerItemAction)player->itemAction));

    // if (player->modelGroup != client.modelGroup) {
    //     // Hack to account for usage of gSaveContext
    //     s32 originalAge = gSaveContext.linkAge;
    //     gSaveContext.linkAge = client.linkAge;
    //     u8 originalButtonItem0 = gSaveContext.equips.buttonItems[0];
    //     gSaveContext.equips.buttonItems[0] = client.buttonItem0;
    //     Player_SetModelGroup(player, client.modelGroup);
    //     gSaveContext.linkAge = originalAge;
    //     gSaveContext.equips.buttonItems[0] = originalButtonItem0;
    // }

    // if (player->cylinder.base.acFlags & AC_HIT && player->invincibilityTimer == 0) {
    //     player->invincibilityTimer = 20;
    //     Anchor::Instance->SendPacket_DamagePlayer(client.clientId, player->actor.colChkInfo.damageEffect,
    //     player->actor.colChkInfo.damage);
    // }

    // if (player->stateFlags1 & PLAYER_STATE1_SHIELDING) {
    //     player->cylinder.dim.height = player->cylinder.dim.height * 0.8f;
    // }

    // Collider_UpdateCylinder(&player->actor, &player->cylinder);

    // if (!(player->stateFlags2 & PLAYER_STATE2_FROZEN)) {
    //     if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_HANGING_OFF_LEDGE |
    //     PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_ON_HORSE))) {
    //         CollisionCheck_SetOC(play, &play->colChkCtx, &player->cylinder.base);
    //     }

    //     if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_DAMAGED)) && (player->invincibilityTimer <=
    //     0)) {
    //         CollisionCheck_SetAC(play, &play->colChkCtx, &player->cylinder.base);

    //         if (player->invincibilityTimer < 0) {
    //             CollisionCheck_SetAT(play, &play->colChkCtx, &player->cylinder.base);
    //         }
    //     }
    // }

    // if (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE)) {
    //     player->actor.colChkInfo.mass = MASS_IMMOVABLE;
    // } else {
    //     player->actor.colChkInfo.mass = 50;
    // }

    // Collider_ResetCylinderAC(play, &player->cylinder.base);
}

void DummyPlayer_Draw(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    if (client.sceneId != gPlayState->sceneId || !client.online || !client.isSaveLoaded) {
        return;
    }

    Player_DrawGameplay(play, player, 1, gCullBackDList, Player_OverrideLimbDrawGameplayDefault);
}

void DummyPlayer_Destroy(Actor* actor, PlayState* play) {
}

// OOT STUFF ABOVE

// Primarily modeled after EnTest3_Init and Player_Init
void DummyInit(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (!Anchor::Instance->clients.contains(clientId)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[clientId];

    player->actor.room = -1;
    player->csId = CS_ID_NONE;
    player->transformation = client.transformation;
    player->ageProperties = &sPlayerAgeProperties[player->transformation];
    player->heldItemAction = PLAYER_IA_NONE;
    player->heldItemId = ITEM_OCARINA_OF_TIME;

    Player_SetModelGroup(player, PLAYER_MODELGROUP_DEFAULT);
    play->playerInit(player, play, gPlayerSkeletons[player->transformation]);

    // Prop Hunt
    // SkelAnime_InitFlex(play, &skelAnime, propSkeletons[propIndex].first, NULL, jointTable, morphTable,
    // propSkeletons[propIndex].second);

    Effect_Add(play, &player->meleeWeaponEffectIndex[0], EFFECT_BLURE2, 0, 0, &D_8085D30C);
    Effect_Add(play, &player->meleeWeaponEffectIndex[1], EFFECT_BLURE2, 0, 0, &D_8085D30C);
    Effect_Add(play, &player->meleeWeaponEffectIndex[2], EFFECT_TIRE_MARK, 0, 0, &D_8085D330);

    player->maskObjectSegment = ZeldaArena_Malloc(0x3800);
    play->func_18780(player, play);

    // Ensures the actor is always updating/drawing even when culled/out of distance
    actor->flags =
        ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_INSIDE_CULLING_VOLUME;
    // Will ensure the actor is always updating even when in a separate room than the player
    actor->room = -1;

    NameTag_RegisterForActorWithOptions(actor, client.name.c_str(), { .yOffset = 30 });
}

// Mostly used to update the actor with new data from the client
void DummyUpdate(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (Anchor::Instance->clients.contains(clientId)) {
        if (Anchor::Instance->clients[clientId].sceneId == gPlayState->sceneId) {
            AnchorClient& client = Anchor::Instance->clients[clientId];
            Math_Vec3s_Copy(&actor->shape.rot, &client.posRot.rot);
            Math_Vec3f_Copy(&actor->world.pos, &client.posRot.pos);
            memcpy(&player->jointTableBuffer, &client.jointTable, 159);
            memcpy(&player->jointTableUpperBuffer, &client.upperJointTable, 159);

            player->maskObjectLoadState = 0;
            player->maskId = player->currentMask;
            player->currentMask = client.currentMask;
            player->rightHandType = client.rightHandType;
            player->leftHandType = client.leftHandType;
            player->currentShield = client.currentShield;
            player->sheathType = client.sheathType;
            player->heldItemAction = client.heldItemAction;
            player->heldItemId = client.heldItemId;
            player->itemAction = client.itemAction;
            player->stateFlags1 = client.stateFlags1;
            player->stateFlags2 = client.stateFlags2;
            player->stateFlags3 = client.stateFlags3;
            player->unk_B0C = client.unk_B0C;
            player->unk_B28 = client.unk_B28;
            player->unk_ACC = client.unk_ACC;
            Player_SetModels(player, Player_ActionToModelGroup(player, (PlayerItemAction)player->itemAction));

            // Prop Hunt
            // static int lastAnimIndex = -1;
            // if (client.linearVelocity > 5.5f) { // Running
            //     if (lastAnimIndex != 2) {
            //         SubS_ChangeAnimationByInfoS(&skelAnime, propAnimationInfo, propIndex * 3 + 2);
            //         lastAnimIndex = 2;
            //     }
            // } else if (client.linearVelocity > 0.0f) { // Walking
            //     if (lastAnimIndex != 1) {
            //         SubS_ChangeAnimationByInfoS(&skelAnime, propAnimationInfo, propIndex * 3 + 1);
            //         lastAnimIndex = 1;
            //     }
            // } else { // Stopped
            //     if (lastAnimIndex != 0) {
            //         SubS_ChangeAnimationByInfoS(&skelAnime, propAnimationInfo, propIndex * 3);
            //         lastAnimIndex = 0;
            //     }
            // }

            // SkelAnime_Update(&skelAnime);
        } else {
            actor->world.pos.x = -9999.0f;
            actor->world.pos.y = -9999.0f;
            actor->world.pos.z = -9999.0f;
        }
    } else {
        Actor_Kill(actor);
    }
}

void DummyDraw(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    if (Anchor::Instance->clients.contains(clientId) &&
        Anchor::Instance->clients[clientId].sceneId == gPlayState->sceneId) {
        AnchorClient& client = Anchor::Instance->clients[clientId];
        Player_DrawGameplay(play, player, 1, gCullBackDList, Player_OverrideLimbDrawGameplayDefault);

        // Prop Hunt
        // OPEN_DISPS(play->state.gfxCtx);
        // if (propIndex == 0) {
        //     gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 200, 0);
        // } else if (propIndex == 1) {
        //     gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)object_mm_Tex_002950);
        // }
        // SkelAnime_DrawFlexOpa(play, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount,
        //                   NULL, NULL, actor);
        // CLOSE_DISPS(play->state.gfxCtx);
    }
}

// No-op to replace the ACTOR_ITEM_INBOX destroy
void DummyDestroy(Actor* actor, PlayState* play) {
}
