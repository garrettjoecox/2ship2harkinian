#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "overlays/actors/ovl_En_Suttari/z_en_suttari.h"
void EnSuttari_TriggerTransition(PlayState* play, u16 entrance);
}

// Upon the second bomb theft textbox, Sakon fires off a move route that updates even before the textbox closes. This
// means it is possible for him to escape before the textbox is dismissed. This happens even in vanilla, so note that
// this is not a bug introduced by this skip.
void RegisterSkipBombBagTheftCutscene() {
    REGISTER_VB_SHOULD(VB_PLAY_BOMB_BAG_THEFT_CS, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            EnSuttari* enSuttari = va_arg(args, EnSuttari*);
            if (enSuttari->actor.colChkInfo.damageEffect == 0xF && enSuttari->unk428 == 4) {
                // unk428 stores the schedule return value, which becomes 4 when Sakon steals the bomb bag.
                // damageEffect is 0xF if the player stopped Sakon without killing him.
                // In this scenario, skip ahead to the transition instead of waiting for him flee.
                // No need to alter the scenario where the player kills Sakon, as the Bomb Shop Lady will already
                // trigger the transition upon his death.
                enSuttari->actor.speed = 0.0f;
                SET_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG);
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
                enSuttari->flags2 |= 4 | 8;
                EnSuttari_TriggerTransition(gPlayState, ENTRANCE(NORTH_CLOCK_TOWN, 7));
            } else {
                // Avoid cutscene
                *should = false;
            }
        }
    });
}
