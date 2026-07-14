#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <math.h>

// ─── Constants ────────────────────────────────────────────────────────────────
#define TILE_SIZE             80
#define MAP_ROWS              13
#define MAP_COLS              15
#define MAX_BOMBS             20
#define MAX_ENEMIES            4
#define MAX_POWERUPS          10
#define MAX_PARTICLES        100
#define ROUND_TIME_LIMIT      90.0f
#define SUDDEN_DEATH_DELAY     0.4f
#define WINS_TO_WIN            3

// ─── Tile types ───────────────────────────────────────────────────────────────
#define TILE_EMPTY   0
#define TILE_WALL    1
#define TILE_BLOCK   2

// ─── Power-up types ───────────────────────────────────────────────────────────
#define PU_NONE    0
#define PU_RANGE   1
#define PU_SPEED   2
#define PU_BOMB    3

// ─── Game states ──────────────────────────────────────────────────────────────
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_ROUND_OVER,
    STATE_GAMEOVER
} GameState;

// ─── Structs ──────────────────────────────────────────────────────────────────
typedef struct {
    int x, y;
    int lives, alive;
    int bombRange, maxBombs, activeBombs;
    float speed, moveTimer;
    float invulnTimer;
    Color color;
    const char *label;
    KeyboardKey up, down, left, right, bombKey;
} Player;

typedef struct {
    int x, y;
    float timer;
    int range, active, exploding;
    float explodeTimer;
    Player *owner;
} Bomb;

typedef struct {
    int x, y;
    int alive;
    float moveTimer, moveDelay;
    int dir;
} Enemy;

typedef struct {
    int x, y;
    int type, active;
} PowerUp;

typedef struct {
    Vector2 pos, speed;
    Color color;
    float alpha, lifeTime;
    int active;
} Particle;
// Dans types.h, dans la struct Game
typedef struct {
    Texture2D player1;
    Texture2D player2;
    Texture2D bomb;
    Texture2D enemy;
    Texture2D wall;
    Texture2D block;
    Texture2D floor;
} Assets;

typedef struct {
    int map[MAP_ROWS][MAP_COLS];
    int itemMap[MAP_ROWS][MAP_COLS];
    float explosionMap[MAP_ROWS][MAP_COLS];
    Player p1, p2;
    Bomb bombs[MAX_BOMBS];
    Enemy enemies[MAX_ENEMIES];
    PowerUp powerups[MAX_POWERUPS];
    Particle particles[MAX_PARTICLES];
    int puCount;
    GameState state;
    int roundWinner;
    int matchWinner;
    int p1Wins, p2Wins;
    float roundTimer;
    // Sudden death
    int sdSpiralX[MAP_ROWS * MAP_COLS];
    int sdSpiralY[MAP_ROWS * MAP_COLS];
    int sdTotalTiles;
    int sdIndex;
    float sdTimer;
    Assets assets;
} Game;


#endif
