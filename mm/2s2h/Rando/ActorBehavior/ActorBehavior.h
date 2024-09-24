#ifndef RANDO_ACTOR_BEHAVIOR_H
#define RANDO_ACTOR_BEHAVIOR_H

#include "Rando/Rando.h"

namespace Rando {

void InitActorBehavior(bool isRando);

namespace ActorBehavior {

void InitEnBoxBehavior(bool isRando);
void InitEnElfgrpBehavior(bool isRando);
void InitEnElforgBehavior(bool isRando);
void InitEnItem00Behavior(bool isRando);
void InitItemBHeartBehavior(bool isRando);

} // namespace ActorBehavior

} // namespace Rando

#endif
