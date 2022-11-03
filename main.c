#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

// Essa livraria é "intended for tools development"
// https://github.com/raysan5/raygui
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Blocos Arrastáveis
#define NUM_BLOCKS 50 // Máximo de blocos que podem existir
#define MAX_TEXT_BLOCK 30 // Tamanho máximo do texto de cada bloco
#define FONT_SIZE 20 // Tamanho do texto
#define BLOCK_TEXT_PADDING 8 // Padding entre o bloco e o texto no meio
// Campo de Bloco
#define NUM_BLOCK_FIELDS 50 // Máximo de campos de bloco
#define BLOCK_FIELD_PADDING 4 // Padding entre o campo do bloco e o bloco
// Geradores de Blocos
#define NUM_BLOCK_SPAWNER 50 // Máximo de geradores de blocos

// Bloco Arrastável
typedef struct Block {
    char text[MAX_TEXT_BLOCK + 1];
    Rectangle rec;
    bool hover;
    bool dragging;
} Block;

Block newBlock(char text[], Vector2 position) {
    float width = MeasureText(text, FONT_SIZE) + BLOCK_TEXT_PADDING * 2;
    float height = FONT_SIZE + BLOCK_TEXT_PADDING * 2;

    Rectangle rec = {
        .height = height,
        .width = width,
        .x = position.x,
        .y = position.y
        // .x = ,
        // .y = rand() % (int)(GetScreenHeight() - height)
    };
    Block b = {
        .text = "",
        .rec = rec,
        .hover = false,
        .dragging = false
    };
    strcpy(b.text, text);

    return b;
}

bool spawnBlock(Block *arr, int *num_blocks, char text[], Vector2 position) {
    if (*num_blocks >= NUM_BLOCKS) {
        printf("Não foi possível gerar um novo bloco, máximo (%d) atingido\n", NUM_BLOCKS);
        return false; // Não é possível gerar mais blocos
    } else {
        arr[*num_blocks] = newBlock(text, position);
        *num_blocks += 1;
        return true;
    }
}

void DrawBlock(Block *b, Block *holding, Block *hovering) {
    Color textColor = MAROON;
    int segments = 50; float roundness = 0.4f; float lineThick = 2.0f;
    DrawRectangleRoundedLines(b->rec, roundness, segments, lineThick, DARKGRAY); // Outline
    DrawRectangleRounded(b->rec, roundness, segments, LIGHTGRAY); // Preenchimento sólido
    if (holding == b) {
        DrawRectangleRounded(b->rec, roundness, segments, MAROON); // Preenchimento
        textColor = WHITE;
    } else if (hovering == b) {
        DrawRectangleRounded(b->rec, roundness, segments, DARKGRAY); // Preenchimento
        textColor = WHITE;
    };
    DrawText(b->text, (int)b->rec.x + BLOCK_TEXT_PADDING, (int)b->rec.y + BLOCK_TEXT_PADDING, FONT_SIZE, textColor); // Texto
}

// Gerador de Bloco
typedef struct BlockSpawner {
    Block base;
    Block *block;
} BlockSpawner;

BlockSpawner newBlockSpawner(Block base) {
    BlockSpawner bs = {
        .base = base,
        .block = NULL,
    };
    return bs;
}

bool spawnBlockSpawner(BlockSpawner *arr, int *num_bspawners, Block base) {
    if (*num_bspawners >= NUM_BLOCK_SPAWNER) {
        printf("Não foi possível gerar um novo gerador de blocos, máximo (%d) atingido\n", NUM_BLOCK_SPAWNER);
        return false; // Não é possível gerar mais blocos
    } else {
        arr[*num_bspawners] = newBlockSpawner(base);
        *num_bspawners += 1;
        return true;
    }
}

void DrawBlockSpawner(BlockSpawner *bf, Block *holding, Block *hovering) {
    DrawBlock(&bf->base, holding, hovering);
}

// Campo de bloco
typedef struct BlockField {
    Block *block; 
    Rectangle rec;
} BlockField;

BlockField newBlockField() {
    // Dimensões iniciais
    float width, height;
    height = width = FONT_SIZE + (BLOCK_FIELD_PADDING + BLOCK_TEXT_PADDING) * 2;

    Rectangle rec = {
        .height = height,
        .width = width,
        .x = rand() % (int)(GetScreenWidth() - width),
        .y = rand() % (int)(GetScreenHeight() - height)
    };
    BlockField bf = {
        .block = NULL,
        .rec = rec
    };

    return bf;
}

bool spawnBlockField(BlockField *arr, int *num_bfields) {
    if(*num_bfields >= NUM_BLOCK_FIELDS) {
        printf("Não foi possível gerar um novo campo, máximo (%d) atingido\n", NUM_BLOCK_FIELDS);
        return false; // Não é possível gerar um novo campo
    } else {
        arr[*num_bfields] = newBlockField();
        *num_bfields += 1;
        return true;
    }
}

void DrawBlockField(BlockField *bf) {
    int segments = 50; float roundness = 0.4f; float lineThick = 1.0f;
    DrawRectangleRoundedLines(bf->rec, roundness, segments, lineThick, MAROON);
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
    Color mouseIndicatorColor = DARKPURPLE;

    Vector2 initialMousePosition = GetMousePosition();

    // Blocos e inicialização
    int num_blocks = 0; // Número de blocos no momento
    char text_block[MAX_TEXT_BLOCK] = "Teste!";
    Block blocks[NUM_BLOCKS];

    // Campos de Bloco e inicialização
    int num_bfields = 0; // Número de campos de bloco no momento
    BlockField bfields[NUM_BLOCK_FIELDS];

    // Spawners de Bloco e inicialização
    // TODO: Aqui vai ser necessário pegar todos os geradores que precisa colocar
    //       calcular a posição de cada um e ai sim colocar no vetor, apenas para
    //       testes aqui    
    int num_bspawner = 0;
    BlockSpawner bspawners[NUM_BLOCK_SPAWNER];
    Block bTeste = newBlock("Teste", (Vector2){20, 50});
    spawnBlockSpawner(bspawners, &num_bspawner, bTeste);
    Block bTeste2 = newBlock("Teste 2", (Vector2){20, 100});
    spawnBlockSpawner(bspawners, &num_bspawner, bTeste2);

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
        float deltaTime = GetFrameTime();
        mousePosition = GetMousePosition();

        // Update da colisão entre Mouse / Campos de Bloco
        // Quando um mouse está segurando um bloco e para em cima de um campo, esse
        // campo deve acomodar o bloco. 
        // TODO: Potencialmente alterar esse comportamento pro mouse não precisar estar
        //       exatamente em cima, ou uma colisão levando em conta o rec do bloco tbm
        for (int i = 0; i < num_bfields; i++) {
            BlockField *bf = &bfields[i];
            if ((bf->block == NULL || bf->block == holding) && holding != NULL && CheckCollisionPointRec(mousePosition, bf->rec) && IsMouseButtonReleased(0)) {
                    // É importante que essa parte de código seja executada antes do `holding = NULL;` lá embaixo
                    // TODO: Refatorar isso? Updates do mouse antes de tudo talvez, atualizar holding, hovering depois de tudo
                    bf->block = holding;
                    // Move o bloco para o campo
                    bf->block->rec.x = bf->rec.x + BLOCK_FIELD_PADDING;
                    bf->block->rec.y = bf->rec.y + BLOCK_FIELD_PADDING;
                    // Ajusta tamanho do campo
                    bf->rec.width = bf->block->rec.width  + BLOCK_FIELD_PADDING * 2;
                }
            if (bf->block != NULL) {
                // O bloco está no lugar certo? Se o botão do mouse estiver pressionado, esperamos o usuário terminar a ação dele
                if ((bf->block->rec.x != bf->rec.x + BLOCK_FIELD_PADDING || 
                    bf->block->rec.y != bf->rec.y + BLOCK_FIELD_PADDING) &&
                    !IsMouseButtonDown(0)) 
                {
                    bf->block = NULL;
                    bf->rec.width = bf->rec.height;
                }
            }
        }

        // Update da colisão entre Mouse / Blocos
        // TODO: Consertar a seleção de blocos que são desenhados em cima do outro, mas que não seguem essa ordem para seleção
        for (int i = 0; i < num_blocks; i++) {
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

        // Update da colisão Mouse / Gerador de Blocos
        for (int i = 0; i < num_bspawner; i++) {
            BlockSpawner *bs = &bspawners[i];
            Block *base = &bs->base;
            Vector2 basePosition = {base->rec.x, base->rec.y};

            base->hover = CheckCollisionPointRec(mousePosition, base->rec);
            if (base->hover && hovering == NULL) {
                hovering = base;
            } else if (hovering == base && !base->hover) {
                hovering = NULL;
            }

            if (hovering == base && IsMouseButtonPressed(0) && holding == NULL) {
                // Obtem posição do mouse relativa ao retângulo
                offset = Vector2Subtract(mousePosition, basePosition);
                // Cria um novo bloco
                holding = base;
                base->dragging = true;
            }
            if (base->dragging && IsMouseButtonDown(0)) {
                // Usando o offset, move o bloco selecionado
                basePosition = Vector2Subtract(mousePosition, offset);
                base->rec.x = basePosition.x;
                base->rec.y = basePosition.y;
            }
            if (IsMouseButtonReleased(0)) {
                holding = NULL;
                base->dragging = false;
            };
        }

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
            
            // Desenha os blocos
            for (int i = 0; i < num_blocks; i++) {
                DrawBlock(&blocks[i], holding, hovering);
            }

            // Desenha os campos
            for (int i = 0; i < num_bfields; i++) {
                DrawBlockField(&bfields[i]);
            }

            // Desenha geradores de blocos
            for (int i = 0; i < num_bspawner; i++) {
                DrawBlockSpawner(&bspawners[i], holding, hovering);
            }

            BeginMode2D(camera);


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
                spawnBlock(blocks, &num_blocks, text_block, (Vector2){rand() % (int)(GetScreenWidth()-50), rand() % (int)(GetScreenWidth()-50)});
            }
            // 3
            posY -= dist_linhas;
            DrawText("Painel de testes", posX, posY, 10, DARKGRAY);
            // ------------------------------------------------------------------------------

            // Indicador do mouse
            if (drawMouseIndicator){
                float radius = 5.0f;
                if (hovering != NULL) radius = 8.0f;
                if (holding != NULL) radius = 4.0f;
                DrawCircleV(mousePosition, radius, LIGHTGRAY);
                DrawCircleV(mousePosition, radius-2, mouseIndicatorColor);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();

    return 0;
}
