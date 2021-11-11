#include <stdbool.h>
#include "tas-lock.h"

bool TestAndSet(TASLock *lock, bool newValue) {
   bool prior = lock->flag;
   lock->flag = newValue;
   return prior;
}

void TAS_Init(TASLock *lock) {
   // 0 indicates that lock is available, 1 that it is held
   lock->flag = false;
}

void TAS_Lock(TASLock *lock) {
   while (TestAndSet(lock, true) == true)
   ; // spin-wait (do nothing)
}

void TAS_Unlock(TASLock *lock) {
   lock->flag = false;
}