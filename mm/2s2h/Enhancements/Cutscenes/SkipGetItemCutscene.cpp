#include <utility>
#include <unordered_map>

#include "libultraship/libultraship.h"
#include "2s2h/Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
extern PlayState* gPlayState;
extern GetItemEntry sGetItemTable[GI_MAX - 1];
s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
void Player_StopCutscene(Player* player);
void func_80848294(PlayState* play, Player* player);
}

struct HashTuple {
    template <class T1, class T2, class T3> std::size_t operator()(const std::tuple<T1, T2, T3>& p) const {
        auto [first, second, third] = p;
        auto hash1 = std::hash<T1>{}(first);
        auto hash2 = std::hash<T2>{}(second);
        auto hash3 = std::hash<T3>{}(third);
        return hash1 ^ hash2 ^ hash3;
    }
};

std::unordered_map<std::tuple<FlagType, u32, u32>, GetItemId, HashTuple> GetItemLocations = {
    { { FLAG_WEEK_EVENT_REG, WEEKEVENTREG_RECEIVED_ROOM_KEY, 0 }, GI_ROOM_KEY },
    { { FLAG_CYCL_SCENE_CHEST, 0x00, SCENE_CLOCKTOWER }, GI_RUPEE_RED },
    { { FLAG_CYCL_SCENE_COLLECTIBLE, 0x0A, SCENE_CLOCKTOWER }, GI_HEART_PIECE },
    { { FLAG_CYCL_SCENE_CHEST, 0x0A, SCENE_TOWN }, GI_RUPEE_SILVER },
    { { FLAG_WEEK_EVENT_REG, WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP, 0 }, GI_HEART_PIECE },
    { { FLAG_WEEK_EVENT_REG, WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP, 0 }, GI_HEART_PIECE },
};

void EnItem00_DrawCustomForFreestanding(Actor* thisx, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    GetItem_Draw(play, enItem00->shipDrawId);
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

void RegisterSkipGetItemCutscene() {
    REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_SCRIPT, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscene", 0)) {
            // Auto queue without location list
            // GetItemId* getItemId = static_cast<GetItemId*>(opt);
            // GameInteractor_ItemGiveQueue_Push(*getItemId);
            *should = false;
        }
    });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(GI_VB_GIVE_ITEM_FROM_ITEM00, [](GIVanillaBehavior _, bool* should, void* opt) {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscene", 0)) {
            EnItem00* item00 = static_cast<EnItem00*>(opt);

            // If it's one of our items ignore it
            if (item00->actor.params == ITEM00_NOTHING || item00->actor.params == (ITEM00_NOTHING | 0x8000)) {
                return;
            }

            Flags_SetCollectible(gPlayState, item00->collectibleFlag);
            Actor_Kill(&item00->actor);
            *should = false;
        }
    });

    REGISTER_VB_SHOULD(GI_VB_GIVE_ITEM_FROM_CHEST, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscene", 0)) {
            // EnBox* enBox = static_cast<EnBox*>(opt);
            Player* player = GET_PLAYER(gPlayState);
            func_80832558(gPlayState, player, func_80837C78_override);
            // Auto queue without location list
            // GameInteractor_ItemGiveQueue_Push((GetItemId)enBox->getItemId);
            *should = false;
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([] (FlagType flagType, u32 flag) {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscene", 0)) {
            auto it = GetItemLocations.find({ flagType, flag, 0 });
            if (it != GetItemLocations.end()) {
                GameInteractor_ItemGiveQueue_Push(it->second, false);
            }
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([] (s16 sceneId, FlagType flagType, u32 flag) {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscene", 0)) {
            auto it = GetItemLocations.find({ flagType, flag, sceneId });
            if (it != GetItemLocations.end()) {
                GameInteractor_ItemGiveQueue_Push(it->second, false);
            }
        }
    });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_ITEM00, [](Actor* actor) {
        EnItem00* enItem00 = (EnItem00*)actor;

        // If it's one of our items ignore it
        if ((actor->params == (0x8000 | ITEM00_NOTHING) || actor->params == ITEM00_NOTHING)) {
            return;
        }

        auto it = GetItemLocations.find({ FLAG_CYCL_SCENE_COLLECTIBLE, enItem00->collectibleFlag, gPlayState->sceneId });
        if (it != GetItemLocations.end()) {
            enItem00->shipDrawId = ABS(sGetItemTable[it->second - 1].gid) - 1;
            actor->draw = EnItem00_DrawCustomForFreestanding;
        }
    });
}
