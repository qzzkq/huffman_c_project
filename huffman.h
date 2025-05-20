#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 256

typedef struct tree_node {
    double f;
    char sym;
    struct tree_node* left;
    struct tree_node* right;
} tree_node;

typedef struct {
    tree_node** data;
    int         size;
    int         capacity;
} Heap;

typedef struct {
    char* code;
    int   length;
} HuffCode;

void swap_nodes(tree_node** a, tree_node** b);
Heap* initHeap(int capacity);
void siftUp(Heap* heap, int i);
void siftDown(Heap* heap, int i);
void insert(Heap* heap, tree_node* node);
tree_node* extractMin(Heap* heap);
tree_node* newTreeFromSym(char sym, double f);
tree_node* newTreeFromTwoTrees(tree_node* t1, tree_node* t2);
tree_node* BuildHuffmanTree(int n, double* freq);
void writeTree(tree_node* root, FILE* out);
void generateCodes(tree_node* node, char* buf, int depth, HuffCode* codes);
void compression(char* input, char* output);
void decompression(char* input, char* output);
void freeTree(tree_node* node);
void freeCodes(HuffCode* codes, char* buf);

#endif // HUFFMAN_H
