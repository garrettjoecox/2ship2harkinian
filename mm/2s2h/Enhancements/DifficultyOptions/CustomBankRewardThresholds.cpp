#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/Enhancements.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.LowerBankRewardThresholds"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

const int FIRST_BANK_THRESHOLD = 100;
const int INTEREST_BANK_THRESHOLD = 500;
const int SECOND_BANK_THRESHOLD = 1000;

void RegisterCustomBankRewardThresholds() {
    REGISTER_VB_SHOULD(VB_PASS_FIRST_BANK_THRESHOLD, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (CVAR) {
            *should = (HS_GET_BANK_RUPEES() >= FIRST_BANK_THRESHOLD) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_10_08);
        }
    });

    REGISTER_VB_SHOULD(VB_PASS_INTEREST_BANK_THRESHOLD, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (CVAR) {
            *should = (HS_GET_BANK_RUPEES() >= INTEREST_BANK_THRESHOLD);
        }
    });

    REGISTER_VB_SHOULD(VB_PASS_SECOND_BANK_THRESHOLD, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (CVAR && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_08) && HS_GET_BANK_RUPEES() >= SECOND_BANK_THRESHOLD) {
            *should = true;
        }
    });

    REGISTER_VB_SHOULD(VB_PASS_SECOND_BANK_THRESHOLD_ALT, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        if (CVAR && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_08) && HS_GET_BANK_RUPEES() >= SECOND_BANK_THRESHOLD) {
            *should = true;
        }
    });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x045B, [](u16* textId, bool* loadFromMessageTable) {
            if (!CVAR) {
                return;
            }

            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.autoFormat = false;
            CustomMessage::Replace(&entry.msg, "200", "100");

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x045C, [](u16* textId, bool* loadFromMessageTable) {
            if (!CVAR) {
                return;
            }

            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.autoFormat = false;
            CustomMessage::Replace(&entry.msg, "1000", "500");

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x045D, [](u16* textId, bool* loadFromMessageTable) {
            if (!CVAR) {
                return;
            }

            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.autoFormat = false;
            CustomMessage::Replace(&entry.msg, "5000", "1000");

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });
}
