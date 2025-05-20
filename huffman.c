#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 256 // char, 1 байт


void swap_nodes(tree_node** a, tree_node** b) { // функци€ дл€ обмена узлов дерева хаффмана
	tree_node* tmp = *a;
	*a = *b;
	*b = tmp;
}


Heap* initHeap(int size) { // функци€ дл€ инициализировани€ кучи
	Heap* newHeap = (Heap*)malloc(sizeof(Heap));
	newHeap->data = (tree_node**)malloc(sizeof(tree_node*) * size);
	newHeap->size = 0;
	newHeap->capacity = size;
	return newHeap;
}

void siftUp(Heap* heap, int i) { //  просеивание вверх дл€ кучи
	while (heap->data[i]->f < heap->data[(i - 1) / 2]->f) {
		swap_nodes(&heap->data[i], &heap->data[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

void siftDown(Heap* heap, int i) { // просеивание вниз дл€ кучи
	int right, left, j;
	while (2 * i + 1 < heap->size) {
		left = 2 * i + 1;
		right = 2 * i + 2;
		j = left;
		if (right < heap->size && heap->data[right]->f < heap->data[left]->f) {
			j = right;
		}
		if (heap->data[i]->f <= heap->data[j]->f) {
			break;
		}
		swap_nodes(&heap->data[i], &heap->data[j]);
		i = j;
	}
}
void insert(Heap* heap, tree_node* node) { // вставка эл-ма в кучу
	heap->data[heap->size++] = node;
	siftUp(heap, heap->size - 1);
}

tree_node* extractMin(Heap* heap) { // извлечение минимального элемента из кучи
	tree_node* min = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	--heap->size;
	siftDown(heap, 0);
	return min;
}
tree_node* newTreeFromSym(char sym, double f) {
	tree_node* newNode = (tree_node*)malloc(sizeof(tree_node));
	newNode->f = f;
	newNode->sym = sym;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

tree_node* newTreeFromTwoTrees(tree_node* tree1, tree_node* tree2) {
	tree_node* newNode = (tree_node*)malloc(sizeof(tree_node));
	newNode->f = tree1->f + tree2->f;
	newNode->left = tree1;
	newNode->right = tree2;
	return newNode;
}

tree_node* BuildHuffmanTree(int n, double* f) {
	Heap* heap = initHeap(n);
	tree_node* tree0;
	tree_node* tree1;
	for (int i = 0; i < n; ++i) {
		if (f[i] != 0) {
			insert(heap, newTreeFromSym((char)i, f[i]));
		}
	}
	while (heap->size > 1) {
		tree0 = extractMin(heap);
		tree1 = extractMin(heap);
		insert(heap, newTreeFromTwoTrees(tree0,tree1));
	}
	if (heap->size == 0) {
		return NULL;
	}
	tree0 = extractMin(heap);
	free(heap->data);
	free(heap);
	return tree0;
}

void writeTree(tree_node* root, FILE* out) { // запись дерева хаффмана в выходной файл
	if (!(root->left || root->right)) {
		fputc('L', out);
		fputc(root->sym, out);
	}
	else {
		fputc('I', out);
		writeTree(root->left, out);
		writeTree(root->right, out);
	}
}
tree_node* readTree(FILE* in) { // чтение дерева хаффмана
	char marker = fgetc(in);
	if (marker == EOF) return NULL;
	if (marker == 'L') {
		char sym = fgetc(in);
		tree_node* leaf = malloc(sizeof(tree_node));
		leaf->f = 0;
		leaf->sym = sym;
		leaf->left = leaf->right = NULL;
		return leaf;
	}
	else if (marker == 'I'){
		tree_node* node = malloc(sizeof(tree_node));
		node->f = 0;
		node->sym = 0;
		node->left = readTree(in);
		node->right = readTree(in);
		return node;
	}
	return NULL;
}

void generateCodes(tree_node* node, char* buf, int depth, HuffCode* codes) {
	if (!node || depth >= SIZE) return;
	// если лист Ч сохран€ем путь
	if (!node->left && !node->right) {
		if (depth == 0) {
			depth = 1;
			buf[0] = '0';
		}
		codes[(unsigned char)node->sym].length = depth;
		codes[(unsigned char)node->sym].code = malloc(depth + 1);
		memcpy(codes[(unsigned char)node->sym].code, buf, depth);
		codes[(unsigned char)node->sym].code[depth] = '\0';
		return;
	}
	// влево Ч 0
	buf[depth] = '0';
	generateCodes(node->left, buf, depth + 1, codes);
	// вправо Ч 1
	buf[depth] = '1';
	generateCodes(node->right, buf, depth + 1, codes);
}

void freeTree(tree_node* node) {
	if (!node) return;
	freeTree(node->left);
	freeTree(node->right);
	free(node);
}

void freeCodes(HuffCode* codes, char* buf) {
	if (codes) {
		for (int i = 0; i < SIZE; ++i)
			free(codes[i].code);
		free(codes);
	}
	free(buf);
}


void compression(char* input, char* output) {
	printf("Compressing data from %s to %s:\n", input, output);
	printf("Opening files...\n");
	FILE* in = fopen(input, "rb");
	if (!in) {
		printf("Wrong input file.\n");
		printf("Use '--help'.\n");
	}
	FILE* out = fopen(output, "wb+");
	printf("Counting frequencies...\n");
	double* freq = (double*)calloc(SIZE, sizeof(double));
	unsigned char ch;
	long total_symbols = 0;
	while (fread(&ch, sizeof(unsigned char), 1, in)) {
		++freq[ch];
		++total_symbols;
	}
	if (total_symbols == 0) {
		printf("Empty input file.\n");
		free(freq);
		fclose(in);
		fclose(out);
	}
	printf("Building tree...\n");
	tree_node* root = BuildHuffmanTree(SIZE, freq);

	writeTree(root, out);
	fwrite(&total_symbols, sizeof(long), 1, out);
	char* buf = malloc(SIZE+1);
	HuffCode* codes = (HuffCode*)calloc(SIZE,sizeof(HuffCode));
	generateCodes(root, buf, 0, codes);
	fseek(in, 0, SEEK_SET);
	printf("Writing compressed data to the output file...\n");
	unsigned char outByte = 0;
	int bitPos = 0;
	while (fread(&ch, 1, 1, in) == 1) {
		HuffCode hc = codes[ch];
		for (int i = 0; i < hc.length; ++i) {
			if (hc.code[i] == '1') {
				outByte |= (1 << (7 - bitPos));
			}
			bitPos++;
			if (bitPos == 8) {
				fwrite(&outByte, 1, 1, out);
				outByte = 0;
				bitPos = 0;
			}
		}
	}
	if (bitPos > 0) {
		fwrite(&outByte, 1, 1, out);
	};
	free(freq);
	freeCodes(codes, buf);
	freeTree(root);
	fflush(in);
	fflush(out);
	long out_size = ftell(out);
	long in_size = ftell(in);
	double coef = (double)in_size / (double)out_size;
	printf("Done!\nOriginal size of %s: %ld bytes\nCompressed size of %s: %ld bytes\nCompression coefficient: %.2f\n\n", input, in_size, output, out_size,coef);
	fclose(in);
	fclose(out);

}

void decompression(char* input, char* output) {
	printf("Decompressing data from %s to %s:\n", input, output);
	printf("Opening files...\n");
	FILE* in = fopen(input, "rb");
	if (!in) {
		printf("Wrong input file.\n");
		printf("Use '--help'.\n");
	}
	FILE* out = fopen(output, "wb+");
	printf("Reading tree...\n");
	tree_node* root = readTree(in);
	tree_node* cur = root;
	long total_symbols;
	fread(&total_symbols, sizeof(long), 1, in);
	printf("Writing decompressed data to the output file...\n");
	if (root && !root->left && !root->right) {
		for (long i = 0; i < total_symbols; ++i) {
			fputc(root->sym, out);
		}
	}
	else {
		cur = root;
		long symbols_written = 0;
		int byte;
		while ((byte = fgetc(in)) != EOF && symbols_written < total_symbols) {
			for (int bit = 7; bit >= 0; --bit) {
				if (symbols_written >= total_symbols) break;
				int b = (byte >> bit) & 1;
				cur = b ? cur->right : cur->left;
				if (!cur->left && !cur->right) {
					fputc(cur->sym, out);
					symbols_written++;
					cur = root;
				}
			}
		}
	}
	if (cur != root) {
		printf("File may be corrupted.\n");
	}
	freeTree(root);

	fclose(in);
	fclose(out);
	printf("Done!\n\n");
}