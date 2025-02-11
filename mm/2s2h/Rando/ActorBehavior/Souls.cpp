#include "Souls.h"
#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

#include "2s2h/Rando/DrawFuncs.h"

extern "C" {
#include "variables.h"
}

std::unordered_map<RandoItemId, std::tuple<std::function<void()>, std::vector<ActorId>, RandoInf>> soulMap = {
    { RI_SOUL_GOHT, { DrawGoht, { ACTOR_BOSS_HAKUGIN }, RANDO_INF_OBTAINED_SOUL_OF_GOHT } },
    { RI_SOUL_GYORG, { DrawGyorg, { ACTOR_BOSS_03 }, RANDO_INF_OBTAINED_SOUL_OF_GYORG } },
    { RI_SOUL_MAJORA, { DrawMajora, { ACTOR_BOSS_07 }, RANDO_INF_OBTAINED_SOUL_OF_MAJORA } },
    { RI_SOUL_ODOLWA, { DrawOdolwa, { ACTOR_BOSS_01 }, RANDO_INF_OBTAINED_SOUL_OF_ODOLWA } },
    { RI_SOUL_TWINMOLD, { DrawTwinmold, { ACTOR_BOSS_02 }, RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD } },
};

bool FindSoul(int16_t actorId) {
    bool isFound = false;

    for (auto& soul : soulMap) {
        for (auto& actor : std::get<1>(soul.second)) {
            if (actor == actorId) {
                if (gSaveContext.save.shipSaveInfo.rando
                        .enemySouls[(std::get<2>(soul.second) - RANDO_INF_OBTAINED_SOUL_OF_GOHT)] == 1) {
                    isFound = true;
                }
            }
        }
    }

    return isFound;
}

extern void SoulObtained(RandoItemId randoItemId) {
    auto it = soulMap.find(randoItemId);
    if (it != soulMap.end()) {
        for (auto& actorList : std::get<1>(it->second)) {
            gSaveContext.save.shipSaveInfo.rando
                .enemySouls[(std::get<2>(it->second) - RANDO_INF_OBTAINED_SOUL_OF_GOHT)] = 1;
        }
    }
}

extern void DrawSoul(RandoItemId randoItemId) {
    auto it = soulMap.find(randoItemId);
    if (it != soulMap.end()) {
        std::get<0>(it->second)();
    }
}

void Rando::ActorBehavior::InitSoulsBehavior() {
    COND_HOOK(ShouldActorDraw, IS_RANDO, [](Actor* actor, bool* should) {
        if (actor->category != ACTORCAT_BOSS) {
            return;
        }

        if (!FindSoul(actor->id)) {
            actor->flags &= ~ACTOR_FLAG_TARGETABLE;
            *should = false;
        } else {
            actor->flags |= ACTOR_FLAG_TARGETABLE;
        }
    });
    COND_HOOK(ShouldActorUpdate, IS_RANDO, [](Actor* actor, bool* should) {
        if (actor->category != ACTORCAT_BOSS) {
            return;
        }

        if (!FindSoul(actor->id)) {
            *should = false;
        }
    });
}