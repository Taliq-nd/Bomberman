#include "player.h"
#include "bomb.h"
#include "map.h"

void Player_Init(Player *p, int x, int y, Color color, const char *label,
                 KeyboardKey up, KeyboardKey down, KeyboardKey left,
                 KeyboardKey right, KeyboardKey bombKey) {
    p->x = x;       p->y = y;
    p->lives = 3;   p->alive = 1;
    p->bombRange = 2; p->maxBombs = 2; p->activeBombs = 0;
    p->speed = 0.15f; p->moveTimer = 0.0f;
    p->invulnTimer = 0.0f;
    p->color = color; p->label = label;
    p->up = up; p->down = down; p->left = left;
    p->right = right; p->bombKey = bombKey;
}

void Player_Hit(Player *p, int spawnX, int spawnY) {
    if (p->invulnTimer > 0.0f) return;
    p->lives--;
    if (p->lives <= 0) {
        p->alive = 0;
    } else {
        p->x = spawnX;
        p->y = spawnY;
        p->invulnTimer = 1.5f;
    }
}

// Returns 1 if player can step on (row, col)
static int CanStep(Game *g, Player *p, int row, int col) {
    if (!Map_CanMove(g, row, col)) return 0;
    // Block walking onto an active (non-exploding) bomb — unless already standing on it
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb *b = &g->bombs[i];
        if (b->active && !b->exploding && b->x == col && b->y == row) {
            if (p->x == col && p->y == row) return 1; // Already on it — allow escape
            return 0;
        }
    }
    return 1;
}

void Player_Update(Player *p, float dt, Game *g) {
    if (!p->alive) return;

    // Invulnerability countdown
    if (p->invulnTimer > 0.0f) p->invulnTimer -= dt;

    // Movement
    p->moveTimer += dt;
    if (p->moveTimer >= p->speed) {
        if (IsKeyDown(p->up)    && CanStep(g, p, p->y - 1, p->x)) { p->y--; p->moveTimer = 0; }
        if (IsKeyDown(p->down)  && CanStep(g, p, p->y + 1, p->x)) { p->y++; p->moveTimer = 0; }
        if (IsKeyDown(p->left)  && CanStep(g, p, p->y, p->x - 1)) { p->x--; p->moveTimer = 0; }
        if (IsKeyDown(p->right) && CanStep(g, p, p->y, p->x + 1)) { p->x++; p->moveTimer = 0; }
    }

    // Place bomb
    if (IsKeyPressed(p->bombKey) && p->activeBombs < p->maxBombs)
        Bomb_Place(g, p->x, p->y, p->bombRange, p);

    // Pick up power-ups
    for (int i = 0; i < g->puCount; i++) {
        PowerUp *pu = &g->powerups[i];
        if (!pu->active || pu->x != p->x || pu->y != p->y) continue;
        if (pu->type == PU_RANGE) p->bombRange++;
        if (pu->type == PU_SPEED && p->speed > 0.06f) p->speed -= 0.025f;
        if (pu->type == PU_BOMB)  p->maxBombs++;
        pu->active = 0;
    }

    // Damage from explosion
    if (g->explosionMap[p->y][p->x] > 0.5f) {
        int sx = (p->label[1] == '1') ? 1 : MAP_COLS - 2;
        int sy = (p->label[1] == '1') ? 1 : MAP_ROWS - 2;
        Player_Hit(p, sx, sy);
    }
}
