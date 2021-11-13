#include <stdbool.h>

typedef struct __lock {
    bool held;
} TAS_lock;

// void tas_init_lock(bool *lock);
// void tas_acquire_lock(bool *lock);
// void tas_release_lock(bool *lock);

void tas_init_lock(TAS_lock *lock);
void tas_acquire_lock(TAS_lock *lock);
void tas_release_lock(TAS_lock *lock);