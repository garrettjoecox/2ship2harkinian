#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Bal/z_en_bal.h"
void EnBal_SetupTalk(EnBal* enBal);
}

std::map<int16_t, std::vector<RandoCheckId>> tingleMap = {
    { SCENE_BACKTOWN, { RC_CLOCK_TOWN_NORTH_TINGLE_MAP_1, RC_CLOCK_TOWN_NORTH_TINGLE_MAP_2 } },
    { SCENE_24KEMONOMITI, { RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_1, RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_2 } },
    { SCENE_17SETUGEN, { RC_TWIN_ISLANDS_TINGLE_MAP_1, RC_TWIN_ISLANDS_TINGLE_MAP_2 } },
    { SCENE_ROMANYMAE, { RC_MILK_ROAD_TINGLE_MAP_1, RC_MILK_ROAD_TINGLE_MAP_2 } },
    { SCENE_30GYOSON, { RC_GREAT_BAY_COAST_TINGLE_MAP_1, RC_GREAT_BAY_COAST_TINGLE_MAP_2 } },
    { SCENE_IKANA, { RC_IKANA_CANYON_TINGLE_MAP_1, RC_IKANA_CANYON_TINGLE_MAP_2 } }
};

void OnOpenText(u16* textId, bool* loadFromMessageTable) {
    RandoItemId randoItemId1 = RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][0]].randoItemId;
    RandoItemId randoItemId2 = RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][1]].randoItemId;

    randoItemId1 = Rando::ConvertItem(randoItemId1, tingleMap[gPlayState->sceneId][0]);
    randoItemId2 = Rando::ConvertItem(randoItemId2, tingleMap[gPlayState->sceneId][1]);

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 20 Rupees\x11"
                "\x02{item2}\x01 40 Rupees\x11"
                "\x02No thanks";

    CustomMessage::Replace(&entry.msg, "{item1}", Rando::StaticData::Items[randoItemId1].name);
    CustomMessage::Replace(&entry.msg, "{item2}", Rando::StaticData::Items[randoItemId2].name);
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void Rando::ActorBehavior::InitEnBalBehavior() {
    COND_VB_SHOULD(VB_ALREADY_HAVE_TINGLE_MAP, IS_RANDO, {
        EnBal* enBal = va_arg(args, EnBal*);
        s32* price = va_arg(args, s32*);
        // TODO: Allow randomization of price
        if (gPlayState->msgCtx.choiceIndex == 0) {
            *price = 20;
        } else {
            *price = 40;
        }

        if (RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].obtained) {
            *should = true;
        } else {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_TINGLE_GIVE_MAP_UNLOCK, IS_RANDO, {
        EnBal* enBal = va_arg(args, EnBal*);
        RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].eligible = true;
        Message_StartTextbox(gPlayState, 0x1D17, &enBal->picto.actor);
        enBal->textId = 0x1D17;
        EnBal_SetupTalk(enBal);
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D11, IS_RANDO, OnOpenText);
    COND_ID_HOOK(OnOpenText, 0x1D12, IS_RANDO, OnOpenText);
    COND_ID_HOOK(OnOpenText, 0x1D13, IS_RANDO, OnOpenText);
    COND_ID_HOOK(OnOpenText, 0x1D14, IS_RANDO, OnOpenText);
    COND_ID_HOOK(OnOpenText, 0x1D15, IS_RANDO, OnOpenText);
    COND_ID_HOOK(OnOpenText, 0x1D16, IS_RANDO, OnOpenText);

    COND_VB_SHOULD(VB_HAVE_MAGIC_FOR_TINGLE, IS_RANDO, { *should = true; });
}
