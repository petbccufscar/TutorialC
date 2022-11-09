#include "puzzle.h"

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
    new->owned = true;
    new->permanent = false;
    return new;
}

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

// Remove nó com base no seu ponteiro
bool removeBNode(Mouse *mouse, bList *list, bNode *node) {
    // Removendo referências que o mouse pode ter do node
    if (mouse->holding == node) { mouse->holding = NULL; }
    if (mouse->hovering == node) { mouse->hovering = NULL; }

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
    bNode *new = newBNode(NULL, b, NULL);
    if (list->head == NULL) {
        list->head = new;
        list->end = new;
        list->size++;
    } else {
        new->prev = list->end;
        list->end->next = new;
        list->end = new;
        list->size++;
    }
    return new;
}

BlockSpawner newBlockSpawner(Block base) {
    BlockSpawner bs = {
        .base = (bNode){
            base,
            NULL,
            NULL,
            true,
            true
        },
        .node = NULL,
    };
    return bs;
}

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
        .node = NULL,
        .rec = rec
    };

    return bf;
}

bNode* spawnBNode(bList *list, char text[], Vector2 position) {
    Block b = newBlock(text, position);
    return addBNode(list, b);
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


// TODO: Alterar pra receber bNodes ao invés de Blocks
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

// TODO: Alterar pra receber bNodes ao invés de Blocks
void DrawBlockSpawner(BlockSpawner *bf, Block *holding, Block *hovering) {
    DrawBlock(&bf->base.block, holding, hovering);
}

void DrawBlockField(BlockField *bf) {
    int segments = 50; float roundness = 0.4f; float lineThick = 1.0f;
    DrawRectangleRoundedLines(bf->rec, roundness, segments, lineThick, MAROON);
}


// Update da colisão entre Mouse / Campos de Bloco
void updateCampos(Mouse *mouse, BlockField bfields[], int *num_bfields) {
    // Quando um mouse está segurando um bloco e para em cima de um campo, esse
    // campo deve acomodar o bloco. 

    // Um bloco acabou de ser solto?
    if (IsMouseButtonReleased(0)) {
        for (int i = 0; i < *num_bfields; i++) {
            // Para cada campo, verificar se foi solto na sua posição
            BlockField *bf = &bfields[i];
            if (CheckCollisionPointRec(mouse->position, bf->rec)
                && mouse->holding != NULL
                && (bf->node == NULL || bf->node == mouse->holding)) {
                bf->node = mouse->holding;
                // Move bloco para o campo
                bf->node->block.rec.x = bf->rec.x + BLOCK_FIELD_PADDING;
                bf->node->block.rec.y = bf->rec.y + BLOCK_FIELD_PADDING;
                // Ajusta o tamanho do campo
                bf->rec.width = bf->node->block.rec.width  + BLOCK_FIELD_PADDING * 2;
                // Retira dragging
                bf->node->block.dragging = false;
                bf->node->owned = true;
            } else if (mouse->holding == bf->node) {
                if (bf->node != NULL){
                    bf->node = NULL;
                }
                bf->rec.width = bf->rec.height;
            }
        }
    }

    for (int i = 0; i < *num_bfields; i++) {
        BlockField *bf = &bfields[i];
        if (bf->node != NULL) {
            bf->node->owned = true;
        }
    }
}

// Update dos Geradores de Bloco
void updateGeradores(Mouse *mouse, bList *list, BlockSpawner bspawners[], int *num_bspawners) {
    for (int i = 0; i < *num_bspawners; i++) {
        BlockSpawner *bs = &bspawners[i];
        bNode *base = &bs->base;
        Vector2 basePosition = {base->block.rec.x, base->block.rec.y};

        // Hover
        base->block.hover = CheckCollisionPointRec(mouse->position, base->block.rec);
        if (base->block.hover && mouse->hovering == NULL) {
            mouse->hovering = base;
        } else if (mouse->hovering == base && !base->block.hover) {
            mouse->hovering = NULL;
        }

        // Holding e Dragging
        if (mouse->hovering == base && IsMouseButtonPressed(0) && mouse->holding == NULL) {
            // Obtem posição do mouse relativa ao retângulo
            mouse->offset = Vector2Subtract(mouse->position, basePosition);
            // Cria um novo bloco
            bs->node = spawnBNode(list, base->block.text, basePosition);
            if (bs->node != NULL) {
                mouse->hovering = bs->node;
                mouse->holding = bs->node;
                bs->node->block.dragging = true;
            }
        }    

        if (bs->node != NULL && mouse->holding == bs->node) {
            bs->node->owned = true;
        }    
    }
}

// Update da colisão entre Mouse / Blocos
void updateBNodes(Mouse *mouse, bList *list) {
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
            mouse->hovering = n;
        } else if (mouse->hovering == n && !b->hover) {
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

        // Se o mouse foi solto, resetar todos os blocos
        if (mouseReleased) {
            b->dragging = false;
        }

        if (!n->owned) {
            removeBNode(mouse, list, n);
        } else if (!n->permanent) {
            // Exige que se confirme um dono pra próxima vez
            // Os updates anteriores devem tornar owned=true a cada iteração
            n->owned = false;
        }

        // Avança o loop
        n = n->next;
    }

    if (lastHover != NULL) {
        Block *b = &lastHover->block;
        if (IsMouseButtonPressed(0) && mouse->holding == NULL) {
            // Obterm posição do mouse relativa ao retângulo
            Vector2 blockPosition = (Vector2){b->rec.x, b->rec.y};
            mouse->offset = Vector2Subtract(mouse->position, blockPosition);
            mouse->holding = lastHover;
            b->dragging = true;
        }
    }
}