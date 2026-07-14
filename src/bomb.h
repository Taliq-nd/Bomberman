#ifndef BOMB_H
#define BOMB_H

#include "types.h"

void Bomb_Place(Game *g, int x, int y, int range, Player *owner);
void Bomb_Explode(Game *g, Bomb *b);
void Bomb_ClearExplosion(Game *g, Bomb *b);
void Bomb_Update(Game *g, float dt);

#endif
