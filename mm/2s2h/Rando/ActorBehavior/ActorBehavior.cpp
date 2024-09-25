#include "ActorBehavior.h"

void Rando::InitActorBehavior(bool isRando) {
    ActorBehavior::InitEnBoxBehavior(isRando);
    ActorBehavior::InitEnElfgrpBehavior(isRando);
    ActorBehavior::InitEnElforgBehavior(isRando);
    ActorBehavior::InitEnItem00Behavior(isRando);
    ActorBehavior::InitEnSiBehavior(isRando);
    ActorBehavior::InitItemBHeartBehavior(isRando);
}
