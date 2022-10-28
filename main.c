#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
// #define RAYGUI_IMPLEMENTATION
// #include "raygui.h"

// Bloco Arrastável
typedef struct Block {
    Rectangle rec;
    bool hover;
    bool dragging;
} Block;

Block new_block(){
    float width = 180.0 - rand() % 80; // TODO: Remover aleatório
    float height = 40.0;
    Rectangle rec = {
        .height = height,
        .width = width,
        .x = ((float)GetScreenWidth() - width - 250)/2,
        .y = (GetScreenHeight() - height)/2.0f
    };
    Block b = {
        .rec = rec,
        .hover = false,
        .dragging = false,
    };
    return b;
}

int main(void)
{
    // Janela
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // Indicador do mouse
    Vector2 mousePosition = {-100.0f, -100.0f};
    Vector2 offset = {0.0f, 0.0f};
    Block *holding = NULL;
    Color mouseIndicatorColor = DARKGRAY;

    Vector2 initialMousePosition = {0.0f, 0.0f};

    // Retângulos (Teste) e inicialização
    const int NUM_BLOCKS = 3;
    Block blocks[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++){
        blocks[i] = new_block();
        blocks[i].rec.x += rand() % 100; //TODO: Remover aleatório
        blocks[i].rec.y += rand() % 100; //TODO: Remover aleatório
    }

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

        // Checa colisão mouse - blocos
        for (int i = 0; i < NUM_BLOCKS; i++){
            Block *b = &blocks[i];
            Vector2 blockPosition = {b->rec.x, b->rec.y};
            b->hover = CheckCollisionPointRec(mousePosition, b->rec);

            if (b->hover && IsMouseButtonPressed(0)) {
            // Obtem posição do mouse relativa ao retângulo
            offset = Vector2Subtract(mousePosition, blockPosition);
            b->dragging = true;
            }
            if (b->dragging && IsMouseButtonDown(0)) {
                blockPosition = Vector2Subtract(mousePosition, offset);
                b->rec.x = blockPosition.x;
                b->rec.y = blockPosition.y;
            }
            if (IsMouseButtonReleased(0)) {
                b->dragging = false;
            };
        }
        // hover = CheckCollisionPointRec(GetMousePosition(), block);
        


        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            
            // Desenha os blocos
            for (int i = 0; i < NUM_BLOCKS; i++){
                Block *b = &blocks[i];
                
                DrawRectangleRoundedLines(b->rec, 0.1f, 20, 1, DARKGRAY);
                if (b->dragging) {
                    DrawRectangleRounded(b->rec, 0.1f, 20, DARKBLUE);
                } else if (b->hover) {
                    DrawRectangleRounded(b->rec, 0.1f, 20, DARKGRAY);
                };
            }
            
            // Indicador do mouse
            DrawCircleV(mousePosition, 5, mouseIndicatorColor);

            BeginMode2D(camera);

            EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
