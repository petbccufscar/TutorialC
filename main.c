#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "raylib.h"
#include "raymath.h"
#include "puzzle.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
// Pragma pra ignorar warnings da biblioteca
#define RAYGUI_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wpedantic"
#include "raygui.h"
#pragma GCC diagnostic pop

#define MAX_FONTS 2

int main(void)
{
    // Janela
    const int screenWidth = 1500;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Font fonts[MAX_FONTS] = { 0 }; 

    // fonts[0] = LoadFontEx("resources/fonts/unifont.ttf", 30, 0, 500);
    fonts[0] = LoadFont("resources/fonts/Ticketing.fnt");

    // Indicador do mouse
    Mouse mouse = newMouse();

    // Blocos e inicialização
    char text_block[MAX_TEXT_SIZE] = "Teste"; // Texto usado nos blocos, alterado pela GUI
    bList *blocos = newBList(); 

    // Campos de Bloco e inicialização
    int num_bfields = 0; // Número de campos de bloco no momento
    BlockField bfields[NUM_BLOCK_FIELDS];

    /**============================================
     **               Puzzles
     *=============================================**/
    // TODO: Inicializações de teste abaixo, vamos usar um arquivo depois
    // Spawners de Bloco e inicialização
    int num_bspawners = 0;
    BlockSpawner bspawners[NUM_BLOCK_SPAWNER];
    Block bTeste = newBlock("Hahaha", (Vector2){20, 50});
    spawnBlockSpawnerOld(bspawners, &num_bspawners, bTeste);
    Block bTeste2 = newBlock("Só precisei mudar o código todo", (Vector2){20, 100});
    spawnBlockSpawnerOld(bspawners, &num_bspawners, bTeste2);

    // Inicialização manual de um CodePuzzle
    CodePuzzle cp = newCodePuzzle();
    spawnElementTxt(&cp, "Teste!");
    spawnElementBf(&cp);
    spawnElementTxt(&cp, "Ok");

    spawnBlockSpawner(&cp, "Sim");
    spawnBlockSpawner(&cp, "Não");

    // Camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    // Parâmetros dos controles 
    bool drawMouseIndicator = false;

    // Main game loop
    while (!WindowShouldClose())
    {
    /**========================================================================
     **                           UPDATE
     *========================================================================**/
        // float deltaTime = GetFrameTime();
        mouse.position = GetMousePosition();

        //! A ordem do update importa
        updateGeradores(&mouse, blocos, bspawners, &num_bspawners);
        updateCampos(&mouse, bfields, &num_bfields);
        updateBNodes(&mouse, blocos);

        // Update da Cãmera
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        // Update do Mouse
        if (drawMouseIndicator) {
            DisableCursor();
        } else {
            EnableCursor();
        }

    /**========================================================================
     **                           DRAW
     *========================================================================**/
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            
            // Desenha geradores de blocos
            for (int i = 0; i < num_bspawners; i++) { DrawBlockSpawner(&bspawners[i], &mouse); }

            // Desenha os blocos
            bNode *node = blocos->head;
            while (node != NULL) {
                DrawBlock(&node->block, &mouse); 
                node = node->next;
            }

            // Desenha os campos
            for (int i = 0; i < num_bfields; i++) { DrawBlockField(&bfields[i]); }

            // Desenha o CodePuzzle
            DrawCodePuzzle(&cp, &mouse);

            BeginMode2D(camera);
            // 1. Begin 2D
            // 2. ???
            // 3. Profit
            EndMode2D();

        /**============================================
         ** DRAW:   Controles GUI e Debugging
         * Ver https://raylibtech.itch.io/rguiicons 
         * para ID dos ícones
         *=============================================**/
                float posX = screenWidth - 200; float posY = screenHeight - 50;
                float dist_linhas = 25;
                DrawFPS(10, 10);
                
                // Linha 0 ===
                drawMouseIndicator = GuiCheckBox((Rectangle){ posX, posY, 20, 20}, "Indicador do Mouse", drawMouseIndicator);

                // Linha 1 ===
                posY -= dist_linhas;
                if (GuiButton((Rectangle){posX, posY, 165, 20}, GuiIconText(112, "Criar Campo"))) { 
                    spawnBlockField(bfields, &num_bfields);
                }

                // Linha 2 ===
                posY -= dist_linhas;
                GuiTextBox((Rectangle){posX, posY, 100, 20}, text_block, MAX_TEXT_SIZE, true);
                if (GuiButton((Rectangle){posX + 105, posY, 60, 20}, GuiIconText(112, "Criar"))) { 
                    spawnBNode(blocos, text_block, (Vector2){GetScreenWidth()/2 + rand()%50, GetScreenHeight()/2 + rand()%50})->permanent = true;
                    // spawnBlockOld(blocks, &num_blocks, text_block, (Vector2){GetScreenWidth()/2 + rand()%50, GetScreenHeight()/2 + rand()%50});
                }

                // Linha 3 ===
                posY -= dist_linhas;
                DrawText("Painel de testes", posX, posY, 10, DARKGRAY);

        /**============================================
         ** DRAW:    Indicador do Mouse
         *=============================================**/
            if (drawMouseIndicator){
                float radius = 5.0f;
                if (mouse.hovering != NULL) radius = 8.0f;
                if (mouse.holding != NULL) radius = 4.0f;
                DrawCircleV(mouse.position, radius, LIGHTGRAY);
                DrawCircleV(mouse.position, radius-2, mouse.color);
            }

        // TODO: Teste de fonte, remover depois
        DrawTextEx(fonts[0], "typedef enum ElementType {text, field} ElementType;", (Vector2){300.0f, 300.0f}, (float)fonts[0].baseSize, 2, MAROON);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
