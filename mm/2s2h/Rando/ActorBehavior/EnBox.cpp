#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
void Player_StopCutscene(Player* player);
void func_80848294(PlayState* play, Player* player);
}

void Player_Action_65_override(Player* player, PlayState* play) {
    if (PlayerAnimation_Update(play, &player->skelAnime)) {
        Player_StopCutscene(player);
        func_80848294(play, player);
    }
}

void func_80837C78_override(PlayState* play, Player* player) {
    Player_SetAction_PreserveMoveFlags(play, player, Player_Action_65_override, 0);
    player->stateFlags1 |= (PLAYER_STATE1_400 | PLAYER_STATE1_20000000);
}

// This simply prevents the player from getting an item from the chest, but still
// plays the chest opening animation and ensure the treasure chest flag is set
void Rando::ActorBehavior::InitEnBoxBehavior() {
    static uint32_t shouldHook1Id = 0;
    static uint32_t shouldHook2Id = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook1Id);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHook2Id);

    shouldHook1Id = 0;
    shouldHook2Id = 0;

    if (!IS_RANDO) {
        return;
    }

    shouldHook1Id = REGISTER_VB_SHOULD(VB_GIVE_ITEM_FROM_CHEST, {
        EnBox* enBox = va_arg(args, EnBox*);
        Player* player = GET_PLAYER(gPlayState);
        func_80832558(gPlayState, player, func_80837C78_override);
        *should = false;
    });
    shouldHook2Id = REGISTER_VB_SHOULD(VB_CHEST_SPAWN_FAIRY, { *should = false; });
}
