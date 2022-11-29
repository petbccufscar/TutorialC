#include "puzzle.h"

// Alocação de memória estática das variáveis globais (extern no .h)
Mouse *mouse;

/**==============================================
 *                newBlock()
 *  Inicializa um bloco com o texto e posição
 *  dados, além de calcular o tamanho correto
 *  dado o texto
 *=============================================**/
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

/**============================================
 *               newBList()
 * Aloca uma nova bList e a inicializa. 
 * Retorna um ponteiro para ela.
 *=============================================**/
bList* newBList() {
    bList *new = malloc(sizeof (bList));
    new->end = NULL;
    new->head = NULL;
    new->size = 0;
    return new;
}

/**============================================
 *               newBNode()
 * Aloca um novo nó de bloco (bNode) e o inicializa
 * conforme recebido. Também define o padrão de
 * bloco com dono e não permanente.
 *=============================================**/
bNode* newBNode(bNode *prev, Block b, bNode *next) {
    bNode *new = malloc(sizeof (bNode));
    new->block = b;
    new->prev = prev;
    new->next = next;
    new->owned = true;
    new->permanent = false;
    return new;
}

/**============================================
 *               newMouse()
 * Cria uma instância da estrutura do Mouse e 
 * inicializa ela com valores padrões.
 *=============================================**/
Mouse* newMouse() {
    Mouse *m = malloc(sizeof(Mouse));
    m->position = (Vector2){-100.0f, -100.0f};
    m->offset = (Vector2){0.0f, 0.0f};
    m->holding = NULL;
    m->hovering = NULL;
    m->color = MAROON;
    return m;
}

/**============================================
 *               removeBNode()
 * Dado o ponteiro de um nó e sua lista correspondente
 * remove esse nó da lista corretamente, ajustando
 * os outros nós conforme necessário.
 * 
 * Também verifica o mouse para evitar que ele tenha
 * referências inválidas
 *=============================================**/
bool removeBNode(bList *list, bNode *node) {
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

/**============================================
 *               addBNode()
 * Adiciona um novo nó ao final da lista dada.
 * Recebe um bloco, que é transformado em um nó
 * padrão (newBNode()) e inserido na lista.
 *=============================================**/
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

/**============================================
 *               newBlockSpawner()
 * Cria uma nova instância de um gerador de blocos
 * utilizando valores padrão e um bloco como base
 *=============================================**/
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

/**============================================
 *               newBlockField()
 * Cria uma nova instância de um campo de blocos
 * utilizando valores padrões definidos no cabeçalho
 * TODO: Retirar a lógica do posicionamento aleatório daqui e colocar no main.c na parte de debugging
 *=============================================**/
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

/**============================================
 *               newCodePuzzle()
 * Cria um novo CodePuzzle vazio
 * Nota: Não sei o por que desse aviso, tudo parece 
 *       funcionar bem... 
 *=============================================**/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
CodePuzzle newCodePuzzle() {
    CodePuzzle cp = {
        .elements = {0},
        .num_elements = 0,
        .bspawners = {0},
        .num_bspawners = 0,
        .blocos = newBList(),
        .nextPosition = {TRAY_H + PUZZLE_PADDING, TRAY_H}
    };
    return cp;
}
#pragma GCC diagnostic pop

/**============================================
 *               newStrElement()
 * Cria um novo elemento do tipo texto usando 
 * uma string
 *=============================================**/
Element newStrElement(char str[]) {
    // TODO: Receber Vector2 da posição depois que as funções de 
    //       spawn ficarem decidindo isso...
    // TODO: Criar função de desalocar memória de um CodePuzzle
    TextElem *txt = malloc(sizeof(TextElem));
    strcpy(txt->str, str);
    txt->position = (Vector2){rand() % (int)(GetScreenWidth()), rand() % (int)(GetScreenHeight())};

    Element e = {
        .type = text,
        .txt = txt
    };
    return e;
}

/**============================================
 *               newBfElement()
 * Cria um novo elemento do tipo field usando 
 * um ponteiro de BlockField
 *=============================================**/
Element newBfElement() {
    BlockField bf = newBlockField();

    Element e = {
        .type = field,
        .bf = malloc(sizeof (BlockField))
    };
    *e.bf = bf; // Copia pro endereço de bf
    return e;
}

/**============================================
 *               spawnBNode()
 * Dada uma lista de nós de bloco, o texto e posição,
 * cria um bloco e o adiciona na lista.
 *=============================================**/
bNode* spawnBNode(bList *list, char text[], Vector2 position) {
    Block b = newBlock(text, position);
    return addBNode(list, b);
}

/**============================================
 *               spawnBlockSpawnerOld()
 * Dado um array de geradores de bloco, o número
 * atual de geradores e um bloco como base, cria
 * e adiciona um novo gerador a array caso ainda
 * exista espaço.
 * TODO: Alterar para lista ligada assim como foi feito com os Blocos (bList)
 *       ou não também... Diferente dos blocos acho que os geradores serão 
 *       criados e deletados todos juntos de acordo com o arquivo lido, é capaz
 *       que não seja necessário, só criar uma função de limpar a array toda.
 *=============================================**/
bool spawnBlockSpawnerOld(BlockSpawner *arr, int *num_bspawners, Block base) {
    if (*num_bspawners >= NUM_BLOCK_SPAWNER) {
        printf("Não foi possível gerar um novo gerador de blocos, máximo (%d) atingido\n", NUM_BLOCK_SPAWNER);
        return false; // Não é possível gerar mais blocos
    } else {
        arr[*num_bspawners] = newBlockSpawner(base);
        *num_bspawners += 1;
        return true;
    }
}

/**============================================
 *               spawnBlockField()
 * Dado um array de campos de bloco e o número atual
 * de campos, cria e adiciona um campo de bloco ao 
 * array caso ainda exista espaço
 * TODO: Alterar para lista ligada assim como foi feito com os Blocos (bList)
 *       ou não também... Ver TODO do spawnBlockSpawnerOld()
 *=============================================**/
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

/**============================================
 *               spawnElementTxt()
 * Adiciona um elemento de texto ao final de um 
 * CodePuzzle, a ordem importa!
 *=============================================**/
bool spawnElementTxt(CodePuzzle *cp, char text[]) {
    if (cp->num_elements >= MAX_PUZZLE_ELEMENTS) {
        printf("Não foi possível gerar um novo elemento, máximo (%d) atingido\n", MAX_PUZZLE_ELEMENTS);
        return false; 
    } else {
        cp->elements[cp->num_elements] = newStrElement(text);
        cp->num_elements += 1;
        return true;
    }
}

/**============================================
 *               spawnElementBf()
 * Adiciona um elemento de campo ao final de um 
 * CodePuzzle, a ordem importa!
 *=============================================**/
bool spawnElementBf(CodePuzzle *cp) {
    if (cp->num_elements >= MAX_PUZZLE_ELEMENTS) {
        printf("Não foi possível gerar um novo elemento, máximo (%d) atingido\n", MAX_PUZZLE_ELEMENTS);
        return false; 
    } else {
        cp->elements[cp->num_elements] = newBfElement();
        cp->num_elements += 1;
        return true;
    }
}

/**============================================
 *               spawnBlockSpawner()
 * Adiciona um gerador a um CodePuzzle, a ordem
 * importa!
 *=============================================**/
bool spawnBlockSpawner(CodePuzzle *cp, char text[]) {
    // Cálculo da posição do spawner  
    // TODO: Usar o tamanho da fonte ao invés de 20 quando tiver as fontes
    // TODO: Possivelmente alinhar os blocos lado a lado no futuro também... Ou centralizar pelo menos
    Vector2 pos = (Vector2){TRAY_PADDING, 
                            TRAY_PADDING + cp->num_bspawners*((BLOCK_TEXT_PADDING*2 + 20) + TRAY_V_SPACING)};

    // Spawn do spawner propriamente dito
    if (cp->num_bspawners >= MAX_PUZZLE_SPAWNERS) {
        printf("Não foi possível gerar um novo gerador de blocos, máximo (%d) atingido\n", MAX_PUZZLE_SPAWNERS);
        return false;
    } else {
        Block base = newBlock(text, pos);
        cp->bspawners[cp->num_bspawners] = newBlockSpawner(base);
        cp->num_bspawners += 1;
        return true;
    }
}

/**============================================
 *               DrawBlock()
 * Recebe um bloco e estados relevantes do mouse.
 * De acordo com isso, desenha o bloco na tela.
 * TODO: Alterar pra receber bNodes ao invés de Blocks
 *=============================================**/
void DrawBlock(bNode *node) {
    Color textColor = MAROON;
    Block *b = &node->block;
    int segments = 50; float roundness = 0.4f; float lineThick = 2.0f;
    DrawRectangleRoundedLines(b->rec, roundness, segments, lineThick, DARKGRAY); // Outline
    DrawRectangleRounded(b->rec, roundness, segments, LIGHTGRAY); // Preenchimento sólido
    if (mouse->holding == node) {
        DrawRectangleRounded(b->rec, roundness, segments, MAROON); // Preenchimento segurando
        textColor = WHITE;
    } else if (mouse->hovering == node) {
        DrawRectangleRounded(b->rec, roundness, segments, DARKGRAY); // Preenchimento hovering
        textColor = WHITE;
    };
    DrawText(b->text, (int)b->rec.x + BLOCK_TEXT_PADDING, (int)b->rec.y + BLOCK_TEXT_PADDING, FONT_SIZE, textColor); // Texto
}

/**============================================
 *               DrawBlockSpawner()
 * Recebe um gerador de blocos e estados relevantes
 * do mouse. De acordo com isso desenha o gerador 
 * na tela.
 * TODO: Alterar pra receber bNodes ao invés de Blocks
 *=============================================**/
void DrawBlockSpawner(BlockSpawner *bf) {
    DrawBlock(&bf->base);
}

/**============================================
 *               DrawBlockField()
 * Recebe um campo de bloco. 
 * De acordo com ele desenha o campo na tela.
 *=============================================**/
void DrawBlockField(BlockField *bf) {
    int segments = 50; float roundness = 0.4f; float lineThick = 1.0f;
    DrawRectangleRoundedLines(bf->rec, roundness, segments, lineThick, MAROON);
}

/**============================================
 *               DrawCodePuzzle()
 * Recebe um CodePuzzle, e desenha ele corretamente
 * na tela
 *=============================================**/
void DrawCodePuzzle(CodePuzzle *cp) {
    DrawRectangle(0, 0, TRAY_H, GetScreenHeight(), GRAY);
    for (int i = 0; i < cp->num_bspawners; i++) {
        DrawBlockSpawner(&cp->bspawners[i]);
    }
    for (int i = 0; i < cp->num_elements; i++) {
        Element e = cp->elements[i];
        if (e.type == text) {
            // Texto
            TextElem *te = e.txt;
            // TODO Alterar para usar fonte externa -> Usar o extern no .h dnv
            DrawText(te->str, te->position.x, te->position.y, 20, MAROON);
        } else {
            // Field
            DrawBlockField(e.bf);
        }
    }
    bNode *node = cp->blocos->head;
    while (node != NULL) {
        DrawBlock(node); 
        node = node->next;
    }
}

/**============================================
 *            updateCamposMouseSolto()
 * Função auxiliar do updateCampos() e updateCodePuzzle()
 *=============================================**/
void updateCampoMouseSolto(BlockField *bf) {
    // Para o dado campo, verificar se foi solto na sua posição
    printf("\n\n%d %d %d\n\n", CheckCollisionPointRec(mouse->position, bf->rec), mouse->holding != NULL, (bf->node == NULL || bf->node == mouse->holding));
    if (CheckCollisionPointRec(mouse->position, bf->rec)        // Posição correta
        && mouse->holding != NULL                               // O mouse está segurando algo
        && (bf->node == NULL || bf->node == mouse->holding)) {  // O campo está vazio ou com o mesmo bloco que o mouse
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

/**============================================
 *               updateCampos()
 * Calcula e altera o estado de cada campo do array
 * passado. Leva em consideração e altera o estado 
 * do mouse.
 * Ao final da função, nós pertencentes a um campo
 * tem seu campo owned = true
 *=============================================**/
void updateCampos(BlockField bfields[], int *num_bfields) {
    // Quando um mouse está segurando um bloco e para em cima de um campo, esse
    // campo deve acomodar o bloco. 

    // Um bloco acabou de ser solto?
    if (IsMouseButtonReleased(0)) {
        for (int i = 0; i < *num_bfields; i++) {
            updateCampoMouseSolto(&bfields[i]);
        }
    }

    for (int i = 0; i < *num_bfields; i++) {
        BlockField *bf = &bfields[i];
        if (bf->node != NULL) {
            bf->node->owned = true;
        }
    }
}

/**============================================
 *               updateGeradores()
 * Calcula e altera o estado de cada gerador do array 
 * passado, levando em consideração o estado do mouse.
 * Recebe também a lista de blocos para poder gerar 
 * os blocos.
 * Ao final da função, blocos gerados por um gerador
 * que estejam agarrados pelo mouse tem seu campo owned = true.
 *=============================================**/
void updateGeradores(bList *list, BlockSpawner bspawners[], int *num_bspawners) {
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

/**============================================
 *               updateBNodes()
 * Calcula e atualiza cada nó de bloco de uma dada
 * lista de nós levando em conta o estado do mouse.
 * 
 * Responsável principalmente por alterações no 
 * estado do mouse, e da verificação de owner de 
 * cada bloco.
 *=============================================**/
void updateBNodes(bList *list) {
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
            removeBNode(list, n);
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
            // Obtem posição do mouse relativa ao retângulo
            Vector2 blockPosition = (Vector2){b->rec.x, b->rec.y};
            mouse->offset = Vector2Subtract(mouse->position, blockPosition);
            mouse->holding = lastHover;
            b->dragging = true;
        }
    }
}

/**============================================
 *               updateCodePuzzle()
 * Percorre o CodePuzzle utilizando as funções
 * de update próprias de cada elemento para fazer
 * o update
 *=============================================**/
void updateCodePuzzle(CodePuzzle *cp) {
    //* Geradores
    updateGeradores(cp->blocos, cp->bspawners, &cp->num_bspawners);

    //* Campos
    if (IsMouseButtonReleased(0)) {
        for (int i = 0; i < cp->num_elements; i++) {
            if (cp->elements[i].type == field) {
                updateCampoMouseSolto(cp->elements[i].bf);
            }
        }
    }

    for (int i = 0; i < cp->num_elements; i++) {
        if (cp->elements[i].type == field) {
            BlockField *bf = cp->elements[i].bf;
            if (bf->node != NULL) {
                bf->node->owned = true;
            }
        } 
    }

    //* Update Posições Textos e Campos
    //! NÃO FUNCIONA AINDAAAA
    for (int i = 0; i < cp->num_elements; i++) {
        Element *e = &cp->elements[i];
        if (e->type == field) {
            e->txt->position.x += 1;
            TextElem *te = e->txt;
            te->position = cp->nextPosition;

            Vector2 tm = MeasureTextEx(GetFontDefault(), te->str, 20, 1);
            cp->nextPosition = Vector2Add(cp->nextPosition, (Vector2){tm.x + PUZZLE_H_SPACING, 0});
        } else {
            BlockField *be = e->bf;
            be->rec.x = cp->nextPosition.x;
            be->rec.y = cp->nextPosition.y;

            cp->nextPosition = Vector2Add(cp->nextPosition, (Vector2){be->rec.width + PUZZLE_H_SPACING, 0});
        }
    }
    cp->nextPosition = (Vector2){TRAY_H + PUZZLE_PADDING, TRAY_H};
}