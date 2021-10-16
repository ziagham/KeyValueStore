#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "kvstore.h"

// Returns new trie node (initialized to NULLs) 
struct TrieNode* create_node(void)
{
	struct TrieNode* pNode = NULL;
 
    pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
	
	pNode->isEndOfWord = false; 
	pNode->no_of_ends=0;

	for (int i = 0; i < 52; i++) 
		pNode->children[i] = NULL; 

	return pNode; 
}

// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
bool insert(struct TrieNode *root, char* key, char* value)
{
	pthread_mutex_lock(&m_lock);
	bool isThere=true; 
    struct TrieNode* curNode = root;
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
		curNode->children[index]->no_of_ends+=1;
		curNode = curNode->children[index];
	}

	bool isEnd=curNode->isEndOfWord; 
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
	pthread_mutex_unlock(&m_lock);
	return isThere && isEnd;
}

bool lookup(struct TrieNode* root, char* key, char** value)
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

bool del(struct TrieNode* root, char* key)
{
	pthread_mutex_lock(&m_lock);
	struct TrieNode* curNode = root;
	int length = strlen(key);

	for (int i = 0; i<length-1; i++) 
	{ 
		int index = get_index(key[i]);

		if (!curNode->children[index]) 
		{
			pthread_mutex_unlock(&m_lock);
			return false; 
		}
		curNode = curNode->children[index]; 
	}

	int i = length-1;
	int index = get_index(key[i]);

	if(curNode->children[index]==NULL)
	{
		pthread_mutex_unlock(&m_lock);
		return false;
	}
	struct TrieNode* temp = curNode;
	temp = temp->children[index];
	for (int i = length-1; i>=0; i--) 
	{
		temp->no_of_ends-=1;
		temp = temp->children[52];
	}

	if(curNode->children[index]->no_of_ends==0 && curNode->children[index]->isEndOfWord)
	{
	
		free(curNode->children[index]);
		curNode->children[index] = NULL;
	}
	else
	{
		curNode->children[index]->value=NULL;
		curNode->children[index]->isEndOfWord=false;
	}
	pthread_mutex_unlock(&m_lock);
	return true;
}

int main() {
	char* test;
	struct TrieNode* root = create_node();
	char keys[][8] = {"art", "artist", "cuck", "cunny", "dog", "zoo", "zoom", "zone"};
	char values[][8] = {"amin", "amin2", "amin3", "amin4", "amin5", "amin6", "amin7", "amin8"};
	char output[][32] = {"Not present in trie", "Present in trie"};

	int i;
    for (i = 0; i < ARRAY_SIZE(keys); i++)
        insert(root, keys[i], values[i]);

	printf("%s --- %s\n", "art", output[lookup(root, "art", &test)]);
    printf("%s --- %s\n", "cuck", output[lookup(root, "cuck", &test)]);
    printf("%s --- %s\n", "dog", output[lookup(root, "dog", &test)]);
    printf("%s --- %s\n", "zoom", output[lookup(root, "zoom", &test)]);

	del(root, "art");
	del(root, "zoom");

    printf("%s --- %s\n", "art", output[lookup(root, "art", &test)]);
    printf("%s --- %s\n", "zoom", output[lookup(root, "zoom", &test)]);
    printf("%s --- %s\n", "artist", output[lookup(root, "artist", &test)]);
    printf("%s --- %s\n", "zoo", output[lookup(root, "zoo", &test)]);
}