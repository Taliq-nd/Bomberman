#ifndef GAME_H
#define GAME_H

#include "types.h"

void Game_Init(Game *g);
void Game_ResetRound(Game *g);
void Game_ResetMatch(Game *g);
void Game_Update(Game *g, float dt);

#endif
