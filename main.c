#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void) {
    InitWindow(MAP_COLS * TILE_SIZE, MAP_ROWS * TILE_SIZE, "ASCII Bomberman");
    SetTargetFPS(60);

    Game g;
    Game_Init(&g);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Game_Update(&g, dt);

        BeginDrawing();
        if (g.state == STATE_MENU) {
            Render_Menu();
        } else {
            Render_Game(&g);
            Render_HUD(&g);
            Render_Overlay(&g);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
