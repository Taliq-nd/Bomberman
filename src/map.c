#include "map.h"
#include <string.h>

static const int BASE_MAP[MAP_ROWS][MAP_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,2,2,2,2,2,2,2,2,2,0,0,1},
    {1,0,1,2,1,2,1,2,1,2,1,2,1,0,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,2,1,2,1,2,1,2,1,2,1,2,1},
    {1,2,2,2,2,2,0,0,0,2,2,2,2,2,1},
    {1,2,1,2,1,2,1,2,1,2,1,2,1,2,1},
    {1,2,2,2,2,2,0,0,0,2,2,2,2,2,1},
    {1,2,1,2,1,2,1,2,1,2,1,2,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,0,1,2,1,2,1,2,1,2,1,2,1,0,1},
    {1,0,0,2,2,2,2,2,2,2,2,2,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Build the spiral order for sudden death (outside → inside)
void Map_InitSpiral(Game *g) {
    g->sdTotalTiles = 0;
    int top = 0, bottom = MAP_ROWS - 1;
    int left = 0, right = MAP_COLS - 1;

    while (top <= bottom && left <= right) {
        for (int i = left; i <= right; i++) {
            g->sdSpiralX[g->sdTotalTiles] = i;
            g->sdSpiralY[g->sdTotalTiles++] = top;
        }
        top++;
        for (int i = top; i <= bottom; i++) {
            g->sdSpiralX[g->sdTotalTiles] = right;
            g->sdSpiralY[g->sdTotalTiles++] = i;
        }
        right--;
        if (top <= bottom) {
            for (int i = right; i >= left; i--) {
                g->sdSpiralX[g->sdTotalTiles] = i;
                g->sdSpiralY[g->sdTotalTiles++] = bottom;
            }
            bottom--;
        }
        if (left <= right) {
            for (int i = bottom; i >= top; i--) {
                g->sdSpiralX[g->sdTotalTiles] = left;
                g->sdSpiralY[g->sdTotalTiles++] = i;
            }
            left++;
        }
    }
}

void Map_Reset(Game *g) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++) {
            g->map[r][c]       = BASE_MAP[r][c];
            g->itemMap[r][c]   = 0;
            g->explosionMap[r][c] = 0.0f;
        }
    g->roundTimer = 0.0f;
    g->sdIndex    = 0;
    g->sdTimer    = 0.0f;
}

void Map_Update(Game *g, float dt) {
    // Fade explosions
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            if (g->explosionMap[r][c] > 0.0f) {
                g->explosionMap[r][c] -= dt * 2.5f;
                if (g->explosionMap[r][c] < 0.0f) g->explosionMap[r][c] = 0.0f;
            }

    // Sudden death
    g->roundTimer += dt;
    if (g->roundTimer >= ROUND_TIME_LIMIT) {
        g->sdTimer += dt;
        if (g->sdTimer >= SUDDEN_DEATH_DELAY && g->sdIndex < g->sdTotalTiles) {
            int sx = g->sdSpiralX[g->sdIndex];
            int sy = g->sdSpiralY[g->sdIndex];
            g->map[sy][sx]     = TILE_WALL;
            g->itemMap[sy][sx] = 0;
            // Kill anything on that tile
            if (g->p1.alive && g->p1.x == sx && g->p1.y == sy) g->p1.alive = 0;
            if (g->p2.alive && g->p2.x == sx && g->p2.y == sy) g->p2.alive = 0;
            for (int i = 0; i < MAX_ENEMIES; i++)
                if (g->enemies[i].alive && g->enemies[i].x == sx && g->enemies[i].y == sy)
                    g->enemies[i].alive = 0;
            g->sdIndex++;
            g->sdTimer = 0.0f;
        }
    }
}

int Map_CanMove(Game *g, int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
    return g->map[row][col] == TILE_EMPTY;
}
