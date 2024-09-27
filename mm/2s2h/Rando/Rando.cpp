#include "Rando.h"
#include "Enhancements/GameInteractor/GameInteractor.h"
#include <libultraship/libultraship.h>
#include <random>
#include "Rando/ActorBehavior/ActorBehavior.h"
#include <boost_custom/container_hash/hash_32.hpp>

extern "C" {
#include "ShipUtils.h"
#include "variables.h"
#include "functions.h"
}

static bool queued = false;
void RandomizerQueueChecks(Actor* actor) {
    if (queued) {
        return;
    }

    for (auto& randoSaveCheck : RANDO_SAVE_CHECKS) {
        if (randoSaveCheck.eligible && !randoSaveCheck.obtained) {
            queued = true;

            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .getItemText = Rando::StaticData::Items[randoItemId].name,
                .drawItem = [randoItemId]() { Rando::DrawItem(randoItemId); },
                .giveItem =
                    [&randoSaveCheck, randoItemId]() {
                        Rando::GiveItem(randoItemId);
                        randoSaveCheck.obtained = true;
                        queued = false;
                    } });
            return;
        }
    }
}

void RandomizerOnFlagSetHandler(FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    randoSaveCheck.eligible = true;
}

void RandomizerOnSceneFlagSetHandler(s16 sceneId, FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag, sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    randoSaveCheck.eligible = true;
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case GI_VB_GIVE_ITEM_FROM_GURUGURU:
        case GI_VB_GIVE_ITEM_FROM_SCRIPT:
        case GI_VB_GIVE_ITEM_FROM_SWORDSMAN:
            *should = false;
            break;
    }
}

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void OnSaveInitHandler(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;
        memset(&gSaveContext.save.shipSaveInfo.rando, 0, sizeof(gSaveContext.save.shipSaveInfo.rando));

        std::string inputSeed = CVarGetString("gRando.InputSeed", "");

        if (!inputSeed.empty()) {
            uint32_t seedHash = boost::hash_32<std::string>{}(inputSeed);
            Ship_Random_Seed(seedHash);
        }

        std::vector<RandoItemId> itemPool;
        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                itemPool.push_back(randoStaticCheck.randoItemId);
            }
        }

        for (size_t i = 0; i < itemPool.size(); i++) {
            std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size())]);
        }

        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
                itemPool.pop_back();
            }
        }
    }
}

// When a save is loaded, we want to unregister all hooks and re-register them if it's a rando save
void OnSaveLoadHandler(s16 fileNum) {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;
    static uint32_t onPlayerUpdateHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::ShouldVanillaBehavior>(onVanillaBehaviorHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHook);

    onFlagSetHook = 0;
    onSceneFlagSetHook = 0;
    onVanillaBehaviorHook = 0;
    onPlayerUpdateHook = 0;

    Rando::InitActorBehavior(gSaveContext.save.shipSaveInfo.saveType == SAVETYPE_RANDO);

    if (gSaveContext.save.shipSaveInfo.saveType != SAVETYPE_RANDO) {
        return;
    }

    onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
    onSceneFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
    onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldVanillaBehavior>(
        RandomizerOnVanillaBehaviorHandler);
    onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_PLAYER, RandomizerQueueChecks);
}

// Entry point for the rando module
void Rando::Init() {
    queued = false;
    GameInteractor::Instance->currentEvent = GIEventNone{};
    GameInteractor::Instance->events.clear();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(OnSaveInitHandler);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(OnSaveLoadHandler);
}
