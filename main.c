#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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
    Block *hovering = NULL;
    Color mouseIndicatorColor = DARKGRAY;

    Vector2 initialMousePosition = {0.0f, 0.0f};

    // Retângulos (Teste) e inicialização
    const int NUM_BLOCKS = 10;
    Block blocks[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++){
        blocks[i] = new_block();
        blocks[i].rec.x = rand() % (int)(GetScreenWidth() - blocks[i].rec.width); //TODO: Remover aleatório
        blocks[i].rec.y = rand() % (int)(GetScreenHeight() - blocks[i].rec.height); //TODO: Remover aleatório
    }

    // Camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    // Parâmetros dos controles 
    bool drawMouseIndicator = true;

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        mousePosition = GetMousePosition();

        // Update da colisão entre Mouse / Blocos
        for (int i = 0; i < NUM_BLOCKS; i++){
            Block *b = &blocks[i];
            Vector2 blockPosition = {b->rec.x, b->rec.y};

            b->hover = CheckCollisionPointRec(mousePosition, b->rec);
            if (b->hover && hovering == NULL) {
                hovering = b;
            } else if (hovering == b && !b->hover) {
                hovering = NULL;
            }

            if (hovering == b && IsMouseButtonPressed(0) && holding == NULL) {
                // Obtem posição do mouse relativa ao retângulo
                offset = Vector2Subtract(mousePosition, blockPosition);
                holding = b;
                b->dragging = true;
            }
            if (b->dragging && IsMouseButtonDown(0)) {
                // Usando o offset, move o bloco selecionado
                blockPosition = Vector2Subtract(mousePosition, offset);
                b->rec.x = blockPosition.x;
                b->rec.y = blockPosition.y;
            }
            if (IsMouseButtonReleased(0)) {
                holding = NULL;
                b->dragging = false;
            };
        }

        // Update da Cãmera
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
                
                DrawRectangleRoundedLines(b->rec, 0.1f, 20, 1, DARKGRAY); // Outline
                if (holding == b) {
                    DrawRectangleRounded(b->rec, 0.1f, 20, DARKBLUE); // Preenchimento
                } else if (hovering == b) {
                    DrawRectangleRounded(b->rec, 0.1f, 20, DARKGRAY); // Preenchimento
                };
            }
            
            // Indicador do mouse
            if (drawMouseIndicator) DrawCircleV(mousePosition, 5, mouseIndicatorColor);

            BeginMode2D(camera);

            EndMode2D();

            // Draw Controles GUI
            // ------------------------------------------------------------------------------
            drawMouseIndicator = GuiCheckBox((Rectangle){ 640, 380, 20, 20}, "Indicador do Mouse", drawMouseIndicator);
            // ------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
