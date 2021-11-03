//
// @file    : dummy-keystore.c
// @purpose : A implementation for a dummy key-value engine
// @author  : Prof. Phuong Hoai Ha & Ibrahim Umar & Koen van Greevenbroek & Hung Ngoc Phan
// @project : [INF-3210 Green Computing] Coursework Assignment
// @licensed: Copyright (C) 2021 Green Computing Group. Subject to the GNU General Public License 3.0
// @created : someday in 2016
// @modified: 23/08/2021
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>                     // Standard libraries
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "keystore-without-poet.h"             // Template class for benchmarking

pthread_mutex_t m_lock;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A TEMPLATE TO PLUG-IN USER-CUSTOMIZED DATA STRUCTURES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

pthread_t hb_thread_handler;        // A global thread to run heartbeat_timer_thread()

int get_index(char ch)
{
	return ch-65 - (6 & -(ch>='a'));
}

//
// @brief: Intialize new database - An adapter function
//
db_t *db_new() {

    db_t* database = (db_t*) malloc(sizeof(db_t));

    database->isEndOfWord = false; 
	database->no_of_ends=0;

	for (int i = 0; i < 52; i++) 
		database->children[i] = NULL; 

    return database;
}

//
// @brief: Put a (key,value) into database - An adapter function
//
bool db_put(db_t *db_data, char *key, char *val) {
    pthread_mutex_lock(&m_lock);
    bool isThere=true;
    db_t *curNode = db_data;
    int length = strlen(key);

	for (int i = 0; i < length; i++)
	{
		int index = get_index(key[i]);

		if (!curNode->children[index])
		{
			curNode->children[index] = db_new();
			curNode->children[index]->children[52] = curNode;
			isThere=false;
		}
		curNode->children[index]->no_of_ends+=1;
		curNode = curNode->children[index];
	}

	bool isEnd=curNode->isEndOfWord; 
	curNode->isEndOfWord = true;
	curNode->value=val;

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
    // return true;
}

//
// @brief: Get a corresponding 'value' of 'key' from database - An adapter function
//
char* db_get(db_t *db_data, char *key) {
    db_t *curNode = db_data;
	int length = strlen(key);

	for (int i = 0; i<length; i++) 
	{
		int index = get_index(key[i]);

		if (!curNode->children[index]) 
			return (char*)"";

		curNode = curNode->children[index]; 
	}

	if(curNode->value!=NULL)
		return curNode->value;
        
	return (char*)"";

    // char* query_result = "This is the value of the given key, which is found from db_data";

    // return query_result;
}
