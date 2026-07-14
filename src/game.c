#include "game.h"
#include "map.h"
#include "player.h"
#include "bomb.h"
#include "enemy.h"
#include "render.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

void Game_ResetRound(Game *g) {
    Map_Reset(g);

    // Clear bombs
    memset(g->bombs,     0, sizeof(g->bombs));
    // Clear power-ups
    memset(g->powerups,  0, sizeof(g->powerups));
    g->puCount = 0;
    // Clear particles
    memset(g->particles, 0, sizeof(g->particles));

    // Init players
    Player_Init(&g->p1, 1, 1, BLUE, "P1",
                KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE);
    Player_Init(&g->p2, MAP_COLS - 2, MAP_ROWS - 2, GREEN, "P2",
                KEY_W, KEY_S, KEY_A, KEY_D, KEY_F);

    // Spawn enemies safely
    memset(g->enemies, 0, sizeof(g->enemies));
    Enemy_SpawnSafe(g);

    g->roundWinner = 0;
}

void Game_ResetMatch(Game *g) {
    g->p1Wins     = 0;
    g->p2Wins     = 0;
    g->matchWinner = 0;
    Game_ResetRound(g);
}

void Game_Init(Game *g) {
    srand((unsigned int)time(NULL));
    memset(g, 0, sizeof(Game));
    g->state = STATE_MENU;
    Map_InitSpiral(g);
    Game_ResetMatch(g);
}
// Dans Game_Init(), APRÈS InitWindow() :
void Assets_Load(Game *g) {
    g->assets.player1 = LoadTexture("assets/player1.png");
    g->assets.player2 = LoadTexture("assets/player2.png");
    g->assets.bomb    = LoadTexture("assets/bomb.png");
    g->assets.enemy   = LoadTexture("assets/enemy.png");
    g->assets.wall    = LoadTexture("assets/wall.png");
    g->assets.block   = LoadTexture("assets/block.png");
    g->assets.floor   = LoadTexture("assets/floor.png");
}

void Assets_Unload(Game *g) {
    UnloadTexture(g->assets.player1);
    UnloadTexture(g->assets.player2);
    UnloadTexture(g->assets.bomb);
    UnloadTexture(g->assets.enemy);
    UnloadTexture(g->assets.wall);
    UnloadTexture(g->assets.block);
    UnloadTexture(g->assets.floor);
}
void Game_Update(Game *g, float dt) {
    if (g->state == STATE_MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            g->state = STATE_PLAYING;
        }
        return;
    }

    if (g->state == STATE_ROUND_OVER) {
        if (IsKeyPressed(KEY_SPACE)) {
            Game_ResetRound(g);
            g->state = STATE_PLAYING;
        }
        return;
    }

    if (g->state == STATE_GAMEOVER) {
        if (IsKeyPressed(KEY_R)) {
            Game_ResetMatch(g);
            g->state = STATE_PLAYING;
        }
        return;
    }

    // ── STATE_PLAYING ────────────────────────────────────────────────────────
    Map_Update(g, dt);
    Player_Update(&g->p1, dt, g);
    Player_Update(&g->p2, dt, g);
    Enemy_Update(g, dt);
    Bomb_Update(g, dt);
    Render_UpdateParticles(g, dt);

    // Check round-end conditions
    int p1Dead = !g->p1.alive;
    int p2Dead = !g->p2.alive;

    if (p1Dead || p2Dead) {
        if (!p1Dead && p2Dead)       { g->roundWinner = 1; g->p1Wins++; }
        else if (p1Dead && !p2Dead)  { g->roundWinner = 2; g->p2Wins++; }
        else                         { g->roundWinner = 0; }  // Draw

        if (g->p1Wins >= WINS_TO_WIN) {
            g->matchWinner = 1;
            g->state = STATE_GAMEOVER;
        } else if (g->p2Wins >= WINS_TO_WIN) {
            g->matchWinner = 2;
            g->state = STATE_GAMEOVER;
        } else {
            g->state = STATE_ROUND_OVER;
        }
    }
}
