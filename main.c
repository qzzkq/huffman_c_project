#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "huffman.h"
int main(int argc,char** argv) {
	if (argc == 1 || argc & 1 == 0) {
		printf("Wrong input! Print '--help'.");
		return 0;
	}
	else if (argc == 2 && (strcmp(argv[1], "--help")==0)) {
		printf("Hello! This is a Huffman compressor/decompressor.\n"
			"Usage:\n"
			"  ./a.out <num_operations> <op1> <input1> <output1> <op2> <input2> <output2> ...\n"
			"\n"
			"Where:\n"
			"  <num_operations> - number of operations to perform\n"
			"  <opX> - 'c' for compress or 'd' for decompress\n"
			"  <inputX>/<outputX> - file paths for each operation. File must be .txt or .bin formats\n");
		return 0;
	}
	int num = atoi(argv[1]);
	for (int i = 2; i < 3 * num; i += 3) {
		if (strcmp(argv[i], "c") == 0) {
			compression(argv[i + 1], argv[i + 2]);
		}
		else if (strcmp(argv[i], "d") == 0) {
			decompression(argv[i + 1], argv[i + 2]);
		}
		else {
			printf("Wrong operation! Print '--help'.\n");
		}
	}
}