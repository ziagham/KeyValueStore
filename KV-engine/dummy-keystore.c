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
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define HB_ENERGY_IMPL                  // Use heartbeat energy impl.
#define HB_INTERVAL 10000               // HB Interval (in useconds)
#define USE_POET                        // Use power and performance control by POET
#include <heartbeats/hb-energy.h>       // Heartbeats and POET
#include <heartbeats/heartbeat-accuracy-power.h>
#include <poet/poet.h>
#include <poet/poet_config.h>

#define PREFIX "DUMMYKEY"               // Define prefix for configurations
#include "dummy-keystore.h"             // Template class for benchmarking


int stop_heartbeat = 0;                 // A flag to control heartbeat setting in heartbeat_timer_thread()
heartbeat_t* heart;                     // A parameter 'heart' in heartbeat settings

poet_state* state;                      // System control states and CPU configurations
static poet_control_state_t* control_states;
static poet_cpu_state_t* cpu_states;
pthread_mutex_t m_lock;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// START POET & HEARTBEAT
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// @brief: A thread function to register heartbeat settings to the running system.
//         This function will be called when the database is initialized in db_new(),
//         and will terminate when we set 'stop_heartbeat=1' in db_free().
//
void *heartbeat_timer_thread(void *arg){

    int i = 0;
    while(!stop_heartbeat){
        
        // Register a heartbeat
        heartbeat_acc(heart, i, 1);
        
        // Apply control state with poet
        #ifdef USE_POET
            poet_apply_control(state);
        #endif

        i++;
        usleep(HB_INTERVAL);

    }

    pthread_exit(NULL);
}

//
// @brief: A function to apply control states (of both poet and heartbeat) to the running system.
//         This function will be called when the database is initialized in db_new().
//
void hb_poet_init() {
    float min_heartrate;
    float max_heartrate;
    int window_size;
    double power_target;
    unsigned int nstates;
    
    // Get/Initialize MIN_HEART_RATE param
    if(getenv(PREFIX"_MIN_HEART_RATE") == NULL) {
        min_heartrate = 0.0;
    } else {
        min_heartrate = atof(getenv(PREFIX"_MIN_HEART_RATE"));
    } 
    
    // Get/Initialize MAX_HEART_RATE param
    if(getenv(PREFIX"_MAX_HEART_RATE") == NULL) {
        max_heartrate = 100.0;
    } else {
        max_heartrate = atof(getenv(PREFIX"_MAX_HEART_RATE"));
    }
    
    // Get/Initialize WINDOW_SIZE param
    if(getenv(PREFIX"_WINDOW_SIZE") == NULL) {
        window_size = 30;
    } else {
        window_size = atoi(getenv(PREFIX"_WINDOW_SIZE"));
    }
    
    // Get/Initialize POWER_TARGET param
    if(getenv(PREFIX"_POWER_TARGET") == NULL) {
        power_target = 70;
    } else {
        power_target = atof(getenv(PREFIX"_POWER_TARGET"));
    }

    // Remind user to 'export HEARTBEAT_ENABLED_DIR=/tmp'
    if(getenv("HEARTBEAT_ENABLED_DIR") == NULL) {
        fprintf(stderr, "ERROR: need to define environment variable HEARTBEAT_ENABLED_DIR (see README)\n");
        exit(1);
    }

    // Initialize runtime control with heartbeat using heart-rates and power-target
    printf("init heartbeat with %f %f %d\n", min_heartrate, max_heartrate, window_size);
    heart = heartbeat_acc_pow_init(window_size, 100, "heartbeat.log",
                                   min_heartrate, max_heartrate,
                                   0, 100,
                                   1, hb_energy_impl_alloc(), power_target, power_target);
    if (heart == NULL) {
        fprintf(stderr, "Failed to init heartbeat.\n");
        exit(1);
    }

    #ifdef USE_POET
        // Get 'control_states' and 'nstates' from control_config file
        if (get_control_states("config/control_config", &control_states, &nstates)) {
            fprintf(stderr, "Failed to load control states.\n");
            exit(1);
        }

        // Get 'cpu_states' and 'nstates' from cpu_config file
        if (get_cpu_states("config/cpu_config", &cpu_states, &nstates)) {
            fprintf(stderr, "Failed to load cpu states.\n");
            exit(1);
        }

        // Apply user-defined system configuration with poet_init()
        state = poet_init(heart, nstates, control_states, cpu_states, &apply_cpu_config, &get_current_cpu_state, 1, "poet.log");
        if (state == NULL) {
            fprintf(stderr, "Failed to init poet.\n");
            exit(1);
        }
    #endif

    printf("heartbeat init'd\n");
}

//
// @brief: A function to destroy control states (of both poet and heartbeat) in the running system.
//         This function will be called when the database is freed in db_free().
//
void hb_poet_finish() {

    #ifdef USE_POET
        poet_destroy(state);
        free(control_states);
        free(cpu_states);
    #endif

    heartbeat_finish(heart);
    printf("heartbeat finished\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A TEMPLATE TO PLUG-IN USER-CUSTOMIZED DATA STRUCTURES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

pthread_t hb_thread_handler;        // A global thread to run heartbeat_timer_thread()
time_t time_start, time_end;        // Mark the start-time and end-time of benchmark
int op_count = 0;                   // Counter of queries

int get_index(char ch)
{
	return ch-65 - (6 & -(ch>='a'));
}

//
// @brief: Intialize new database - An adapter function
//
db_t *db_new() {

    // Start timing
    time_start = time(NULL);

    // Initialize runtime control (of both poet and heartbeat)
    hb_poet_init();

    // Register heartbeat settings. This run in background with a thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc = pthread_create(&hb_thread_handler, &attr, heartbeat_timer_thread, NULL);
    if (rc) {
        perror("failed: HB thread create\n");
        exit(-1);
    }

    //////////////////////////////////////////////////////////
    // [Custom-as-yourself] Initialize a database
    //////////////////////////////////////////////////////////

    db_t* database = (db_t*) malloc(sizeof(db_t));

    database->isEndOfWord = false; 
	database->no_of_ends=0;

	for (int i = 0; i < 52; i++) 
		database->children[i] = NULL; 

    return database;
}

db_t *new_node() {
    //struct TrieNode* pNode = (struct TrieNode*) malloc(sizeof(struct TrieNode));
    db_t* pNode = (db_t*) malloc(sizeof(db_t));

    pNode->isEndOfWord = false; 
	pNode->no_of_ends=0;

	for (int i = 0; i < 52; i++) 
		pNode->children[i] = NULL; 

	return pNode; 
}

//
// @brief: Put a (key,value) into database - An adapter function
//
bool db_put(db_t *db_data, char *key, char *val) {

    // Update the counter of queries
    op_count++;

    int length = strlen(key);
    for (int i = 0; i < length; i++)
	{
        printf("key -> %s \n", key);
    }

    //////////////////////////////////////////////////////////
    // [Custom-as-yourself] Insert (key,value) into database
    //////////////////////////////////////////////////////////

    // Call function in your data-structure 
    // to insert a ('key','value') into your database 'db_data'

    // printf("op_count %d \n", op_count);
    
    // pthread_mutex_lock(&m_lock);
    // bool isThere=true;
    // db_t *curNode = db_data;
    // int length = strlen(key);

	// for (int i = 0; i < length; i++)
	// {
    //     printf("key %s \n", key);
	// 	int index = get_index(key[i]);
    //     printf("index %d %d \n", index, i);

	// 	if (!curNode->children[index])
	// 	{
	// 		curNode->children[index] = new_node();
	// 		curNode->children[index]->children[52] = curNode;
	// 		isThere=false;
	// 	}
	// 	curNode->children[index]->no_of_ends+=1;
	// 	curNode = curNode->children[index];
	// }

	// bool isEnd=curNode->isEndOfWord; 
	// curNode->isEndOfWord = true;
	// curNode->value=val;

	// if(isEnd)
	// {
	// 	for (int i = length-1; i>=0; i--) 
	// 	{ 
	// 		curNode->no_of_ends-=1; 
	// 		curNode = curNode->children[52];

	// 	} 
	// }
	// pthread_mutex_unlock(&m_lock);
	// return isThere && isEnd;
    return true;
}

//
// @brief: Get a corresponding 'value' of 'key' from database - An adapter function
//
char* db_get(db_t *db_data, char *key) {
    
    // Update the counter of queries
    op_count++;

    //////////////////////////////////////////////////////////
    // [Custom-as-yourself] Get value of a key from database
    //////////////////////////////////////////////////////////

    // Call function in your data-structure 
    // to get a corresponding value of a 'key' from your database 'db_data'
    
    // Store the found value of 'key' in char* and return it
    //* query_result;

    //db_t *curNode = db_data;
	//int length = strlen(key);

	//for (int i = 0; i<length; i++) 
	//{
		//int index = get_index(key[i]);

		//if (!curNode->children[index]) 
			//return (char*)"";

		//curNode = curNode->children[index]; 
	//}

	//if(curNode->value!=NULL)
		//return curNode->value;
        
	//return (char*)"";

    char* query_result = "This is the value of the given key, which is found from db_data";

    return query_result;
}

//
// @brief: Destroy database and Free memory - An adapter function
//
int db_free(db_t *db_data) {

    // Stop heartbeat settings in a background thread
    stop_heartbeat = 1;
    int rc = pthread_join(hb_thread_handler, NULL);
    if (rc) {
        perror("error, pthread_join\n");
        exit(-1);
    }

    // Terminate runtime control (of both poet and heartbeat)
    hb_poet_finish();

    // Stop timing and Calculate total running time
	time_end = time(NULL);
    double elapsed_time = difftime(time_end, time_start);
	printf("Total time %.2f seconds.\n", elapsed_time);

    // Get overall power of entire program execution
    double global_power = hb_get_global_power(heart);
    printf("Global power is: %.2f\n", global_power);

    // Get total energy of entire program execution (in Joule)
    double total_energy = global_power * elapsed_time;
	printf("Total Joule: %.2fJ\n", total_energy );

	// Get the total amount of queries
	printf("Opcount: %d\n",op_count);

    //////////////////////////////////////////////////////////
    // [Custom-as-yourself] Destroy database and Free memory
    //////////////////////////////////////////////////////////

    // Call function in your data-structure 
    // to destroy your database and free up memory allocation
    
    return 0;
}

