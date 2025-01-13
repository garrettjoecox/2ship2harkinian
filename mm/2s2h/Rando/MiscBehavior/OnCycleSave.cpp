#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include <variables.h>
#include <functions.h>
}

SaveContext saveContextCopy;

void Rando::MiscBehavior::BeforeEndOfCycleSave() {
    memcpy(&saveContextCopy, &gSaveContext, sizeof(SaveContext));
}

void Rando::MiscBehavior::AfterEndOfCycleSave() {
    for (int i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.inventory.dungeonItems[i] = saveContextCopy.save.saveInfo.inventory.dungeonItems[i];
        gSaveContext.save.saveInfo.inventory.dungeonKeys[i] =
            saveContextCopy.save.shipSaveInfo.rando.foundDungeonKeys[i];
        gSaveContext.save.saveInfo.inventory.strayFairies[i] = saveContextCopy.save.saveInfo.inventory.strayFairies[i];
    }

    if (saveContextCopy.save.saveInfo.weekEventReg[((WEEKEVENTREG_08_80) >> 8)] & ((WEEKEVENTREG_08_80)&0xFF)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS]) {
        gSaveContext.save.saveInfo.skullTokenCount = saveContextCopy.save.saveInfo.skullTokenCount;
    }

    // For now, we're just going to always persist these slots. We may do something smarter here later if this causes
    // any issues.
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_DEED];
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA];
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE];

    // Unset any flags used for checks, whether or not they get the item or junk is determined on our end instead.
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        if (randoCheckId == RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET || randoCheckId == RC_CLOCK_TOWN_WEST_BANK_INTEREST ||
            randoCheckId == RC_CLOCK_TOWN_WEST_BANK_PIECE_OF_HEART) {
            continue;
        }
        switch (randoStaticCheck.flagType) {
            case FLAG_WEEK_EVENT_REG:
                // Clear the flag without triggering hook
                WEEKEVENTREG((randoStaticCheck.flag) >> 8) =
                    GET_WEEKEVENTREG((randoStaticCheck.flag) >> 8) & (u8) ~((randoStaticCheck.flag) & 0xFF);
                break;
                // most of the others are handled by the game, with the exception of PERSISTENT_CYCLE_FLAGS_SET, not
                // sure if any of these cases affect us yet so ignoring for now
        }
    }
}
