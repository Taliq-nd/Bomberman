#include "bomb.h"
#include "render.h"
#include <stdlib.h>

void Bomb_Place(Game *g, int x, int y, int range, Player *owner) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!g->bombs[i].active) {
            g->bombs[i].x           = x;
            g->bombs[i].y           = y;
            g->bombs[i].timer       = 3.0f;
            g->bombs[i].range       = range;
            g->bombs[i].active      = 1;
            g->bombs[i].exploding   = 0;
            g->bombs[i].explodeTimer = 0.0f;
            g->bombs[i].owner       = owner;
            owner->activeBombs++;
            return;
        }
    }
}

// Chain reaction: explode any bomb sitting on (row, col)
static void ChainReaction(Game *g, int row, int col) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (g->bombs[i].active && !g->bombs[i].exploding &&
            g->bombs[i].x == col && g->bombs[i].y == row) {
            Bomb_Explode(g, &g->bombs[i]);
        }
    }
}

void Bomb_Explode(Game *g, Bomb *b) {
    b->exploding    = 1;
    b->explodeTimer = 0.45f;

    g->map[b->y][b->x]         = TILE_EMPTY;
    g->explosionMap[b->y][b->x] = 1.0f;
    g->itemMap[b->y][b->x]     = 0;
    Render_SpawnParticles(g, b->x, b->y, ORANGE);

    int dx[] = {0,  0, 1, -1};
    int dy[] = {1, -1, 0,  0};

    for (int d = 0; d < 4; d++) {
        for (int i = 1; i <= b->range; i++) {
            int nx = b->x + dx[d] * i;
            int ny = b->y + dy[d] * i;
            if (nx < 0 || nx >= MAP_COLS || ny < 0 || ny >= MAP_ROWS) break;
            if (g->map[ny][nx] == TILE_WALL) break;

            // Chain another bomb?
            ChainReaction(g, ny, nx);

            if (g->map[ny][nx] == TILE_BLOCK) {
                g->map[ny][nx]         = TILE_EMPTY;
                g->explosionMap[ny][nx] = 1.0f;
                g->itemMap[ny][nx]     = 0;
                Render_SpawnParticles(g, nx, ny, BROWN);
                // 33% chance power-up drop
                if (g->puCount < MAX_POWERUPS && rand() % 3 == 0) {
                    g->powerups[g->puCount++] = (PowerUp){nx, ny, (rand() % 3) + 1, 1};
                }
                break;
            }
            g->explosionMap[ny][nx] = 1.0f;
            g->itemMap[ny][nx]     = 0;
        }
    }
}

void Bomb_Update(Game *g, float dt) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb *b = &g->bombs[i];
        if (!b->active) continue;

        if (!b->exploding) {
            b->timer -= dt;
            if (b->timer <= 0.0f) Bomb_Explode(g, b);
        } else {
            b->explodeTimer -= dt;
            if (b->explodeTimer <= 0.0f) {
                if (b->owner && b->owner->activeBombs > 0)
                    b->owner->activeBombs--;
                b->active = 0;
            }
        }
    }
}
