#include <pthread.h>

#define ALPHABET_SIZE (53)
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

pthread_mutex_t m_lock;

struct TrieNode
{ 
	struct TrieNode* children[ALPHABET_SIZE];
	bool isEndOfWord;
	char* value;
	int no_of_ends;
};

int get_index(char ch)
{
	return ch-65 - (6 & -(ch>='a'));
}