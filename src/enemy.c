#include "enemy.h"
#include "player.h"
#include "render.h"
#include <stdlib.h>

static int CanEnemyStep(Game *g, int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
    if (g->map[row][col] != TILE_EMPTY) return 0;
    // Don't walk onto active bombs
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb *b = &g->bombs[i];
        if (b->active && !b->exploding && b->x == col && b->y == row) return 0;
    }
    return 1;
}

// Safe spawn with:
//  - Minimum distance from both player spawn corners
//  - Minimum distance between enemies
//  - Clears surrounding destructible blocks so enemy can move
void Enemy_SpawnSafe(Game *g) {
    int spawned = 0, attempts = 0;
    while (spawned < MAX_ENEMIES && attempts < 2000) {
        attempts++;
        int row = rand() % MAP_ROWS;
        int col = rand() % MAP_COLS;

        int distP1 = abs(row - 1) + abs(col - 1);
        int distP2 = abs(row - (MAP_ROWS - 2)) + abs(col - (MAP_COLS - 2));
        if (distP1 <= 4 || distP2 <= 4) continue;
        if (g->map[row][col] == TILE_WALL) continue;

        // Spacing between enemies
        int tooClose = 0;
        for (int i = 0; i < spawned; i++) {
            int d = abs(row - g->enemies[i].y) + abs(col - g->enemies[i].x);
            if (d < 4) { tooClose = 1; break; }
        }
        if (tooClose) continue;

        // Clear the tile and its 4 neighbours (destructible only)
        g->map[row][col] = TILE_EMPTY;
        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};
        for (int d = 0; d < 4; d++) {
            int nx = col + dx[d], ny = row + dy[d];
            if (nx > 0 && nx < MAP_COLS - 1 && ny > 0 && ny < MAP_ROWS - 1)
                if (g->map[ny][nx] == TILE_BLOCK) g->map[ny][nx] = TILE_EMPTY;
        }

        g->enemies[spawned].x         = col;
        g->enemies[spawned].y         = row;
        g->enemies[spawned].alive     = 1;
        g->enemies[spawned].moveTimer = 0.0f;
        // Random speed between 0.30s and 0.55s per tile
        g->enemies[spawned].moveDelay = 0.30f + (float)(rand() % 26) / 100.0f;
        g->enemies[spawned].dir       = rand() % 4;
        spawned++;
    }
}

void Enemy_Update(Game *g, float dt) {
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->alive) continue;

        // Kill by explosion
        if (g->explosionMap[e->y][e->x] > 0.5f) {
            e->alive = 0;
            Render_SpawnParticles(g, e->x, e->y, RED);
            continue;
        }

        // Movement: try current direction, else pick a random valid one
        e->moveTimer += dt;
        if (e->moveTimer >= e->moveDelay) {
            e->moveTimer = 0.0f;
            int nx = e->x + dx[e->dir];
            int ny = e->y + dy[e->dir];

            if (CanEnemyStep(g, ny, nx)) {
                e->x = nx;
                e->y = ny;
            } else {
                // Collect all valid directions and pick one at random
                int valid[4], count = 0;
                for (int d = 0; d < 4; d++) {
                    if (CanEnemyStep(g, e->y + dy[d], e->x + dx[d]))
                        valid[count++] = d;
                }
                if (count > 0) {
                    e->dir = valid[rand() % count];
                    e->x += dx[e->dir];
                    e->y += dy[e->dir];
                }
            }
        }

        // Kill player on contact
        if (g->p1.alive && g->p1.x == e->x && g->p1.y == e->y)
            Player_Hit(&g->p1, 1, 1);
        if (g->p2.alive && g->p2.x == e->x && g->p2.y == e->y)
            Player_Hit(&g->p2, MAP_COLS - 2, MAP_ROWS - 2);
    }
}

int Enemy_AllDead(Game *g) {
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].alive) return 0;
    return 1;
}
