#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

// Blocos Arrastáveis
#define NUM_BLOCKS 50 // Máximo de blocos que podem existir
#define MAX_TEXT_BLOCK 255 // Tamanho máximo do texto de cada bloco (Nota: Se o texto está bugando com acentos, deve ser isso)
#define FONT_SIZE 20 // Tamanho do texto
#define BLOCK_TEXT_PADDING 8 // Padding entre o bloco e o texto no meio
// Campo de Bloco
#define NUM_BLOCK_FIELDS 50 // Máximo de campos de bloco
#define BLOCK_FIELD_PADDING 4 // Padding entre o campo do bloco e o bloco
// Geradores de Blocos
#define NUM_BLOCK_SPAWNER 50 // Máximo de geradores de blocos


// ---------------------------------------------------------------------------------------------------
// Estruturas
// Bloco Arrastável
typedef struct Block {
    char text[MAX_TEXT_BLOCK + 1];
    Rectangle rec;
    bool hover;
    bool dragging;
} Block;

// Precisamos de uma estrutura que permita remoção de blocos fora de ordem
// Como não teremos blocos sobrevivendo "sozinhos" (Sempre estarão ou acoplados
// a um gerador ou campo) acho que não vai causar muitos problemas... espero
typedef struct bNode { // Block Node
    Block block;
    struct bNode *next;
    struct bNode *prev;
    bool owned;
    bool permanent;
} bNode;

typedef struct bList {
    bNode *head;
    bNode *end;
    size_t size;
} bList;

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


// ---------------------------------------------------------------------------------------------------
// Funções de criação e deleção
Block newBlock(char text[], Vector2 position);
bList* newBList();
bNode* newBNode(bNode *prev, Block b, bNode *next);
Mouse newMouse();
// Remove nó com base no seu ponteiro
bool removeBNode(Mouse *mouse, bList *list, bNode *node);
// Adiciona um nó na lista (Final da lista)
bNode* addBNode(bList *list, Block b);
BlockSpawner newBlockSpawner(Block base);
BlockField newBlockField();


// ---------------------------------------------------------------------------------------------------
// Funções de Spawn
bNode* spawnBNode(bList *list, char text[], Vector2 position);
bool spawnBlockField(BlockField *arr, int *num_bfields);
bool spawnBlockSpawner(BlockSpawner *arr, int *num_bspawners, Block base);


// ---------------------------------------------------------------------------------------------------
// Funções de desenho
// TODO: Alterar pra receber bNodes ao invés de Blocks
void DrawBlock(Block *b, Block *holding, Block *hovering);
// TODO: Alterar pra receber bNodes ao invés de Blocks
void DrawBlockSpawner(BlockSpawner *bf, Block *holding, Block *hovering);
void DrawBlockField(BlockField *bf);


// ---------------------------------------------------------------------------------------------------
// Funções de Atualização
// Update da colisão entre Mouse / Campos de Bloco
void updateCampos(Mouse *mouse, BlockField bfields[], int *num_bfields);
// Update dos Geradores de Bloco
void updateGeradores(Mouse *mouse, bList *list, BlockSpawner bspawners[], int *num_bspawners);
// Update da colisão entre Mouse / Blocos
void updateBNodes(Mouse *mouse, bList *list);

#endif