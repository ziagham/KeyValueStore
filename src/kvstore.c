#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// struct Slice
// {
// 	uint8_t size;
// 	char *data;
// };

struct TrieNode
{ 
	struct TrieNode* children[ALPHABET_SIZE];
	//long long int descendants;
	bool isEndOfWord;
	// struct Slice* value;
	char* value;
	//struct Slice word;
	int no_of_ends;
};

// Function that returns a new Trie node
struct TrieNode* create_node(void)
{
	struct TrieNode* pNode = NULL;
 
    pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
	
	pNode->isEndOfWord = false; 
	pNode->no_of_ends=0;

	for (int i = 0; i < ALPHABET_SIZE; i++) 
		pNode->children[i] = NULL; 

	return pNode; 
}

void free_node(struct TrieNode* node) {
    // Free the trienode sequence
    for(int i=0; i<ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            free_node(node->children[i]);
        }
        else {
            continue;
        }
    }
    free(node);
}

int get_index(char ch)
{
	// if(ch>='a')
	//     return ch-71;
	// return ch-65;
	return ch-65 - (6 & -(ch>='a'));
}

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
bool insert(struct TrieNode *root, char* key, char* value)
{
	bool isThere=true; 
    struct TrieNode *curNode = root;
	int length = strlen(key);

	for (int i = 0; i < length; i++)
	{
		int index = get_index(key[i]);

		if (!curNode->children[index])
		{ 
			curNode->children[index] = create_node();
			curNode->children[index]->children[52] = curNode; 
			isThere=false;
		}
		// pthread_mutex_lock(&curNode->children[index]->lock); 
		curNode->children[index]->no_of_ends+=1;
		curNode = curNode->children[index];
	}

	bool isEnd=curNode->isEndOfWord; 
	// struct Slice *val;
	// val=(struct Slice *)malloc(sizeof(struct Slice));
	// val->size=value.size;
	// val->data=value.data; 
	// pthread_mutex_lock(&curNode->lock); 
	curNode->isEndOfWord = true;
	curNode->value=value;

	if(isEnd)
	{
		for (int i = length-1; i>=0; i--) 
		{ 
			curNode->no_of_ends-=1; 
			curNode = curNode->children[52];

		} 
	}
	// pthread_mutex_unlock(&m_lock);
	return isThere && isEnd;
}

bool search(struct TrieNode *root, char* key, char** value)
{
    struct TrieNode *curNode = root;
	int length = strlen(key);

	for (int i = 0; i<length; i++) 
	{ 
		// int index = get_index(key.data[i]); 
		int index = get_index(key[i]);

		if (!curNode->children[index]) 
		{
			// pthread_mutex_unlock(&m_lock);
			return false; 
		}
		curNode = curNode->children[index]; 
	}
	// return curNode->value;
	if(curNode->value!=NULL)
		*value=curNode->value;
	// pthread_mutex_unlock(&m_lock);
	return (curNode != NULL && curNode->isEndOfWord); 
}

int main() {
	//char* a,b,c,d,e,f,test,test1;
	char* test;
	struct TrieNode* root = create_node();
	char keys[][8] = {"the", "a", "there", "answer", "any", "by", "bye", "their"};
	char values[][8] = {"amin", "amin2", "amin3", "amin4", "amin5", "amin6", "amin7", "amin8"};
	char output[][32] = {"Not present in trie", "Present in trie"};

	int i;
    for (i = 0; i < ARRAY_SIZE(keys); i++)
        insert(root, keys[i], values[i]);

	printf("%s --- %s\n", "the", output[search(root, "the", &test)]);
    printf("%s --- %s\n", "these", output[search(root, "these", &test)]);
    printf("%s --- %s\n", "their", output[search(root, "their", &test)]);
    printf("%s --- %s\n", "thaw", output[search(root, "thaw", &test)]);

	// bool result = search(root, "hi", &test);
	// if (result)
	// 	printf("Found!\nThe value:%s\n", test);
	// else
	// 	printf("Not Found\n");
}