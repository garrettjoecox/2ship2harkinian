#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Bal/z_en_bal.h"
void EnBal_SetupTalk(EnBal* enBal);
}

RandoItemId randoItemId1;
RandoItemId randoItemId2;
std::string item1;
std::string item2;
uint8_t choiceIndex;

std::map<int16_t, std::vector<RandoCheckId>> tingleMap = {
    { SCENE_BACKTOWN, { RC_CLOCK_TOWN_NORTH_TINGLE_MAP_1, RC_CLOCK_TOWN_NORTH_TINGLE_MAP_2 } },
    { SCENE_24KEMONOMITI, { RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_1, RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_2 } },
    { SCENE_17SETUGEN, { RC_TWIN_ISLANDS_TINGLE_MAP_1, RC_TWIN_ISLANDS_TINGLE_MAP_2 } },
    { SCENE_ROMANYMAE, { RC_MILK_ROAD_TINGLE_MAP_1, RC_MILK_ROAD_TINGLE_MAP_2 } },
    { SCENE_30GYOSON, { RC_GREAT_BAY_COAST_TINGLE_MAP_1, RC_GREAT_BAY_COAST_TINGLE_MAP_2 } },
    { SCENE_IKANA, { RC_IKANA_CANYON_TINGLE_MAP_1, RC_IKANA_CANYON_TINGLE_MAP_2 } }
};

void CreateTingleDialogueMsg(CustomMessage::Entry entry) {
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 5 Rupees\x11"
                "\x02{item2}\x01 40 Rupees\x11"
                "\x02No thanks";

    CustomMessage::Replace(&entry.msg, "{item1}", item1);
    CustomMessage::Replace(&entry.msg, "{item2}", item2);
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
};

void Identify_TingleWares() {
    randoItemId1 = RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][0]].randoItemId;
    randoItemId2 = RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][1]].randoItemId;
    item1 = Rando::StaticData::Items[randoItemId1].name;
    item2 = Rando::StaticData::Items[randoItemId2].name;
}

void Rando::ActorBehavior::InitEnBalBehavior() {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE);
    }

    COND_VB_SHOULD(VB_ALREADY_HAVE_TINGLE_MAP, IS_RANDO, {
        EnBal* refActor = va_arg(args, EnBal*);
        choiceIndex = gPlayState->msgCtx.choiceIndex;
        if (RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].obtained) {
            *should = true;
        } else {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_TINGLE_GIVE_MAP_UNLOCK, IS_RANDO, {
        EnBal* refActor = va_arg(args, EnBal*);
        Message_StartTextbox(gPlayState, 0x1D17, &refActor->picto.actor);
        refActor->textId = 0x1D17;
        EnBal_SetupTalk(refActor);
        RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][choiceIndex]].eligible = true;
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D11, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D12, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D13, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D14, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D15, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D16, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Identify_TingleWares();
        CreateTingleDialogueMsg(entry);
        *loadFromMessageTable = false;
    });
}