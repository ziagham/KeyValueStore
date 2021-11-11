#include <stdbool.h>

typedef struct __lock {
    bool flag;
} TASLock;

void TAS_Init(TASLock *lock);
void TAS_Lock(TASLock *lock);
void TAS_Unlock(TASLock *lock);