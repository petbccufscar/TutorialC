#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Máximo de blocos que podem existir
#define NUM_BLOCKS 50
// Tamanho máximo do texto de cada bloco
#define MAX_TEXT_BLOCK 20

// Bloco Arrastável
typedef struct Block {
    char text[MAX_TEXT_BLOCK + 1];
    Rectangle rec;
    bool hover;
    bool dragging;
} Block;

Block new_block(char text[]){
    float width = 180.0 - rand() % 80; // TODO: Remover aleatório
    float height = 40.0;
    printf("%s\n", text);
    Rectangle rec = {
        .height = height,
        .width = width,
        .x = rand() % (int)(GetScreenWidth() - width),
        .y = rand() % (int)(GetScreenHeight() - height)
    };
    Block b = {
        .text = "",
        .rec = rec,
        .hover = false,
        .dragging = false,
    };
    strcpy(b.text, text);
    return b;
}

bool spawnBlock(Block *arr, int *num_blocks, char text[]){
    if (*num_blocks >= NUM_BLOCKS) {
        return false; // Não é possível gerar mais blocos
    } else {
        arr[*num_blocks] = new_block(text);
        *num_blocks += 1;
        return true;
    }
}

void DrawBlock(Block *b, Block *holding, Block *hovering){
    Color textColor = MAROON;
    DrawRectangleRoundedLines(b->rec, 0.1f, 20, 1, DARKGRAY); // Outline
    if (holding == b) {
        DrawRectangleRounded(b->rec, 0.1f, 20, DARKBLUE); // Preenchimento
        textColor = WHITE;
    } else if (hovering == b) {
        DrawRectangleRounded(b->rec, 0.1f, 20, DARKGRAY); // Preenchimento
        textColor = WHITE;
    };
    DrawText(b->text, (int)b->rec.x + 5, (int)b->rec.y + 8, 20, textColor); // Texto
}

int main(void)
{
    // Janela
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // Indicador do mouse
    Vector2 mousePosition = {-100.0f, -100.0f};
    Vector2 offset = {0.0f, 0.0f};
    Block *holding = NULL;
    Block *hovering = NULL;
    Color mouseIndicatorColor = DARKGRAY;

    Vector2 initialMousePosition = {0.0f, 0.0f};

    // Retângulos (Teste) e inicialização
    int num_blocks = 0; // Número de blocos no momento
    char text_block[MAX_TEXT_BLOCK] = "Alou";
    Block blocks[NUM_BLOCKS];

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
        for (int i = 0; i < num_blocks; i++){
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
            for (int i = 0; i < num_blocks; i++){
                DrawBlock(&blocks[i], holding, hovering);
            }
            
            // Indicador do mouse
            if (drawMouseIndicator) DrawCircleV(mousePosition, 5, mouseIndicatorColor);

            BeginMode2D(camera);

            EndMode2D();

            // Draw Controles GUI e Debugging
            // ------------------------------------------------------------------------------
                // Ver https://raylibtech.itch.io/rguiicons para ID dos ícones
            DrawFPS(10, 10);
            drawMouseIndicator = GuiCheckBox((Rectangle){ screenWidth - 200, screenHeight - 50, 20, 20}, "Indicador do Mouse", drawMouseIndicator);
            GuiTextBox((Rectangle){screenWidth - 200, screenHeight - 80, 100, 20}, text_block, MAX_TEXT_BLOCK, true);
            if (GuiButton((Rectangle){screenWidth - 95, screenHeight - 80, 60, 20}, GuiIconText(112, "Criar"))) { 
                spawnBlock(blocks, &num_blocks, text_block);
            }
            // ------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
