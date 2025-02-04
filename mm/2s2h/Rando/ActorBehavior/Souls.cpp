#include "Souls.h"
#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

#include "2s2h/Rando/DrawFuncs.h"

extern "C" {
#include "variables.h"
}

std::unordered_map<RandoItemId, std::tuple<std::function<void()>, std::vector<ActorId>, RandoInf>> soulMap = {
    { RI_SOUL_ALIEN, { DrawAlien, { ACTOR_EN_INVADEPOH }, RANDO_INF_OBTAINED_SOUL_OF_ALIENS } },
    { RI_SOUL_ARMOS, { DrawArmos, { ACTOR_EN_AM }, RANDO_INF_OBTAINED_SOUL_OF_ARMOS } },
    { RI_SOUL_BAT, { DrawBat, { ACTOR_EN_BAT }, RANDO_INF_OBTAINED_SOUL_OF_BATS } },
    { RI_SOUL_BEAMOS, { DrawBeamos, { ACTOR_EN_VM }, RANDO_INF_OBTAINED_SOUL_OF_BEAMOS } },
    { RI_SOUL_BOE, { DrawBoe, { ACTOR_EN_MKK }, RANDO_INF_OBTAINED_SOUL_OF_BOES } },
    { RI_SOUL_BOMBCHU, { DrawRealBombchu, { ACTOR_EN_RAT }, RANDO_INF_OBTAINED_SOUL_OF_BOMBCHU } },
    { RI_SOUL_DEATH_ARMOS, { DrawDeathArmos, { ACTOR_EN_FAMOS }, RANDO_INF_OBTAINED_SOUL_OF_DEATH_ARMOS } },
    { RI_SOUL_DEKU_BABA,
      { DrawDekuBaba,
        { ACTOR_EN_DEKUBABA, ACTOR_EN_KAREBABA, ACTOR_BOSS_05 },
        RANDO_INF_OBTAINED_SOUL_OF_DEKU_BABAS } },
    { RI_SOUL_DINOLFOS, { DrawDinolfos, { ACTOR_EN_DINOFOS }, RANDO_INF_OBTAINED_SOUL_OF_DINOLFOS } },
    { RI_SOUL_DODONGO, { DrawDodongo, { ACTOR_EN_DODONGO }, RANDO_INF_OBTAINED_SOUL_OF_DODONGOS } },
    { RI_SOUL_EENO, { DrawEeno, { ACTOR_EN_SNOWMAN }, RANDO_INF_OBTAINED_SOUL_OF_EENOS } },
    { RI_SOUL_GARO, { DrawGaroMaster, { ACTOR_EN_JSO2, ACTOR_EN_JSO }, RANDO_INF_OBTAINED_SOUL_OF_GARO_MASTERS } },
    { RI_SOUL_GRASSHOPPER, { DrawGrasshopper, { ACTOR_EN_GRASSHOPPER }, RANDO_INF_OBTAINED_SOUL_OF_GRASSHOPPERS } },
    { RI_SOUL_GUAY, { DrawGuay, { ACTOR_EN_CROW, ACTOR_EN_RUPPECROW }, RANDO_INF_OBTAINED_SOUL_OF_GUAYS } },
    { RI_SOUL_IRON_KNUCKLE, { DrawIronKnuckle, { ACTOR_EN_IK }, RANDO_INF_OBTAINED_SOUL_OF_IRON_KNUCKLES } },
    { RI_SOUL_KEESE, { DrawKeese, { ACTOR_EN_FIREFLY }, RANDO_INF_OBTAINED_SOUL_OF_KEESE } },
    { RI_SOUL_LEEVER, { DrawLeever, { ACTOR_EN_NEO_REEBA }, RANDO_INF_OBTAINED_SOUL_OF_LEEVERS } },
    { RI_SOUL_MAD_SCRUB, { DrawMadScrub, { ACTOR_EN_DEKUNUTS }, RANDO_INF_OBTAINED_SOUL_OF_MAD_SCRUBS } },
    { RI_SOUL_OCTOROK, { DrawOctorok, { ACTOR_EN_OKUTA }, RANDO_INF_OBTAINED_SOUL_OF_OCTOROKS } },
    { RI_SOUL_PEEHAT, { DrawPeehat, { ACTOR_EN_PEEHAT }, RANDO_INF_OBTAINED_SOUL_OF_PEEHATS } },
    { RI_SOUL_REDEAD, { DrawRedead, { ACTOR_EN_RD, ACTOR_EN_RAILGIBUD }, RANDO_INF_OBTAINED_SOUL_OF_REDEADS } },
    { RI_SOUL_SHELLBLADE, { DrawShellBlade, { ACTOR_EN_SB }, RANDO_INF_OBTAINED_SOUL_OF_SHELLBLADES } },
    { RI_SOUL_SKULLFISH,
      { DrawSkullfish, { ACTOR_EN_PR, ACTOR_EN_PRZ, ACTOR_EN_PR2 }, RANDO_INF_OBTAINED_SOUL_OF_SKULLFISH } },
    { RI_SOUL_SKULLTULA, { DrawSkulltula, { ACTOR_EN_ST, ACTOR_EN_SW }, RANDO_INF_OBTAINED_SOUL_OF_SKULLTULAS } },
    { RI_SOUL_SLIME, { DrawSlime, { ACTOR_EN_SLIME }, RANDO_INF_OBTAINED_SOUL_OF_SLIMES } },
    { RI_SOUL_SNAPPER, { DrawSnapper, { ACTOR_EN_KAME }, RANDO_INF_OBTAINED_SOUL_OF_SNAPPERS } },
    { RI_SOUL_STALCHILD,
      { DrawStalchild, { ACTOR_EN_SKB, ACTOR_EN_RAIL_SKB }, RANDO_INF_OBTAINED_SOUL_OF_STALCHILDREN } },
    { RI_SOUL_TEKTITE, { DrawTektite, { ACTOR_EN_TITE }, RANDO_INF_OBTAINED_SOUL_OF_TEKTITES } },
    { RI_SOUL_WALLMASTER,
      { DrawWallmaster, { ACTOR_EN_WALLMAS, ACTOR_EN_FLOORMAS }, RANDO_INF_OBTAINED_SOUL_OF_WALLMASTERS } },
    { RI_SOUL_WOLFOS, { DrawWolfos, { ACTOR_EN_WF }, RANDO_INF_OBTAINED_SOUL_OF_WOLFOS } },
};

bool FindSoul(int16_t actorId) {
    bool isFound = false;

    for (auto& soul : soulMap) {
        for (auto& actor : std::get<1>(soul.second)) {
            if (actor == actorId) {
                if (gSaveContext.save.shipSaveInfo.rando.enemySouls[std::get<2>(soul.second)] == 1) {
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
            gSaveContext.save.shipSaveInfo.rando.enemySouls[std::get<2>(it->second)] = 1;
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
        if (actor->category != ACTORCAT_ENEMY) {
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
        if (actor->category != ACTORCAT_ENEMY) {
            return;
        }

        if (!FindSoul(actor->id)) {
            *should = false;
        }
    });
}