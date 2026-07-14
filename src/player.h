#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

void Player_Init(Player *p, int x, int y, Color color, const char *label,
                 KeyboardKey up, KeyboardKey down, KeyboardKey left,
                 KeyboardKey right, KeyboardKey bombKey);
void Player_Update(Player *p, float dt, Game *g);
void Player_Hit(Player *p, int spawnX, int spawnY);

#endif
