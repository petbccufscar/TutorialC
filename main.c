#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

int main(void)
{
    // Janela
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // Indicador do mouse
    Vector2 mousePosition = {-100.0f, -100.0f};
    Color mouseIndicatorColor = DARKGRAY;

    Vector2 initialMousePosition = {0.0f, 0.0f};

    // Retângulo (Teste)
    bool hover = false;
    bool dragging = false;
    int width = 200;
    int height = 100;
    Vector2 offset = {0.0f, 0.0f};
    Vector2 blockPosition = {((float)GetScreenWidth() - width - 250)/2, (GetScreenHeight() - height)/2.0f};

    // Camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        mousePosition = GetMousePosition();

        Rectangle block = { 
             blockPosition.x,
             blockPosition.y,
            (float)width, 
            (float)height 
        };

        // Checa colisão mouse - bloco
        hover = CheckCollisionPointRec(GetMousePosition(), block);
        if (hover && IsMouseButtonPressed(0)) {
            // Obtem posição do mouse relativa ao retângulo
            printf("DEBUG: BLOCK: %f %f\n", blockPosition.x, blockPosition.y);
            printf("DEBUG: MOUSE: %f %f\n", mousePosition.x, mousePosition.y);
            offset = Vector2Subtract(mousePosition, blockPosition);
            printf("DEBUG: OFFSET: %f %f\n", offset.x, offset.y);
            dragging = true;
        }
        if (dragging && IsMouseButtonDown(0)) {
            blockPosition = Vector2Subtract(mousePosition, offset);
        }
        if (IsMouseButtonReleased(0)) {
            dragging = false;
        };


        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            DrawRectangleRoundedLines(block, 0.1f, 20, 1, DARKGRAY);
            if (dragging) {
                DrawRectangleRounded(block, 0.1f, 20, DARKBLUE);
            } else if (hover) {
                DrawRectangleRounded(block, 0.1f, 20, DARKGRAY);
            };

            DrawCircleV(mousePosition, 5, mouseIndicatorColor);

            BeginMode2D(camera);

            EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
