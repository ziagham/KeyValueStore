//
// @file    : bench_common.h
// @purpose : A header file containing the common structures of benchmark
// @author  : Prof. Phuong Hoai Ha & Ibrahim Umar & Koen van Greevenbroek & Hung Ngoc Phan
// @project : [INF-3210 Green Computing] Coursework Assignment
// @licensed: Copyright (C) 2021 Green Computing Group. Subject to the GNU General Public License 3.0
// @created : someday in 2016
// @modified: 23/08/2021
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BENCH_COMMON_H_
#define _BENCH_COMMON_H_

//
// @brief: The size of key, value in bytes
//
#define NKEY 16
#define NVAL 32

//
// @brief: A dictionary of query types
//
enum query_types{   
    query_put=0,
    query_get,
    query_del,
};

//
// @brief: A defined format of each query. Each query contains a key and a type.
//
typedef struct __attribute__((__packed__)) {
    char hashed_key[NKEY];
    char type;
} query;

//
// @brief: A placeholder to store the output statistics from the benchmark
//
typedef struct __attribute__((__packed__)) {
    double grand_total_time;
    double total_tput;
    double total_time;
    size_t total_hits;
    size_t total_miss;
    size_t total_gets;
    size_t total_puts;
    size_t num_threads;
} result_t;

#endif
