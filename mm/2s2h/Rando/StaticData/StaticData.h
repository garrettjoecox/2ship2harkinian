#ifndef RANDO_STATIC_DATA_H
#define RANDO_STATIC_DATA_H

#include <map>
#include "Rando/Types.h"
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64item.h"
#include "z64scene.h"
}

namespace Rando {

namespace StaticData {

struct RandoStaticCheck {
    RandoCheckId randoCheckId;
    const char* name;
    RandoCheckType randoCheckType;
    SceneId sceneId;
    FlagType flagType;
    s32 flag;
    RandoItemId randoItemId;
};

extern std::map<RandoCheckId, RandoStaticCheck> Checks;

RandoStaticCheck GetCheckFromFlag(FlagType flagType, s32 flag, s16 sceneId = SCENE_MAX);
RandoCheckId GetCheckIdFromName(const char* name);

struct RandoStaticItem {
    RandoItemId randoItemId;
    const char* spoilerName;
    const char* name;
    ItemId itemId;
    GetItemId getItemId;
    GetItemDrawId drawId;
};

extern std::map<RandoItemId, RandoStaticItem> Items;

RandoItemId GetItemIdFromName(const char* name);

struct RandoStaticOption {
    RandoOptionId randoOptionId;
    const char* name;
    const char* cvar;
};

extern std::map<RandoOptionId, RandoStaticOption> Options;

RandoOptionId GetOptionIdFromName(const char* name);

} // namespace StaticData

} // namespace Rando

#endif // RANDO_STATIC_DATA_H
