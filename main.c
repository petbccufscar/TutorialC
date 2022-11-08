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

// Precisamos de uma estrutura que permita remoção de blocos fora de ordem
// Como não teremos blocos sobrevivendo "sozinhos" (Sempre estarão ou acoplados
// a um gerador ou campo) acho que não vai causar muitos problemas... espero
typedef struct bNode { // Block Node
    Block block;
    struct bNode *next;
    struct bNode *prev;
} bNode;

typedef struct bList {
    bNode *head;
    bNode *end;
    size_t size;
} bList;

bList* newBList() {
    bList *new = malloc(sizeof (bList));
    new->end = NULL;
    new->head = NULL;
    new->size = 0;
    return new;
}

bNode* newBNode(bNode *prev, Block b, bNode *next) {
    bNode *new = malloc(sizeof (bNode));
    new->block = b;
    new->prev = prev;
    new->next = next;
    return new;
}

// Remove nó com base no seu ponteiro
bool removeBNode(bList *list, bNode *node) {
    if (list->head == node && list->end == node) {
    // Único nó da lista
        list->end = NULL;
        list->head = NULL;
        list->size = 0;
        free(node);
    } else if (list->end == node) {
    // Último nó da lista
        node->prev->next = NULL;
        list->end = node->prev;
        list->size--;
        free(node);
    } else if (list->head == node) {
    // Primeiro nó da lista
        node->next->prev = NULL;
        list->head = node->next;
        list->size--;
        free(node);
    } else {
    // Nó no meio da lista
        node->prev->next = node->next;
        node->next->prev = node->prev;
        list->size--;
        free(node);
    }
    return true;
}

// Adiciona um nó na lista (Final da lista)
bNode* addBNode(bList *list, Block b) {
    // Lista vazia
    if (list->head == NULL) {
        bNode *new = newBNode(NULL, b, NULL);
        list->head = new;
        list->end = new;
        list->size++;
    } else {
        bNode *new = newBNode(NULL, b, NULL);
        list->end->next = new;
        list->end = new;
        list->size++;
    }
}

bNode* spawnBNode(bList *list, char text[], Vector2 position) {
    Block b = newBlock(text, position);
    return addBNode(list, b);
}

// Usar spawnBNode no lugar
Block* spawnBlockOld(Block *arr, int *num_blocks, char text[], Vector2 position) {
    if (*num_blocks >= NUM_BLOCKS) {
        printf("Não foi possível gerar um novo bloco, máximo (%d) atingido\n", NUM_BLOCKS);
        return NULL; // Não é possível gerar mais blocos
    } else {
        arr[*num_blocks] = newBlock(text, position);
        return &arr[(*num_blocks)++]; // Retorna num_blocks e depois aumenta o valor
    }
}

void DrawBlock(Block *b, Block *holding, Block *hovering) {
    Color textColor = MAROON;
    int segments = 50; float roundness = 0.4f; float lineThick = 2.0f;
    DrawRectangleRoundedLines(b->rec, roundness, segments, lineThick, DARKGRAY); // Outline
    DrawRectangleRounded(b->rec, roundness, segments, LIGHTGRAY); // Preenchimento sólido
    if (holding == b) {
        DrawRectangleRounded(b->rec, roundness, segments, MAROON); // Preenchimento segurando
        textColor = WHITE;
    } else if (hovering == b) {
        DrawRectangleRounded(b->rec, roundness, segments, DARKGRAY); // Preenchimento hovering
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

typedef struct Mouse {
    Vector2 position;
    Vector2 offset;
    Block *holding;
    Block *hovering;
    Color color;
} Mouse;

Mouse newMouse() {
    Mouse m = {
        .position = {-100.0f, -100.0f},
        .offset = {0.0f, 0.0f},
        .holding = NULL,
        .hovering = NULL,
        .color = MAROON
    };
    return m;
}

// Funções de Atualização
// Update da colisão entre Mouse / Campos de Bloco
void updateMouseCampos(Mouse *mouse, BlockField bfields[], int *num_bfields) {
    // Quando um mouse está segurando um bloco e para em cima de um campo, esse
    // campo deve acomodar o bloco. 

    // Um bloco acabou de ser solto?
    if (IsMouseButtonReleased(0)) {
        for (int i = 0; i < *num_bfields; i++) {
            // Para cada campo, verificar se foi solto na sua posição
            BlockField *bf = &bfields[i];
            if (CheckCollisionPointRec(mouse->position, bf->rec)
                && mouse->holding != NULL
                && bf->block == NULL) {
                bf->block = mouse->holding;
                // Move bloco para o campo
                bf->block->rec.x = bf->rec.x + BLOCK_FIELD_PADDING;
                bf->block->rec.y = bf->rec.y + BLOCK_FIELD_PADDING;
                // Ajusta o tamanho do campo
                bf->rec.width = bf->block->rec.width  + BLOCK_FIELD_PADDING * 2;
                // Retira dragging
                bf->block->dragging = false;
            } else if (mouse->holding == bf->block) {
                bf->block = NULL;
                bf->rec.width = bf->rec.height;
            }
        }
    }
}

// Update da colisão Mouse / Gerador de Blocos
void updateMouseGeradores(Mouse *mouse, Block blocks[], int *num_blocks, BlockSpawner bspawners[], int *num_bspawners) {
    for (int i = 0; i < *num_bspawners; i++) {
        BlockSpawner *bs = &bspawners[i];
        Block *base = &bs->base;
        Vector2 basePosition = {base->rec.x, base->rec.y};

        // Hover
        base->hover = CheckCollisionPointRec(mouse->position, base->rec);
        if (base->hover && mouse->hovering == NULL) {
            mouse->hovering = base;
        } else if (mouse->hovering == base && !base->hover) {
            mouse->hovering = NULL;
        }

        // Holding e Dragging
        if (mouse->hovering == base && IsMouseButtonPressed(0) && mouse->holding == NULL) {
            // Obtem posição do mouse relativa ao retângulo
            mouse->offset = Vector2Subtract(mouse->position, basePosition);
            // Cria um novo bloco
            bs->block = spawnBlockOld(blocks, num_blocks, base->text, Vector2Add(basePosition, (Vector2){20,20}));
            if (bs->block != NULL) {
                mouse->holding = bs->block;
                bs->block->dragging = true;
            }
        }
    }
}

// Update da colisão entre Mouse / Blocos
void updateMouseBlocos(Mouse *mouse, bList *list) {
    bNode *lastHover = NULL;

    bool mouseReleased = IsMouseButtonReleased(0);
    if (mouseReleased) {
        mouse->holding = NULL;
    }

    bNode *n = list->head; // Nó atual
    while(n != NULL) {
        Block *b = &n->block;
        Vector2 blockPosition = {b->rec.x, b->rec.y};

        // Ao fim do loop, deixa em lastHover o bloco que deve ser selecionado
        b->hover = CheckCollisionPointRec(mouse->position, b->rec);
        if (b->hover && mouse->holding == NULL) {
            lastHover = n;
            mouse->hovering = b;
        } else if (mouse->hovering == b && !b->hover) {
            lastHover = NULL;
            mouse->hovering = NULL;
        }

        // Se o bloco está sendo arrastado, mover
        if (b->dragging) {
            // Usando o offset, move o bloco selecionado
            blockPosition = Vector2Subtract(mouse->position, mouse->offset);
            b->rec.x = blockPosition.x;
            b->rec.y = blockPosition.y;
        }
        n = n->next;

        // Se o mouse foi solto, resetar todos os blocos
        if (mouseReleased) {
            b->dragging = false;
        }
    }

    if (lastHover != NULL) {
        Block *b = &lastHover->block;
        if (IsMouseButtonPressed(0) && mouse->holding == NULL) {
            // Obterm posição do mouse relativa ao retângulo
            Vector2 blockPosition = (Vector2){b->rec.x, b->rec.y};
            mouse->offset = Vector2Subtract(mouse->position, blockPosition);
            mouse->holding = b;
            b->dragging = true;
        }
    }
}

// Usar updateMouseBlocos no lugar
void updateMouseBlocosOld(Mouse *mouse, Block blocks[], int *num_blocks) {
    // Update da colisão entre Mouse / Blocos
    int nHover = -1;

    bool mouseSolto = IsMouseButtonReleased(0);
    if (mouseSolto) {
        mouse->holding = NULL;
    }

    for (int i = 0; i < *num_blocks; i++) {
        Block *b = &blocks[i];
        Vector2 blockPosition = {b->rec.x, b->rec.y};

        // Determinando qual o bloco selecionado
        b->hover = CheckCollisionPointRec(mouse->position, b->rec);
        if (b->hover && mouse->holding == NULL) {
            nHover = i; // Marcamos o bloco mais a frente que colide com o mouse
            mouse->hovering = b;
        } else if (mouse->hovering == b && !b->hover) {
            nHover = -1;
            mouse->hovering = NULL;
        }

        // Se o bloco está sendo arrastado, mover
        if (b->dragging) {
            // Usando o offset, move o bloco selecionado
            blockPosition = Vector2Subtract(mouse->position, mouse->offset);
            b->rec.x = blockPosition.x;
            b->rec.y = blockPosition.y;
        }

        // Se o mouse foi solto, resetar todos os blocos
        if (mouseSolto) {
            b->dragging = false;
        }
    }

    if (nHover != -1) {
        Block *b = &blocks[nHover]; // bloco selecionado
        if (IsMouseButtonPressed(0) && mouse->holding == NULL) {
            // Obtem posição do mouse relativa ao retângulo
            Vector2 blockPosition = (Vector2){b->rec.x, b->rec.y};
            mouse->offset = Vector2Subtract(mouse->position, blockPosition);
            mouse->holding = b;
            b->dragging = true;
        }
    }
}

int main(void)
{
    // Janela
    const int screenWidth = 1000;
    const int screenHeight = 650;

    InitWindow(screenWidth, screenHeight, "Tutorial C");

    // Indicador do mouse
    Mouse mouse = newMouse();

    // Blocos e inicialização
    int num_blocks = 0; // Número de blocos no momento
    char text_block[MAX_TEXT_BLOCK] = "Teste!"; // Texto usado nos blocos, alterado pela GUI
    Block blocks[NUM_BLOCKS]; // TODO: Acho que vai precisar escrever uma lista ligada pra permitir a remoção de blocos... trabaio ein
    bList *blocos = newBList(); // TODO: Nova lista ligada, remover aos poucos a antiga

    // Campos de Bloco e inicialização
    int num_bfields = 0; // Número de campos de bloco no momento
    BlockField bfields[NUM_BLOCK_FIELDS];

    // Spawners de Bloco e inicialização
    // TODO: Aqui vai ser necessário pegar todos os geradores que precisa colocar
    //       calcular a posição de cada um e ai sim colocar no vetor, apenas para
    //       testes aqui    
    int num_bspawners = 0;
    BlockSpawner bspawners[NUM_BLOCK_SPAWNER];
    Block bTeste = newBlock("Teste", (Vector2){20, 50});
    spawnBlockSpawner(bspawners, &num_bspawners, bTeste);
    Block bTeste2 = newBlock("Teste 2", (Vector2){20, 100});
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
        float deltaTime = GetFrameTime();
        mouse.position = GetMousePosition();

        updateMouseCampos(&mouse, bfields, &num_bfields);
        updateMouseGeradores(&mouse, blocks, &num_blocks, bspawners, &num_bspawners);
        // updateMouseBlocosOld(&mouse, blocks, &num_blocks);
        updateMouseBlocos(&mouse, blocos);

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
            for (int i = 0; i < num_bspawners; i++) { DrawBlockSpawner(&bspawners[i], mouse.holding, mouse.hovering); }
            // Desenha os blocos
            // for (int i = 0; i < num_blocks; i++) { DrawBlock(&blocks[i], mouse.holding, mouse.hovering); }
            bNode *node = blocos->head;
            while (node != NULL) {
                DrawBlock(&node->block, mouse.holding, mouse.hovering); 
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
                spawnBNode(blocos, text_block, (Vector2){GetScreenWidth()/2 + rand()%50, GetScreenHeight()/2 + rand()%50});
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
