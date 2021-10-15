#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#define encode(x, c)      \
	if (c >= 'a')         \
		x = c - 'a' + 26; \
	else                  \
		x = c - 'A';

struct Slice
{
	uint8_t size;
	char *data;
};

struct TrieNode
{ 
	struct TrieNode* children[ALPHABET_SIZE];
	//long long int descendants;
	bool isEndOfWord;
	struct Slice* value;
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
bool insert(struct TrieNode *root, struct Slice key, struct Slice value)
{
	bool isThere=true; 
    struct TrieNode *curNode = root;
	
	for (int i = 0; i < key.size; i++)
	{
		int index = get_index(key.data[i]);

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
	struct Slice *val;
	val=(struct Slice *)malloc(sizeof(struct Slice));
	val->size=value.size;
	val->data=value.data; 
	// pthread_mutex_lock(&curNode->lock); 
	curNode->isEndOfWord = true;
	curNode->value=val;

	if(isEnd)
	{
		for (int i = key.size-1; i>=0; i--) 
		{ 
			curNode->no_of_ends-=1; 
			curNode = curNode->children[52];

		} 
	}
	// pthread_mutex_unlock(&m_lock);
	return isThere && isEnd;
}

bool search(struct TrieNode *root, struct Slice* key, struct Slice* value)
{
    struct TrieNode *curNode = root;

	for (int i = 0; i<key.size; i++) 
	{ 
		// int index = get_index(key.data[i]); 
		int index = get_index(key.data[i]);

		if (!curNode->children[index]) 
		{
			// pthread_mutex_unlock(&m_lock);
			return false; 
		}
		curNode = curNode->children[index]; 
	}
	// return curNode->value;
	if(curNode->value!=NULL)
		value=*curNode->value;
	// pthread_mutex_unlock(&m_lock);
	return (curNode != NULL && curNode->isEndOfWord); 
}

int main() {
	struct Slice* a,b,c,d,e,f,test,test1;

	struct TrieNode* root = create_node();

	a=(struct Slice *)malloc(sizeof(struct Slice));
    b=(struct Slice *)malloc(sizeof(struct Slice));
    c=(struct Slice *)malloc(sizeof(struct Slice));
    d=(struct Slice *)malloc(sizeof(struct Slice));
    e=(struct Slice *)malloc(sizeof(struct Slice));
    f=(struct Slice *)malloc(sizeof(struct Slice));
    test=(struct Slice *)malloc(sizeof(struct Slice));
    test1=(struct Slice *)malloc(sizeof(struct Slice));

	a.data=(char *)"hi";
    a.size=2;
	insert(root, a);

    b.data=(char *)"hello";
    b.size=5;
	insert(root, b);

    c.data=(char *)"welcome";
    c.size=7;
	insert(root, c);

    d.data=(char *)"adab";
    d.size=4;
	insert(root, d);

    e.data=(char *)"namaste";
    e.size=7;
	insert(root, e);

    f.data=(char *)"vanakkam";
    f.size=8;
	insert(root, f);

	bool result = search(head, a, &test);
	if (result)
		printf("Found!\nThe value:%s", test.data);
	else
		printf("Not Found\n");
}