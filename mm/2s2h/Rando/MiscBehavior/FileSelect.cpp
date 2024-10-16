#include "MiscBehavior.h"
#include <libultraship/libultraship.h>
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h_assets.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include <macros.h>
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
extern s16 sWindowContentColors[3];
extern FileSelectState* gFileSelectState;
}

u8 isRando[4];

// Cannot use OPEN_DISPS inside a namespaced function?
void RegisterShoulds() {
    REGISTER_VB_SHOULD(VB_DRAW_FILE_SELECT_SAVE_TYPE_BOX, {
        int fileIndex = va_arg(args, int);

        if (!isRando[fileIndex]) {
            return;
        }

        *should = false;

        OPEN_DISPS(gFileSelectState->state.gfxCtx);

        gDPSetPrimColor(
            POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1], sWindowContentColors[2],
            gFileSelectState->selectedFileIndex == fileIndex ? 255 : gFileSelectState->nameBoxAlpha[fileIndex]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelRANDButtonTex, G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);

        CLOSE_DISPS(gFileSelectState->state.gfxCtx);
    });
}

// Doesn't really look great yet, but the start to how we will augment the file select screen for rando saves
void Rando::MiscBehavior::InitFileSelect() {
    RegisterShoulds();

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFileSelectSaveLoad>(
        [](s16 fileNum, bool isOwlSave, SaveContext* saveContext) {
            isRando[fileNum + (isOwlSave ? 2 : 0)] = saveContext->save.shipSaveInfo.saveType == SAVETYPE_RANDO;
        });
}
