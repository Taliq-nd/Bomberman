#ifndef RENDER_H
#define RENDER_H

#include "types.h"

void Render_SpawnParticles(Game *g, int tileX, int tileY, Color color);
void Render_UpdateParticles(Game *g, float dt);
void Render_Menu(void);
void Render_Game(Game *g);
void Render_HUD(Game *g);
void Render_Overlay(Game *g);

#endif
