#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Kujiya/z_en_kujiya.h"
void EnKujiya_Wait(EnKujiya* enKujiya, PlayState* play);
}

std::string IdentifyItemName() {
    std::string itemName = Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].randoItemId].article;
    if (itemName != "") {
        itemName += " ";
    }
    itemName += Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].randoItemId].name;
    itemName += '\x00';

    return itemName;
}

void Rando::ActorBehavior::InitEnKujiyaBehavior() {
    COND_VB_SHOULD(VB_GIVE_LOTTERY_WINNINGS, IS_RANDO, {
        EnKujiya* refActor = va_arg(args, EnKujiya*);

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_LOTTERY].eligible = true;
        refActor->actionFunc = EnKujiya_Wait;
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x2b5c, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "50 Rupees", IdentifyItemName());
        entry.msg.replace(entry.msg.end() - 38, entry.msg.end() - 3, "");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x2b66, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        std::string itemString = "\x11";
        itemString += IdentifyItemName();

        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "50 Rupees", itemString);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}