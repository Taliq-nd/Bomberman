#include "render.h"
#include <stdlib.h>

// Fonction utilitaire à mettre en haut de render.c
static void DrawTile(Texture2D tex, int col, int row) {
    Rectangle src  = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dest = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

// ─── Particles ────────────────────────────────────────────────────────────────

void Render_SpawnParticles(Game *g, int tileX, int tileY, Color color) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < 8; i++) {
        Particle *p = &g->particles[i];
        if (p->active) continue;
        p->pos    = (Vector2){ tileX * TILE_SIZE + TILE_SIZE / 2.0f,
                               tileY * TILE_SIZE + TILE_SIZE / 2.0f };
        p->speed  = (Vector2){ (float)(rand() % 100 - 50) / 10.0f,
                               (float)(rand() % 100 - 50) / 10.0f };
        p->color    = color;
        p->alpha    = 1.0f;
        p->lifeTime = 0.3f + (float)(rand() % 50) / 100.0f;
        p->active   = 1;
        spawned++;
    }
}

void Render_UpdateParticles(Game *g, float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &g->particles[i];
        if (!p->active) continue;
        p->pos.x   += p->speed.x;
        p->pos.y   += p->speed.y;
        p->lifeTime -= dt;
        p->alpha     = p->lifeTime * 2.0f;
        if (p->lifeTime <= 0.0f) p->active = 0;
    }
}

// ─── Menu ─────────────────────────────────────────────────────────────────────

void Render_Menu(void) {
    int W = MAP_COLS * TILE_SIZE;
    int H = MAP_ROWS * TILE_SIZE;
    int cx = W / 2;

    ClearBackground(GetColor(0x1a1a2eff));

    // Title
    const char *title = "ASCII BOMBERMAN";
    DrawText(title, cx - MeasureText(title, 48) / 2, H / 2 - 120, 48, YELLOW);

    // Controls
    DrawText("P1: Arrow keys + SPACE",  cx - 130, H / 2 - 30, 20, BLUE);
    DrawText("P2: WASD + F",            cx - 130, H / 2,      20, GREEN);

    // Power-up legend
    DrawText("Power-ups:", cx - 130, H / 2 + 50, 18, LIGHTGRAY);
    DrawRectangle(cx - 130, H / 2 + 74, 14, 14, YELLOW);
    DrawText("+Range",  cx - 112, H / 2 + 72, 16, LIGHTGRAY);
    DrawRectangle(cx - 60, H / 2 + 74, 14, 14, SKYBLUE);
    DrawText("+Speed",  cx - 42,  H / 2 + 72, 16, LIGHTGRAY);
    DrawRectangle(cx + 12, H / 2 + 74, 14, 14, PURPLE);
    DrawText("+Bomb",   cx + 30,  H / 2 + 72, 16, LIGHTGRAY);

    // Prompt
    const char *prompt = "Press ENTER to play";
    int flash = (int)(GetTime() * 2.0) % 2;
    if (flash) DrawText(prompt, cx - MeasureText(prompt, 22) / 2, H / 2 + 110, 22, WHITE);
}

// ─── Game world ───────────────────────────────────────────────────────────────

void Render_Game(Game *g) {
    ClearBackground(GetColor(0x274e13ff));

    // Map tiles
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int x = col * TILE_SIZE, y = row * TILE_SIZE;

            // Checkerboard floor
            Color floor = ((row + col) % 2 == 0)
                        ? GetColor(0x38761dff)
                        : GetColor(0x274e13ff);
            DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, floor);

            if (g->map[row][col] == TILE_WALL) {
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, DARKGRAY);
                DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, GRAY);
            } else if (g->map[row][col] == TILE_BLOCK) {
                DrawRectangle(x + 2, y + 2, TILE_SIZE - 4, TILE_SIZE - 4, BROWN);
                DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, DARKBROWN);
            }

            // Power-up on floor
            if (g->itemMap[row][col] > 0 && g->map[row][col] == TILE_EMPTY) {
                Color puColor = YELLOW;
                if (g->itemMap[row][col] == PU_SPEED) puColor = SKYBLUE;
                if (g->itemMap[row][col] == PU_BOMB)  puColor = PURPLE;
                DrawCircle(x + TILE_SIZE / 2, y + TILE_SIZE / 2, 10, puColor);
            }

            // Explosion overlay
            if (g->explosionMap[row][col] > 0.0f) {
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE,
                              ColorAlpha(ORANGE, g->explosionMap[row][col]));
                if (g->explosionMap[row][col] > 0.5f)
                    DrawText("*", x + 12, y + 10, 20, ColorAlpha(YELLOW, g->explosionMap[row][col]));
            }
        }
    }

    // Power-ups (separate pass for items in powerups array)
    for (int i = 0; i < g->puCount; i++) {
        PowerUp *pu = &g->powerups[i];
        if (!pu->active) continue;
        int x = pu->x * TILE_SIZE, y = pu->y * TILE_SIZE;
        Color c = (pu->type == PU_RANGE) ? YELLOW
                : (pu->type == PU_SPEED) ? SKYBLUE : PURPLE;
        DrawRectangle(x + 6, y + 6, TILE_SIZE - 12, TILE_SIZE - 12, c);
        const char *lbl = (pu->type == PU_RANGE) ? "+R"
                        : (pu->type == PU_SPEED) ? "+S" : "+B";
        DrawText(lbl, x + 8, y + 10, 18, BLACK);
    }

    // Bombs — pulsating circle
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb *b = &g->bombs[i];
        if (!b->active || b->exploding) continue;
        int bx = b->x * TILE_SIZE, by = b->y * TILE_SIZE;
        float pulse = sinf((float)GetTime() * 10.0f) * 2.0f;
        DrawCircle(bx + TILE_SIZE / 2, by + TILE_SIZE / 2, 13 + pulse, BLACK);
        DrawText("B", bx + 11, by + 9, 20, RED);
    }

    // Enemies — rounded rect with eyes
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->alive) continue;
        int ex = e->x * TILE_SIZE, ey = e->y * TILE_SIZE;
        DrawRectangleRounded((Rectangle){ex + 4, ey + 4, TILE_SIZE - 8, TILE_SIZE - 8},
                             0.35f, 6, RED);
        DrawCircle(ex + 14, ey + 16, 4, WHITE);
        DrawCircle(ex + 26, ey + 16, 4, WHITE);
        DrawCircle(ex + 14, ey + 16, 2, BLACK);
        DrawCircle(ex + 26, ey + 16, 2, BLACK);
        DrawText("E", ex + 13, ey + 23, 12, WHITE);
    }

    // Players — blink during invulnerability
    if (g->p1.alive) {
        int flash = (g->p1.invulnTimer > 0.0f) ? (int)(g->p1.invulnTimer * 8) % 2 : 1;
        if (flash) {
            int px = g->p1.x * TILE_SIZE, py = g->p1.y * TILE_SIZE;
            DrawRectangleRounded((Rectangle){px + 4, py + 4, TILE_SIZE - 8, TILE_SIZE - 8},
                                 0.4f, 6, g->p1.color);
            DrawText("P1", px + 6, py + 12, 16, WHITE);
        }
    }
    if (g->p2.alive) {
        int flash = (g->p2.invulnTimer > 0.0f) ? (int)(g->p2.invulnTimer * 8) % 2 : 1;
        if (flash) {
            int px = g->p2.x * TILE_SIZE, py = g->p2.y * TILE_SIZE;
            DrawRectangleRounded((Rectangle){px + 4, py + 4, TILE_SIZE - 8, TILE_SIZE - 8},
                                 0.4f, 6, g->p2.color);
            DrawText("P2", px + 6, py + 12, 16, WHITE);
        }
    }

    // Particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &g->particles[i];
        if (p->active)
            DrawCircleV(p->pos, 3, ColorAlpha(p->color, p->alpha));
    }
}

// ─── HUD ──────────────────────────────────────────────────────────────────────

void Render_HUD(Game *g) {
    int W = MAP_COLS * TILE_SIZE;

    // P1 stats
    DrawText(TextFormat("P1  Lives:%d  Range:%d  Bombs:%d",
             g->p1.lives, g->p1.bombRange, g->p1.maxBombs),
             8, 4, 16, BLUE);

    // P2 stats
    const char *p2hud = TextFormat("P2  Lives:%d  Range:%d  Bombs:%d",
                                   g->p2.lives, g->p2.bombRange, g->p2.maxBombs);
    DrawText(p2hud, W - MeasureText(p2hud, 16) - 8, 4, 16, GREEN);

    // Round wins
    DrawText(TextFormat("P1 Wins: %d/%d", g->p1Wins, WINS_TO_WIN),
             8, MAP_ROWS * TILE_SIZE - 22, 16, BLUE);
    const char *winsP2 = TextFormat("P2 Wins: %d/%d", g->p2Wins, WINS_TO_WIN);
    DrawText(winsP2, W - MeasureText(winsP2, 16) - 8, MAP_ROWS * TILE_SIZE - 22, 16, GREEN);

    // Timer / Sudden death warning
    float remaining = ROUND_TIME_LIMIT - g->roundTimer;
    if (remaining > 0) {
        const char *timer = TextFormat("Time: %d", (int)remaining);
        Color tc = (remaining < 15.0f) ? RED : WHITE;
        DrawText(timer, W / 2 - MeasureText(timer, 20) / 2, 2, 20, tc);
    } else {
        const char *sd = "SUDDEN DEATH!";
        int flash = (int)(GetTime() * 3.0) % 2;
        if (flash)
            DrawText(sd, W / 2 - MeasureText(sd, 20) / 2, 2, 20, RED);
    }
}

// ─── Overlays ─────────────────────────────────────────────────────────────────

void Render_Overlay(Game *g) {
    int W = MAP_COLS * TILE_SIZE;
    int H = MAP_ROWS * TILE_SIZE;

    if (g->state == STATE_ROUND_OVER) {
        DrawRectangle(0, H / 2 - 50, W, 100, ColorAlpha(BLACK, 0.75f));
        const char *msg;
        Color mc;
        if (g->roundWinner == 1)      { msg = "PLAYER 1 WINS THE ROUND!"; mc = BLUE; }
        else if (g->roundWinner == 2) { msg = "PLAYER 2 WINS THE ROUND!"; mc = GREEN; }
        else                          { msg = "DRAW!";                     mc = YELLOW; }
        DrawText(msg, W / 2 - MeasureText(msg, 28) / 2, H / 2 - 20, 28, mc);
        const char *cont = "Press SPACE for next round";
        DrawText(cont, W / 2 - MeasureText(cont, 18) / 2, H / 2 + 20, 18, WHITE);
    }

    if (g->state == STATE_GAMEOVER) {
        DrawRectangle(0, H / 2 - 60, W, 120, ColorAlpha(BLACK, 0.80f));
        const char *msg;
        Color mc;
        if (g->matchWinner == 1)      { msg = "PLAYER 1 WINS THE MATCH!"; mc = BLUE; }
        else if (g->matchWinner == 2) { msg = "PLAYER 2 WINS THE MATCH!"; mc = GREEN; }
        else                          { msg = "IT'S A TIE!";              mc = YELLOW; }
        DrawText(msg, W / 2 - MeasureText(msg, 32) / 2, H / 2 - 36, 32, mc);
        const char *score = TextFormat("Score: P1 %d  -  P2 %d", g->p1Wins, g->p2Wins);
        DrawText(score, W / 2 - MeasureText(score, 20) / 2, H / 2 + 4, 20, LIGHTGRAY);
        const char *restart = "Press R to play again";
        DrawText(restart, W / 2 - MeasureText(restart, 18) / 2, H / 2 + 36, 18, WHITE);
    }
}
