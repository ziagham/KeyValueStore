/*
 * Copyright 2021 Koen van Greevenbroek
 * Subject to the GNU General Public License 3.0
 *
 * See also documentation in convert.c.
 *
 * For the output of this program to work with the `bench_client`
 * program, it is important that the definitions in this file match
 * those for the `bench_client` program.
 */

#define NKEY 64
#define NVAL 256

enum query_types{
    query_put=0,
    query_get,
    query_del,
};

typedef struct __attribute__((__packed__)) {
    char hashed_key[NKEY];
    char type;
} query;
