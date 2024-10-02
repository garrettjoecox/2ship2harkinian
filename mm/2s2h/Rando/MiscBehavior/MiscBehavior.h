#ifndef RANDO_MISC_BEHAVIOR_H
#define RANDO_MISC_BEHAVIOR_H

#include "Rando/Rando.h"

namespace Rando {

namespace MiscBehavior {

void Init();
void OnFileLoad();

void CheckQueue();
void CheckQueueReset();
void InitFileSelect();
void BeforeEndOfCycleSave();
void AfterEndOfCycleSave();
void OnFileCreate(s16 fileNum);
void OnFlagSet(FlagType flagType, u32 flag);
void OnSceneFlagSet(s16 sceneId, FlagType flagType, u32 flag);

} // namespace MiscBehavior

} // namespace Rando

#endif
