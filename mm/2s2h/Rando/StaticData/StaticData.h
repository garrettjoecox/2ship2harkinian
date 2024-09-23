#ifndef RANDO_STATIC_DATA_H
#define RANDO_STATIC_DATA_H

#include <unordered_map>
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
    RandoCheckType randoCheckType;
    SceneId sceneId;
    FlagType flagType;
    s32 flag;
    RandoItemId randoItemId;
};

extern std::unordered_map<RandoCheckId, RandoStaticCheck> Checks;

struct RandoStaticItem {
    const char* name;
    ItemId itemId;
    GetItemId getItemId;
    GetItemDrawId drawId;
};

extern std::unordered_map<RandoItemId, RandoStaticItem> Items;

RandoStaticCheck GetCheckFromFlag(FlagType flagType, s32 flag, s16 sceneId = SCENE_MAX);

} // namespace StaticData

} // namespace Rando

#endif // RANDO_STATIC_DATA_H
