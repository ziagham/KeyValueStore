#include <pthread.h>

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (53)
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

pthread_mutex_t m_lock;

struct TrieNode
{ 
	struct TrieNode* children[ALPHABET_SIZE];
	bool isEndOfWord; // isEndOfWord is true if the node represents end of a word 
	char* value;
	int no_of_ends;
};

int get_index(char ch)
{
	return ch-65 - (6 & -(ch>='a'));
}

// bool insert(struct TrieNode *root, char* key, char* value);
// bool lookup(struct TrieNode* root, char* key, char** value);
// bool del(struct TrieNode* root, char* key);