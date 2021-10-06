#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

const int ALPHABET_SIZE = 58;

typedef struct Slice
{
	uint8_t size;
	char *data;
} Slice;

struct TrieNode 
{ 
	struct TrieNode* children[ALPHABET_SIZE]; 
	long long int descendants;
	bool isEndOfWord; 
	char* value;
	Slice word;
};


int main() {
    printf("Hello World [%d]\n", ALPHABET_SIZE);

   return 0;
}