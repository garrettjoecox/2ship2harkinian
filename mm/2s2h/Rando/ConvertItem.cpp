#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
}

RandoItemId Rando::ConvertItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_PROGRESSIVE_BOW:
            if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                return RI_BOW;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 1) {
                return RI_QUIVER_40;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 2) {
                return RI_QUIVER_50;
            } else {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_PROGRESSIVE_MAGIC:
            if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                return RI_SINGLE_MAGIC;
            } else if (!gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                return RI_DOUBLE_MAGIC;
            } else {
                return RI_RUPEE_BLUE;
            }
            break;
        default:
            return randoItemId;
    }
}
