#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "overlays/actors/ovl_En_Go/z_en_go.h"
}

void RegisterPowderKegCertification() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GO, CVarGetInteger("gEnhancements.Minigames.PowderKegCertification", 0), 
        [](Actor* actor) {
        EnGo* goron = (EnGo*)actor;

        if (actor->params != ENGO_MEDIGORON) {
            return;
        }

        Flags_SetWeekEventReg(WEEKEVENTREG_19_01); // Started Test
        Flags_SetWeekEventReg(WEEKEVENTREG_19_02); // Succeeded Test

        // TODO: Probably migrate this to some unified flag system.
        gSaveContext.cycleSceneFlags[SCENE_17SETUGEN].switch0 |= 1 << (0x34 & 0x1f);
    });

    // "Looks like you succeeded..."
    COND_ID_HOOK(OnOpenText, 0xc86, CVarGetInteger("gEnhancements.Minigames.PowderKegCertification", 0), [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        //entry.autoFormat = false;
        entry.msg = "Take one on the house, don't tell your parents.";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
