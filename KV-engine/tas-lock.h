#include <stdbool.h>

#define LOCKED true
#define TAS_lock_Default { false }

typedef struct lock_s {
    bool held;
} TAS_lock;

//bool test_and_set(bool *flag);
void tas_acquire_lock(TAS_lock *lock);
void tas_release_lock(TAS_lock *lock);