#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

std::map<std::pair<float, float>, RandoCheckId> freestandingMap = {
    { { 439.640961f, 1368.999390f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_12 },
    { { 439.640961f, 1329.000610f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_13 },
    { { 405.0f, 1309.0f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_14 },
    { { 370.359039f, 1329.000610f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_15 },
    { { 370.359039f, 1368.999390f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_16 },
    { { 405.0, 1389.0 }, RC_DEKU_PALACE_FREESTANDING_RUPEE_17 },
    { { 405.0, 1349.0 }, RC_DEKU_PALACE_FREESTANDING_RUPEE_18 },
};

void Rando::ActorBehavior::InitEnItem00Behavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor, bool* should) {
        EnItem00* item00 = (EnItem00*)actor;

        // If it's one of our items ignore it
        if (item00->actor.params == ITEM00_NOTHING) {
            return;
        }

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                                    ENITEM00_GET_7F00(actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            auto it = freestandingMap.find({ item00->actor.home.pos.x, item00->actor.home.pos.z });
            if (it == freestandingMap.end()) {
                return;
            } else {
                randoStaticCheck = Rando::StaticData::Checks[it->second];
            }
        }

        // Pots handle their own items, ignore them
        if (randoStaticCheck.randoCheckType == RCTYPE_POT) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled || Flags_GetCollectible(gPlayState, randoStaticCheck.flag)) {
            return;
        }

        // Prevent the original item from spawning
        *should = false;

        // If it hasn't been collected yet, spawn a dummy item
        CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0, CustomItem::KILL_ON_TOUCH,
            randoStaticCheck.randoCheckId,
            [](Actor* actor, PlayState* play) {
                auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
                switch (randoStaticCheck.flagType) {
                    case FLAG_NONE:
                        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
                            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].eligible = true;
                        }
                        break;
                    case FLAG_CYCL_SCENE_COLLECTIBLE:
                        Flags_SetCollectible(play, randoStaticCheck.flag);
                        break;
                    default:
                        break;
                }
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM));
            });
    });
}
