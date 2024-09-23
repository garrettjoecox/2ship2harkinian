#ifndef RANDO_H
#define RANDO_H

#include "StaticData/StaticData.h"
#include "Types.h"

#define RANDO_SAVE_CHECKS gSaveContext.save.shipSaveInfo.rando.randoSaveChecks

namespace Rando {

void Init();
void DrawItem(RandoItemId randoItemId);
void GiveItem(RandoItemId randoItemId);

} // namespace Rando

#endif
