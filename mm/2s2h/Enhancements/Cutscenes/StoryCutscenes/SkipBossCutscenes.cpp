#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "global.h"
}

// TODO: Handle other bosses like Igos and Majora
void RegisterSkipBossCutscenes() {
    // Odolwa intro
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_BOSS_01, [](Actor* actor, bool* should) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
                SET_EVENTINF(EVENTINF_54);
            }
        });

    // Goht intro
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_BOSS_HAKUGIN, [](Actor* actor, bool* should) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
                /*
                 * EVENTINF_62 gets set after seeing the boss lair intro cutscene.
                 * EVENTINF_53 gets set after melting the ice (not skipped here, as that would skip the ice melting
                 * requirement).
                 */
                SET_EVENTINF(EVENTINF_62);
            }
        });

    // Gyorg intro
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_BOSS_03, [](Actor* actor, bool* should) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
                SET_EVENTINF(EVENTINF_56);
            }
        });

    // Twinmold intro
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_BOSS_02, [](Actor* actor, bool* should) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
                SET_EVENTINF(EVENTINF_55);
            }
        });

    REGISTER_VB_SHOULD(VB_START_CUTSCENE, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            s16* csId = va_arg(args, s16*);
            switch (gPlayState->sceneId) {
                case SCENE_MITURIN_BS: // Odolwa's Lair
                    if (*csId == 9) {  // Warping out
                        *should = false;
                    }
                    break;
                case SCENE_MITURIN:    // Woodfall Temple
                    if (*csId == 34) { // Cleared vines to room Deku Princess is in
                        *should = false;
                    }
                    break;
                case SCENE_HAKUGIN_BS: // Goht's Lair
                    if (*csId == 10) { // Warping out
                        *should = false;
                    }
                    break;
                case SCENE_10YUKIYAMANOMURA2: // Mountain Village (Spring)
                    if (*csId == 13) {        // Warping from Goht's Lair
                        *should = false;
                    }
                    break;
                case SCENE_SEA_BS:    // Gyorg's Lair
                    if (*csId == 9) { // Warping out
                        *should = false;
                    }
                    break;
                case SCENE_31MISAKI:   // Zora Cape
                    if (*csId == 18) { // Warping from Gyorg's Lair
                        *should = false;
                    }
                    break;
                case SCENE_INISIE_BS: // Twinmold's Lair
                    if (*csId == 9) { // Warping out
                        *should = false;
                    }
                    break;
                case SCENE_IKANA:      // Ikana Canyon
                    if (*csId == 31) { // Warping from Twinmold's Lair
                        *should = false;
                    }
                default:
                    break;
            }
        }
    });
}