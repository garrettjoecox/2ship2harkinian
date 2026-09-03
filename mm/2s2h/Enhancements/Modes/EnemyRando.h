#ifndef ENEMY_RANDO_H
#define ENEMY_RANDO_H

#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/Enhancements.h"

#define CVAR_ENEMY_RANDO_MODE "gModes.EnemyRando.Mode"
#define ENEMY_RANDO_MODE CVarGetInteger(CVAR_ENEMY_RANDO_MODE, ENEMY_RANDO_OFF)

void EnemyRando_DrawPoolSelector();

#endif // ENEMY_RANDO_H
