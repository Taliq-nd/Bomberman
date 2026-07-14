#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"

void Enemy_SpawnSafe(Game *g);
void Enemy_Update(Game *g, float dt);
int  Enemy_AllDead(Game *g);

#endif
