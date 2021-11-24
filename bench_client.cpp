//
// @file    : bench_client.cpp
// @purpose : A implementation for a benchmark of an in-memory key-value storage
// @author  : Prof. Phuong Hoai Ha & Ibrahim Umar & Koen van Greevenbroek & Hung Ngoc Phan
// @project : [INF-3210 Green Computing] Coursework Assignment
// @licensed: Copyright (C) 2021 Green Computing Group. Subject to the GNU General Public License 3.0
// @created : someday in 2016
// @modified: 23/08/2021
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>            				// Standard system libraries
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>

#include <unistd.h>           				// Types and miscellaneous
#include <getopt.h>    
#include <inttypes.h> 

#include <time.h>             				// Timing libraries
#include <sys/time.h>
#include <sys/mman.h>

#include <sched.h>            				// Threading, signalling and scheduling
#include <pthread.h>
#include <signal.h>

#include "bench_common.h"    			 	// Custom definitions for benchmarks

#include "KV-engine/dummy-keystore.h"  		// Key-value storage to be benchmarked
///////////////////////////////////////////////////////////////////
// NOTICE: Wrap in extern "C" if your keystore is implemented in C.
// extern "C" {
//     #include "KV-engine/dummy-keystore.h"
// }
///////////////////////////////////////////////////////////////////

typedef struct {			  				// Default parameter for query execution at threads
	size_t tid;
	query *queries;

	size_t num_ops;
	size_t num_puts;
	size_t num_gets;
	size_t num_miss;
	size_t num_hits;

	double tput;
	double time;
} thread_param;

enum {
	R1 = 1,
	R2,
	R3,
};

pthread_mutex_t printmutex;
FILE *outputjson;
FILE *output_csv_result;

int nround;

static size_t key_len;				// Default parameter for queries settings
static size_t val_len;
static size_t num_queries;
static size_t num_threads = 1;

static float duration = 10.0;		// Duration of benchmark
static char* inputfile = NULL;		// Data file of queries
static char* jsonfile = NULL;		// Json file of output logging
static char* csv_result_file = NULL;		// Json file of output logging

db_t *db_data;						// Definition of key-value engine that is implemented in 'dummy-keystore'
volatile int stop = 0;				// Sigalarm's flag for timer

//
// @brief: Calculate period of time in the difference of seconds
//
void print_json(FILE *dest, int tid, int nput, int nget, int nhit, int nmiss, double ratio, double tput);

//
// @brief: Trigger flag 'stop' after a timeout. This is called by 'alarm(time_period)'
//
void trigger(int sig);

//
// @brief: Calculate period of time in the difference of seconds
//
double timeval_diff(struct timeval *start, struct timeval *end);

//
// @brief: Initialize all queries from the YCSB trace file before issuing
//
query *queries_init(char* filename);

//
// @brief: This is a thread-driven function, executing queries at a thread
//
void* queries_exec(void *param);

//
// @brief: Display argument configuration of execution
//
void usage(char* binname);

void print_csv_reslult(FILE *dest, int queries, int threads, int duration) {
	char* result = generate_csv_record(queries, threads, duration);
	fprintf(dest, "%s", result);
	fclose(dest);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// @brief: Main stream of execution
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	
	/////////////////////////////////////////////////////////////////////////////////
	// ARGUMENT LOADING AND PRELIMINARY INITIALIZATION
	/////////////////////////////////////////////////////////////////////////////////

	// Check if users input enough arguments for execution
	if (argc <= 1) {
		printf("argc:%d", argc);
		usage(argv[0]);
		exit(-1);
	}

	// Extract user-defined arguments for program execution
	signed char ch;
	while ((ch = getopt(argc, argv, "t:d:h:l:j:r:")) != -1) {
		switch (ch) {			
			case 't': num_threads 		= atoi(optarg); break;	// Parse number of threads
			case 'd': duration    		= atof(optarg); break;	// Parse duration for alarming
			case 'l': inputfile   		= optarg;       break;	// Parse filename of query data file
			case 'j': jsonfile    		= optarg;       break;	// Parse filename of output json for logging
			case 'r': csv_result_file	= optarg;		break;
			case 'h': usage(argv[0]); exit(0);    break;	// Print argument description for -h
			default:										// Other cases
				printf("optarg:%s\n", optarg);
				printf("ch:%d\n", ch);
				usage(argv[0]);
				exit(-1);
		}
	}

	// Terminate program as data file is invalid
	if (inputfile == NULL) {
		usage(argv[0]);
		exit(-1);
	}

	// Initialize json output for execution logging
	printf("jsonfile:%s\n", jsonfile);
	if (jsonfile != NULL) {
		outputjson = fopen(jsonfile, "w+");
		if (outputjson == NULL) {
			perror("can not open json file");
			exit(1);
		}
		fprintf(outputjson, "{\n");
	}

	if (csv_result_file != NULL) {
		output_csv_result = fopen(csv_result_file, "a");
		if (output_csv_result == NULL) {
			perror("can not open json result file");
			exit(1);
		}
	}

	// Load all queries from 'inputfile'
	query *queries = queries_init(inputfile);

	// Initialize thread definitions
	pthread_t threads[num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&printmutex, NULL);
	thread_param tp[num_threads];

	// Initialize key-value storage engine as a database
	db_data = db_new();


	/////////////////////////////////////////////////////////////////////////////////
	// ROUND 1: BENCHMARK ALL AVAILABLE THREADS CONCURRENTLY -- FIRST TIME
	/////////////////////////////////////////////////////////////////////////////////

	nround = 1;
	printf("\n\nFirst round of benchmark, ALL (%d) threads\n\n", (int)num_threads);

	// Execute queries by all of threads
	size_t t;
	for (t = 0; t < num_threads; t++) {
		
		// Configure thread's params for each thread
		tp[t].queries 	= queries + t * (num_queries / num_threads);
		tp[t].tid     	= t;
		tp[t].num_ops 	= num_queries / num_threads;
		tp[t].num_puts 	= tp[t].num_gets = tp[t].num_miss = tp[t].num_hits = 0;
		tp[t].time 		= tp[t].tput = 0.0;

		// Create a new thread and execute queries_exec() with thread's params
		int rc = pthread_create(
			&threads[t], 
			&attr, 
			queries_exec, (void *) &tp[t]
		);

		// Terminate program if fail to create a new thread
		if (rc) {
			perror("failed: pthread_create\n");
			exit(-1);
		}
	}

	// Activate a signaller: when duration expires, call trigger() to assign a flag 'stop' = 1
	signal(SIGALRM, trigger);
	alarm(duration);

	// Initialize result place_holder
	result_t result;
	result.grand_total_time = 0.0;
	result.total_time 		= 0.0;;
	result.total_tput 		= 0.0;
	result.total_hits 		= 0;
	result.total_miss 		= 0;
	result.total_gets 		= 0;
	result.total_puts 		= 0;
	result.num_threads 		= num_threads;

	// Collect experimental statistics from all of threads
	for (t = 0; t < num_threads; t++) {
		void *status;
		int rc = pthread_join(threads[t], &status);
		
		if (rc) {
			perror("error, pthread_join\n");
			exit(-1);
		}

		result.total_time = (result.total_time > tp[t].time) ? result.total_time : tp[t].time;
		result.total_tput += tp[t].tput;
		result.total_hits += tp[t].num_hits;
		result.total_miss += tp[t].num_miss;
		result.total_gets += tp[t].num_gets;
		result.total_puts += tp[t].num_puts;
	}
	result.grand_total_time += result.total_time;


	/////////////////////////////////////////////////////////////////////////////////
	// ROUND 2: BENCHMARK ONLY ONE THREAD
	/////////////////////////////////////////////////////////////////////////////////

	stop = 0;
	nround = 2;
	printf("\n\nSecond round of benchmark, 1 thread\n\n");

	// Execute queries by one thread
	for (t = 0; t < 1; t++) {
		
		// Configure thread's params for a thread
		tp[t].queries 	= queries + t * (num_queries / num_threads);
		tp[t].tid     	= t;
		tp[t].num_ops 	= num_queries / num_threads;
		tp[t].num_puts 	= tp[t].num_gets = tp[t].num_miss = tp[t].num_hits = 0;
		tp[t].time 		= tp[t].tput = 0.0;
		
		// Create a new thread and execute queries_exec() with thread's params
		int rc = pthread_create(&threads[t], &attr, queries_exec, (void *) &tp[t]);
		
		// Terminate program if fail to create a new thread
		if (rc) {
			perror("failed: pthread_create\n");
			exit(-1);
		}
	}

	// Activate a signaller: when duration expires, call trigger() to assign a flag 'stop' = 1
	signal(SIGALRM, trigger);
	alarm(duration);

	// Collect experimental statistics from the thread
	for (t = 0; t < 1; t++) {
		void *status;
		int rc = pthread_join(threads[t], &status);

		if (rc) {
				perror("error, pthread_join\n");
				exit(-1);
		}

		result.total_time = (result.total_time > tp[t].time) ? result.total_time : tp[t].time;
		result.total_tput += tp[t].tput;
		result.total_hits += tp[t].num_hits;
		result.total_miss += tp[t].num_miss;
		result.total_gets += tp[t].num_gets;
		result.total_puts += tp[t].num_puts;
	}
	result.grand_total_time += result.total_time;


	/////////////////////////////////////////////////////////////////////////////////
	// ROUND 3: BENCHMARK ALL AVAILABLE THREADS CONCURRENTLY -- SECOND TIME
	/////////////////////////////////////////////////////////////////////////////////

	stop = 0;
	nround = 3;
	printf("\n\nThird round of benchmark, ALL (%d) thread\n\n", (int)num_threads);

	// Execute queries by all of threads
	for (t = 0; t < num_threads; t++) {
		
		// Configure thread's params for each thread
		tp[t].queries 	= queries + t * (num_queries / num_threads);
		tp[t].tid     	= t;
		tp[t].num_ops 	= num_queries / num_threads;
		tp[t].num_puts 	= tp[t].num_gets = tp[t].num_miss = tp[t].num_hits = 0;
		tp[t].time 		= tp[t].tput = 0.0;

		// Create a new thread and execute queries_exec() with thread's params
		int rc = pthread_create(&threads[t], &attr, queries_exec, (void *) &tp[t]);
		
		// Terminate program if fail to create a new thread
		if (rc) {
			perror("failed: pthread_create\n");
			exit(-1);
		}
	}

	// Activate a signaller: when duration expires, call trigger() to assign a flag 'stop' = 1
	signal(SIGALRM, trigger);
	alarm(duration);

	// Collect experimental statistics from all of threads
	for (t = 0; t < num_threads; t++) {
		void *status;
		int rc = pthread_join(threads[t], &status);

		if (rc) {
				perror("error, pthread_join\n");
				exit(-1);
		}

		result.total_time = (result.total_time > tp[t].time) ? result.total_time : tp[t].time;
		result.total_tput += tp[t].tput;
		result.total_hits += tp[t].num_hits;
		result.total_miss += tp[t].num_miss;
		result.total_gets += tp[t].num_gets;
		result.total_puts += tp[t].num_puts;
	}
	result.grand_total_time += result.total_time;


	/////////////////////////////////////////////////////////////////////////////////
	// CLEAN UP AND FINALIZATION
	/////////////////////////////////////////////////////////////////////////////////

	// Destroy database
	db_free(db_data);
	
	
	// Print out the summary statistics to standard I/O
	printf("total_time = %.2f\n", result.grand_total_time);
	printf("total_tput = %.2f\n", (float) (result.total_gets+result.total_puts)/result.grand_total_time);
	printf("total_hitratio = %.4f\n", (float) result.total_hits / result.total_gets);

	// Print out the summary statistics to json file and Close the filestream of json
	fprintf(outputjson, "\t\"total\": {\n");
	fprintf(outputjson, "\t\t\"time\": \"%.2f\",\n", result.grand_total_time);
	fprintf(outputjson, "\t\t\"tput\": \"%.2f\",\n", (float) (result.total_gets+result.total_puts)/result.grand_total_time);
	fprintf(outputjson, "\t\t\"hitratio\": \"%.4f\"\n", (float) result.total_hits / result.total_gets);
	fprintf(outputjson, "\t}\n");
	fprintf(outputjson, "}\n");
	fclose(outputjson);

	print_csv_reslult(output_csv_result,(int)num_queries, num_threads, duration);
	
	// Finalize all threads
	pthread_attr_destroy(&attr);
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF UTILITY FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// @brief: Calculate period of time in the difference of seconds
//
void print_json(FILE *dest, int tid, int nput, int nget, int nhit, int nmiss, double ratio, double tput) {
	const char *tstr = (char*) malloc(3);
	switch(nround) {
	case R1: tstr = "r1"; break;
	case R2: tstr = "r2"; break;
	case R3: tstr = "r3"; break;
	default: printf("unknown round...\n"); exit(1); break;
	}

	fprintf(dest, "\t\"%s-%d\": {\n", tstr, tid);
	fprintf(dest, "\t\t\"nput\": \"%d\",\n", nput);
	fprintf(dest, "\t\t\"nget\": \"%d\",\n", nget);
	fprintf(dest, "\t\t\"nhit\": \"%d\",\n", nhit);
	fprintf(dest, "\t\t\"nmiss\": \"%d\",\n", nmiss);
	fprintf(dest, "\t\t\"nget\": \"%.4f\",\n", ratio);
	fprintf(dest, "\t\t\"tput\": \"%.2f\"\n", tput);
	fprintf(dest, "\t},\n");
}

//
// @brief: Trigger flag 'stop' after a timeout. This is called by 'alarm(time_period)'
//
void trigger(int sig) {
	stop = 1;
}

//
// @brief: Calculate period of time in the difference of seconds
//
double timeval_diff(struct timeval *start, struct timeval *end) {
	double r = end->tv_sec - start->tv_sec;

	/* Calculate the microsecond difference */
	if (end->tv_usec > start->tv_usec)
		r += (end->tv_usec - start->tv_usec)/1000000.0;
	else if (end->tv_usec < start->tv_usec)
		r -= (start->tv_usec - end->tv_usec)/1000000.0;
	return r;
}

//
// @brief: Initialize all queries from the YCSB trace file before issuing
//
query *queries_init(char* filename) {
	FILE *input;

	input = fopen(filename, "rb");
	if (input == NULL) {
		perror("can not open file");
		perror(filename);
		exit(1);
	}

	int n;
	n = fread(&key_len, sizeof(key_len), 1, input);
	if (n != 1)
		perror("fread error");
	n = fread(&val_len, sizeof(val_len), 1, input);
	if (n != 1)
		perror("fread error");
	n = fread(&num_queries, sizeof(num_queries), 1, input);
	if (n != 1)
		perror("fread error");

	printf("trace(%s):\n", filename);
	printf("\tkey_len = %zu\n", key_len);
	printf("\tval_len = %zu\n", val_len);
	printf("\tnum_queries = %zu\n", num_queries);
	printf("\n");

	// query *queries = (query*) malloc(num_queries);
	query *queries = (query *) malloc(sizeof(query) * num_queries);
	if (queries == NULL) {
		perror("not enough memory to init queries\n");
		exit(-1);
	}

	size_t num_read;
	num_read = fread(queries, sizeof(query), num_queries, input);
	if (num_read < num_queries) {
		fprintf(stderr, "num_read: %zu\n", num_read);
		perror("can not read all queries\n");
		fclose(input);
		exit(-1);
	}

	fclose(input);
	printf("queries_init...done\n");
	return queries;
}

//
// @brief: This is a thread-driven function, executing queries at a thread
//
void* queries_exec(void *param) {
	/* get the key-value store structure */
	struct timeval tv_s, tv_e;

	thread_param* p = (thread_param*) param;

	pthread_mutex_lock (&printmutex);
	printf("start benching using thread%" PRIu64 "\n", p->tid);
	pthread_mutex_unlock (&printmutex);

	query* queries = p->queries;
	p->time = 0;

	/* Strictly obey the timer */
	while (!stop) {
		gettimeofday(&tv_s, NULL);  // start timing
		size_t i;
		for (i = 0 ; i < p->num_ops; i++) {
			// enum query_types type = (int) (queries[i].type);
			enum query_types type = static_cast<query_types>(queries[i].type);
			
			char *key = queries[i].hashed_key;
			char buf[val_len];

			if (type == query_put) {
				db_put(db_data, key, buf);
				p->num_puts++;
			} else if (type == query_get) {
				char *val = db_get(db_data, key);
				p->num_gets++;
				if (val == NULL) {
					// cache miss, put something (garbage) in cache
					p->num_miss++;
					db_put(db_data, key, buf);
				} else {
					//free(val);
					p->num_hits++;
				}
			} else {
				fprintf(stderr, "unknown query type\n");
			}

			/* Strictly obey the timer */
			if(stop) break;
		}
		gettimeofday(&tv_e, NULL);  // stop timing
		p->time += timeval_diff(&tv_s, &tv_e);
	}

	size_t nops = p->num_gets + p->num_puts;
	p->tput = nops / p->time;

	pthread_mutex_lock (&printmutex);
	printf("thread%" PRIu64 " gets %" PRIu64 " items in %.2f sec \n",
				 p->tid, nops, p->time);
	printf("#put = %zu, #get = %zu\n", p->num_puts, p->num_gets);
	printf("#miss = %zu, #hits = %zu\n", p->num_miss, p->num_hits);
	printf("hitratio = %.4f\n",   (float) p->num_hits / p->num_gets);
	printf("tput = %.2f\n",  p->tput);
	printf("\n");
	print_json(outputjson, p->tid, p->num_puts, p->num_gets, p->num_hits, p->num_miss, p->num_hits / p->num_gets, p->tput);
	pthread_mutex_unlock (&printmutex);

	printf("queries_exec...done\n");
	pthread_exit(NULL);
	// printf("End benching using thread%" PRIu64 "\n", p->tid);
}

//
// @brief: Display argument configuration of execution
//
void usage(char* binname) {
	printf("%s [-t #] [-b #] [-l trace] [-d #] [-h]\n", binname);
	printf("\t-t #: number of working threads, by default %" PRIu64 "\n", num_threads);
	printf("\t-d #: duration of the test in seconds, by default %f\n", duration);
	printf("\t-l trace: e.g., /path/to/ycsbtrace, required\n");
    printf("\t-j json file used for logging\n");
	printf("\t-j file used for results\n");
	printf("\t-h  : show usage\n");
}






