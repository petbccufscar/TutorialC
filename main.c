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

int main(void)
{
    // Janela
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // Indicador do mouse
    Mouse mouse = newMouse();

    // Blocos e inicialização
    char text_block[MAX_TEXT_BLOCK] = "Teste!"; // Texto usado nos blocos, alterado pela GUI
    bList *blocos = newBList(); 

    // Campos de Bloco e inicialização
    int num_bfields = 0; // Número de campos de bloco no momento
    BlockField bfields[NUM_BLOCK_FIELDS];

    // Spawners de Bloco e inicialização
    // TODO: Aqui vai ser necessário pegar todos os geradores que precisa colocar
    //       calcular a posição de cada um e ai sim colocar no vetor, apenas para
    //       testes aqui    
    int num_bspawners = 0;
    BlockSpawner bspawners[NUM_BLOCK_SPAWNER];
    Block bTeste = newBlock("Hahaha", (Vector2){20, 50});
    spawnBlockSpawner(bspawners, &num_bspawners, bTeste);
    Block bTeste2 = newBlock("Só precisei mudar o código todo", (Vector2){20, 100});
    spawnBlockSpawner(bspawners, &num_bspawners, bTeste2);

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
        // Update
        //----------------------------------------------------------------------------------
        // float deltaTime = GetFrameTime();
        mouse.position = GetMousePosition();

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

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);
            
            // Desenha geradores de blocos
            for (int i = 0; i < num_bspawners; i++) { DrawBlockSpawner(&bspawners[i], &mouse.holding->block, &mouse.hovering->block); }
            // Desenha os blocos
            // for (int i = 0; i < num_blocks; i++) { DrawBlock(&blocks[i], mouse.holding, mouse.hovering); }
            bNode *node = blocos->head;
            while (node != NULL) {
                DrawBlock(&node->block, &mouse.holding->block, &mouse.hovering->block); 
                node = node->next;
            }
            // Desenha os campos
            for (int i = 0; i < num_bfields; i++) { DrawBlockField(&bfields[i]); }

            BeginMode2D(camera);
            // 1. Begin 2D
            // 2. ???
            // 3. Profit
            EndMode2D();

            // Draw Controles GUI e Debugging
            // ------------------------------------------------------------------------------
            // Ver https://raylibtech.itch.io/rguiicons para ID dos ícones
            float posX = screenWidth - 200; float posY = screenHeight - 50;
            float dist_linhas = 25;
            DrawFPS(10, 10);
            // 0
            drawMouseIndicator = GuiCheckBox((Rectangle){ posX, posY, 20, 20}, "Indicador do Mouse", drawMouseIndicator);
            // 1
            posY -= dist_linhas;
            if (GuiButton((Rectangle){posX, posY, 165, 20}, GuiIconText(112, "Criar Campo"))) { 
                spawnBlockField(bfields, &num_bfields);
            }
            // 2
            posY -= dist_linhas;
            GuiTextBox((Rectangle){posX, posY, 100, 20}, text_block, MAX_TEXT_BLOCK, true);
            if (GuiButton((Rectangle){posX + 105, posY, 60, 20}, GuiIconText(112, "Criar"))) { 
                spawnBNode(blocos, text_block, (Vector2){GetScreenWidth()/2 + rand()%50, GetScreenHeight()/2 + rand()%50})->permanent = true;
                // spawnBlockOld(blocks, &num_blocks, text_block, (Vector2){GetScreenWidth()/2 + rand()%50, GetScreenHeight()/2 + rand()%50});
            }
            // 3
            posY -= dist_linhas;
            DrawText("Painel de testes", posX, posY, 10, DARKGRAY);
            // ------------------------------------------------------------------------------

            // Indicador do mouse
            if (drawMouseIndicator){
                float radius = 5.0f;
                if (mouse.hovering != NULL) radius = 8.0f;
                if (mouse.holding != NULL) radius = 4.0f;
                DrawCircleV(mouse.position, radius, LIGHTGRAY);
                DrawCircleV(mouse.position, radius-2, mouse.color);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
