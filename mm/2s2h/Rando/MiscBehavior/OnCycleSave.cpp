#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include <variables.h>
}

SaveInfo saveInfoCopyForRando;

void Rando::MiscBehavior::BeforeEndOfCycleSave() {
    memcpy(&saveInfoCopyForRando, &gSaveContext.save.saveInfo, sizeof(SaveInfo));
}

void Rando::MiscBehavior::AfterEndOfCycleSave() {
    // TODO: Tbh idk what exactly to persist, come back here later
}
