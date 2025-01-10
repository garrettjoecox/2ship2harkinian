#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.GalleryTwofer"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define SWAMP_CVAR_NAME "gEnhancements.Minigames.SwampArcheryScore"
#define SWAMP_CVAR CVarGetInteger(SWAMP_CVAR_NAME, 2180)
#define TOWN_CVAR_NAME "gEnhancements.Minigames.TownArcheryScore"
#define TOWN_CVAR CVarGetInteger(TOWN_CVAR_NAME, 50)

static s16 highestScore = 0;

void RegisterGalleryTwofer() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SYATEKI_MAN, CVAR, [](Actor* actor) {
        PlayState* play = gPlayState;
        Player* player = GET_PLAYER(play);
        EnSyatekiMan* thisx = (EnSyatekiMan*)actor;

        bool isSwampGallery = (play->sceneId == SCENE_SYATEKI_MORI);
        bool isTownGallery = (play->sceneId == SCENE_SYATEKI_MIZU);
        bool hasQuiver = isSwampGallery
                             ? CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE)
                             : CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE);

        bool needsHeartPiece = isSwampGallery
                                   ? !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE)
                                   : !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE);

        if (thisx->shootingGameState == SG_GAME_STATE_ENDED && thisx->score > highestScore) {
            highestScore = thisx->score;
        }

        bool gotPerfectScore = isSwampGallery ? (highestScore >= SWAMP_CVAR) : (highestScore >= TOWN_CVAR);

        if (hasQuiver && needsHeartPiece && gotPerfectScore) {
            if (!Actor_HasParent(&thisx->actor, play) && !(player->stateFlags1 & ~(PLAYER_STATE1_20))) {
                if (!IS_RANDO) {
                    // Set player state and position (otherwise player turns around)
                    // Rando handles giving items differently. As of now, can't change player rotation.
                    player->actor.shape.rot.y = -0x8000;
                    player->actor.velocity.z = 0.0f;
                    player->actor.velocity.x = 0.0f;
                    player->actor.world.rot.y = player->actor.shape.rot.y;
                }

                if (IS_RANDO) {
                    if (isSwampGallery) {
                        RANDO_SAVE_CHECKS[RC_SWAMP_SHOOTING_GALLERY_HIGH_SCORE].eligible = true;
                    } else if (isTownGallery) {
                        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_HIGH_SCORE].eligible = true;
                    }
                } else {
                    Actor_OfferGetItem(&thisx->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
                }

                if (isSwampGallery) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE);
                } else if (isTownGallery) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE);
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGalleryTwofer, { CVAR_NAME });