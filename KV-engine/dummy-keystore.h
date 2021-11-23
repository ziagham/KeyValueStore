//
// @file    : dummy-keystore.h
// @purpose : A header file for a dummy key-value engine
// @author  : Prof. Phuong Hoai Ha & Ibrahim Umar & Koen van Greevenbroek & Hung Ngoc Phan
// @project : [INF-3210 Green Computing] Coursework Assignment
// @licensed: Copyright (C) 2021 Green Computing Group. Subject to the GNU General Public License 3.0
// @created : someday in 2016
// @modified: 23/08/2021
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "trie.h"

#ifndef __DUMMY_KEYSTORE_H__
#define __DUMMY_KEYSTORE_H__
//#define ALPHABET_SIZE (56)

// You may need to include your custom data structure (e.g., Tree, Map, Hash) at here.
// For example, I implemented a custom B-tree, 'my-tree.h' in the 'KV-engine/' directory.
// I need to include my custom data structure as
// #include "my-tree.h"                 

// Define a macro to use 'db_t' as a primary keyword for data engine
// You may need to overwrite this definition by replacing your data structure.
// For example: If we have a custom data-structure named 'Key_Value_Tree' in 'my-tree.h', then we define as
//      #define db_t Key_Value_Tree

// struct TrieNode
// { 
// 	struct TrieNode *children[ALPHABET_SIZE];
// 	bool isEndOfWord;
// 	char* value;
// 	bool lockHeld;
// 	int no_of_ends;
// };

#define db_t struct TrieNode

//
// @brief: Intialize new database - An adapter function
//
db_t *db_new();

//
// @brief: Put a (key,value) into database - An adapter function
//
bool db_put(db_t *db_data, char *key, char *val);

//
// @brief: Get a corresponding 'value' of 'key' from database - An adapter function
//
char* db_get(db_t *db_data, char *key);

char* get_json_result(int queries, int threads, int duration);

//
// @brief: Destroy database and Free memory - An adapter function
//
int db_free(db_t *db_data);

#endif
