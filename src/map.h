#ifndef MAP_H
#define MAP_H

#include "types.h"

void Map_InitSpiral(Game *g);
void Map_Reset(Game *g);
void Map_Update(Game *g, float dt);
int  Map_CanMove(Game *g, int row, int col);

#endif
