/*
 * Copyright 2021 Koen van Greevenbroek
 * Subject to the GNU General Public License 3.0
 * 
 * A simple program to convert YCSB output to a format expected by the
 * `bench_client.c` program.
 *
 * The generated file starts with three parameters: `key_len`,
 * `val_len` (both in bytes) and `num_queries` describing the rest of
 * the file contents. The rest of the file is an array of `query`
 * structures, each containing only a key and a type.
 *
 * This program is very fragile, depending on the exact output of
 * YCSB. If the format of that output changes (for example by
 * adjusting some settings), adjust this program accordingly.
 *
 * The program takes two command line arguments: the name of the YCSB
 * trace file, and the name of the file to convert the trace to.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "convert.h"
#include "utilities.c"

const size_t key_len = NKEY;
const size_t val_len = NVAL;


int main(int argc, char **argv) {
    if (argc <= 2) {
        exit(-1);
    }

    // Start by parsing the YSCB file. It starts with a section
    // delineate by asterisks, containing some information about
    // trace. In particular, we need to start by reading the
    // `operationcount`.

    char *input = argv[1];
    if (input == NULL) {
        exit(-1);
    }

    FILE *input_fp;
    input_fp = fopen(input, "r");
    if (input_fp == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int64_t num_queries = 0;

    // Read the operationscount. This is given in the input file as a
    // line which looks like "operationscount"="n", where n is some
    // integer.
    while ((read = getline(&line, &len, input_fp)) != -1) {
        if (strstr(line, "operationcount")) {
            // The number starts on the 18th character.
            int success = sscanf(line + 18, "%ld", &num_queries);
            if (success != 1) {
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    if (num_queries == 0) {
        exit(EXIT_FAILURE);
    }

    // Now read the file again, actually parsing the queries.
    query *queries = malloc(sizeof(query) * num_queries);
    if (queries == NULL) {
        exit(EXIT_FAILURE);
    }
    input_fp = fopen(input, "r");
    if (input_fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int64_t i = 0;
    char key[30];
	char* hashed_key;

    //uint64 hashed_key;
    while ((read = getline(&line, &len, input_fp)) != -1) {
		//printf("line %s \n", line);
        if (strncmp(line, "READ", 4) == 0) {
            queries[i].type = (char) query_get;
            // The key is the third word in the line.
            sscanf(line, "%*s %*s %s", key);
			//printf("Key1: %s \n", removeSpecialCharacter(key));
			
            //hashed_key = CityHash64(key, 30);
			
			//uint64_t num = strtoull(key, NULL, 0);
			//printf("READ Key: %s \n", removeSpecialCharacter(b62_encode(num)));
			
			//printf("READ Key: %s \n", substr(key, 4, strlen(key)));
			
			hashed_key = generateAlphabetKey(key);
			char hashed_key_array[strlen(hashed_key)];
			
			//printf("READ Key: %s \n", hashed_key);
			
			strcpy(hashed_key_array, hashed_key);
            memset(queries[i].hashed_key, 0, NKEY);
            memcpy(queries[i].hashed_key, hashed_key_array, sizeof(hashed_key_array));
            i++;
        }
        if (strncmp(line, "UPDATE", 6) == 0) {
            queries[i].type = (char) query_put;
            // The key is the third word in the line.
            sscanf(line, "%*s %*s %s", key);
			//printf("Key2: %s \n", removeSpecialCharacter(key));
            
			//hashed_key = CityHash64(key, 30);
			
			hashed_key = generateAlphabetKey(key);
			char hashed_key_array[strlen(hashed_key)];
			
			//printf("UPDATE Key: %s \n", hashed_key);
			
			strcpy(hashed_key_array, hashed_key);
            memset(queries[i].hashed_key, 0, NKEY);
            memcpy(queries[i].hashed_key, hashed_key_array, sizeof(hashed_key_array));
            i++;
            // We don't care of the actual value; the benchmark just inserts garbage.
        }
    }

    fclose(input_fp);

    // Now we are ready write the queries to a file.

    char *datafile = argv[2];
    if (datafile == NULL) {
        exit(-1);
    }
    FILE *data_fp;
    data_fp = fopen(datafile, "w");

    fwrite(&key_len, sizeof(key_len), 1, data_fp);
    fwrite(&val_len, sizeof(val_len), 1, data_fp);
    fwrite(&num_queries, sizeof(num_queries), 1, data_fp);
    fwrite(queries, sizeof(query), num_queries, data_fp);

    free(queries);
    fclose(data_fp);
}
