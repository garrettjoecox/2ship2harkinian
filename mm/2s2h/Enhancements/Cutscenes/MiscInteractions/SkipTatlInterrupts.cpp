#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_Elf_Msg3/z_elf_msg3.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipTatlInterrupts() {
    // First time entering Clock Town Interupt
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.entrance == ENTRANCE(SOUTH_CLOCK_TOWN, 0) && gSaveContext.save.cutsceneIndex == 0 &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_04) &&
            CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
            Flags_SetWeekEventReg(WEEKEVENTREG_59_04);
        }
    });

    // General interupt
    COND_VB_SHOULD(VB_TATL_INTERUPT_MSG3, CVAR, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0) && *should) {
            Actor* actor = va_arg(args, Actor*);
            *should = false;
            if (ELFMSG3_GET_SWITCH_FLAG(actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, ELFMSG3_GET_SWITCH_FLAG(actor));
            }
            Actor_Kill(actor);
        }
    });

    // General interupt 2 (the flags were directly copied from the original code)
    COND_VB_SHOULD(VB_TATL_INTERUPT_MSG6, CVAR, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0) && *should) {
            Actor* actor = va_arg(args, Actor*);
            *should = false;
            switch (actor->textId) {
                case 0x224:
                    SET_WEEKEVENTREG(WEEKEVENTREG_79_10);
                    break;

                case 0x25B:
                    SET_WEEKEVENTREG(WEEKEVENTREG_88_20);
                    break;

                case 0x216:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);
                    break;

                case 0x231:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_01);
                    break;

                case 0x232:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_02);
                    break;

                case 0x233:
                    SET_WEEKEVENTREG(WEEKEVENTREG_80_04);
                    break;
            }
            if (CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_10);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_20);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_40);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_80);
            }
            Actor_Kill(actor);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipTatlInterrupts, { CVAR_NAME });
