#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Kgy/z_en_kgy.h"
}

void Rando::ActorBehavior::InitEnKgyBehavior() {
    COND_VB_SHOULD(VB_SMITHY_START_UPGRADING_SWORD, IS_RANDO, {
        EnKgy* refActor = va_arg(args, EnKgy*);
        RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];

        refActor->actor.textId = 0xc3d;

        if (randoRazorSwordSaveCheck.obtained) {
            randoGildedSwordSaveCheck.eligible = true;
        } else {
            randoRazorSwordSaveCheck.eligible = true;
        }

        *should = false;
    });

    COND_VB_SHOULD(VB_SMITHY_CHECK_FOR_RAZOR_SWORD, IS_RANDO, {
        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];
        *should = randoRazorSwordSaveCheck.obtained;
    });

    COND_VB_SHOULD(VB_SMITHY_CHECK_FOR_GILDED_SWORD, IS_RANDO, {
        RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
        *should = randoGildedSwordSaveCheck.obtained;
    });

    COND_ID_HOOK(OnOpenText, 0xc3a, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, "your sword", "you");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc3b, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];
        if (!randoRazorSwordSaveCheck.obtained) {
            entry.autoFormat = false;
            std::string itemName;
            if (!Ship_IsCStringEmpty(Rando::StaticData::Items[randoRazorSwordSaveCheck.randoItemId].article)) {
                itemName += Rando::StaticData::Items[randoRazorSwordSaveCheck.randoItemId].article;
                itemName += " ";
            }
            itemName += Rando::StaticData::Items[randoRazorSwordSaveCheck.randoItemId].name;

            std::string finalMsg = "If you want \x04{itemName}";
            finalMsg += '\x00';
            finalMsg += ",\x11it will cost you\x01 100 Rupees";
            finalMsg += '\x00';
            finalMsg += ".";
            CustomMessage::Replace(&finalMsg, "{itemName}", itemName);
            entry.msg.replace(entry.msg.begin(), entry.msg.begin() + 289, finalMsg);
            entry.msg.replace(entry.msg.end() - 80, entry.msg.end() - 25, "So, do we have a deal");
        } else {
            RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
            std::string itemName = Rando::StaticData::Items[randoGildedSwordSaveCheck.randoItemId].name;
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc3d, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        std::string itemName = "40 Rupees";

        if (!RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD].eligible) {
            RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
            if (!Ship_IsCStringEmpty(Rando::StaticData::Items[randoGildedSwordSaveCheck.randoItemId].article)) {
                itemName = Rando::StaticData::Items[randoGildedSwordSaveCheck.randoItemId].article;
                itemName += " ";
            } else {
                itemName = "";
            }
            itemName += Rando::StaticData::Items[randoGildedSwordSaveCheck.randoItemId].name;
        }
        std::string finalMsg = ", I can offer you\x11\x04{itemName}";
        finalMsg += '\x00';
        finalMsg += "!\xE0\xBF";
        CustomMessage::Replace(&finalMsg, "{itemName}", itemName);
        entry.msg.replace(entry.msg.begin() + 78, entry.msg.end() - 1, finalMsg);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc3e, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg.replace(entry.msg.begin(), entry.msg.end() - 20, "Back for more");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc42, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg.replace(entry.msg.begin(), entry.msg.end() - 2, "Thanks for your business.");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc45, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;

        std::string finalMsg = "Did you bring the \x01gold dust";
        finalMsg += '\x00';
        finalMsg += "?";
        entry.msg.replace(entry.msg.begin(), entry.msg.end() - 2, finalMsg);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc46, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        entry.msg.replace(entry.msg.begin() + 61, entry.msg.begin() + 138, "");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0xc4c, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        entry.msg.replace(entry.msg.begin() + 24, entry.msg.end() - 2, "I'm not made of Randomizer Checks!");

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
