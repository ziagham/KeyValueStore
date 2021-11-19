#include <stdbool.h>

#define ALPHABET_SIZE (56)

struct TrieNode
{ 
	struct TrieNode *children[ALPHABET_SIZE];
	bool isEndOfWord;
	char* value;
	bool lockHeld;
	int no_of_ends;
};