#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

// Blocos
#define NUM_BLOCKS 50            // Máximo de blocos que podem existir
#define MAX_TEXT_SIZE 255       // Tamanho máximo do texto de cada bloco (Nota: Se o texto está bugando com acentos, deve ser isso)
#define FONT_SIZE 20             // Tamanho do texto
#define BLOCK_TEXT_PADDING 8     // Padding entre o bloco e o texto no meio
// Campo de Bloco
#define NUM_BLOCK_FIELDS 50      // Máximo de campos de bloco
#define BLOCK_FIELD_PADDING 4    // Padding entre o campo do bloco e o bloco
// Geradores de Blocos
#define NUM_BLOCK_SPAWNER 50     // Máximo de geradores de blocos
// Puzzles
#define MAX_PUZZLE_ELEMENTS 64   // Máximo de elementos para cada puzzle
#define MAX_PUZZLE_SPAWNERS 16   // Máximo de geradores para cada puzzle
#define PUZZLE_PADDING 16        // Espaçamento entre o puzzle e a parte superior e esquerda 
#define V_ELEMENT_SPACING 8      // Espaçamento vertical entre elementos   
#define H_ELEMENT_SPACING 12     // Espaçamento horizontal entre elementos

/**============================================
 **              Estruturas
 *=============================================**/

// Bloco
typedef struct Block {
    char text[MAX_TEXT_SIZE + 1];
    Rectangle rec;
    bool hover;
    bool dragging;
} Block;

// Lista de Blocos (Nós)
typedef struct bNode {
    Block block;
    struct bNode *next;
    struct bNode *prev;
    bool owned;
    bool permanent;
} bNode;

// Lista de bNodes
typedef struct bList {
    bNode *head;
    bNode *end;
    size_t size;
} bList;

// Mouse
typedef struct Mouse {
    Vector2 position;
    Vector2 offset;
    bNode *holding;
    bNode *hovering;
    Color color;
} Mouse;

// Gerador de Bloco
typedef struct BlockSpawner {
    bNode base;
    bNode *node;
} BlockSpawner;

// Campo de bloco
typedef struct BlockField {
    bNode *node; 
    Rectangle rec;
} BlockField;

/**============================================
 **INCOMPLETO   Elemento dos Puzzles
 * Me baseando nisso https://stackoverflow.com/questions/7798383/array-of-pointers-to-multiple-types-c
 * Pra criar uma array que vai guardar cada puzzle internamente, cada elemento do puzzle vai ser ou
 * um pedaço de código ou texto (uma simples string, a formatação acho que vamos ver depois na hora
 * de desenhar) ou um campo de bloco. Também vai precisar adicionar em cada campo de bloco o bloco
 * apropriado para ele.
 *=============================================**/

typedef enum ElementType {text, field} ElementType;
typedef struct Element {
    ElementType type;
    union {
        char str[MAX_TEXT_SIZE];
        BlockField *bf;
    };
} Element;

typedef struct CodePuzzle {
    Element elements[MAX_PUZZLE_ELEMENTS];
    int num_elements;
    BlockSpawner bspawners[MAX_PUZZLE_SPAWNERS];
    int num_bspawners;
} CodePuzzle;


/**============================================
 **       Funções de Criação e Deleção
 * Funções primitivas de criação de objetos
 *=============================================**/

Block newBlock(char text[], Vector2 position);
bList* newBList();
bNode* newBNode(bNode *prev, Block b, bNode *next);
Mouse newMouse();
bool removeBNode(Mouse *mouse, bList *list, bNode *node);
bNode* addBNode(bList *list, Block b);
BlockSpawner newBlockSpawner(Block base);
BlockField newBlockField();
Element newStrElement(char str[]);
Element newBfElement();
CodePuzzle newCodePuzzle();


/**============================================
 **             Funções de Spawn
 * Criam um objeto e o colocam numa estrutura de
 * armazenamento, utilizada pelos updates
 *=============================================**/

bNode* spawnBNode(bList *list, char text[], Vector2 position);
bool spawnBlockField(BlockField *arr, int *num_bfields);
bool spawnBlockSpawnerOld(BlockSpawner *arr, int *num_bspawners, Block base);
bool spawnElementStr(CodePuzzle *cp, char text[]);
bool spawnElementBf(CodePuzzle *cp);
bool spawnBlockSpawner(CodePuzzle *cp, char text[]);

/**============================================
 **            Funções de Desenho
 * Desenham o objeto passado usando as funções do
 * raylib
 *=============================================**/

void DrawBlock(Block *b, Block *holding, Block *hovering);
void DrawBlockSpawner(BlockSpawner *bf, Block *holding, Block *hovering);
void DrawBlockField(BlockField *bf);


/**============================================
 **            Funções de Atualização
 * Percorrem as estruturas de armazenamento relevantes
 * atualizando a posição e estado de cada objeto
 * com base na interação do usuário
 *=============================================**/

void updateGeradores(Mouse *mouse, bList *list, BlockSpawner bspawners[], int *num_bspawners);
void updateCampos(Mouse *mouse, BlockField bfields[], int *num_bfields);
void updateBNodes(Mouse *mouse, bList *list);


#endif