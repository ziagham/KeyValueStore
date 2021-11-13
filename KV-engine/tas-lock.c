#include <stdio.h>
#include <stdbool.h>
#include "tas-lock.h"

static bool test_and_set(bool *flag) {
   bool prior = *flag;
   *flag = true;
   return prior;
}

void tas_init_lock(TAS_lock *lock) {
   lock->held = false;
   return;
}

void tas_acquire_lock(TAS_lock *lock) {
   // while(true) {
   //    while(lock->held == true) continue;
   //    if(!test_and_set(&lock->held)) break;
   //  }

   // do {
   //    while (lock->held == true) continue;
   // } while (test_and_set(&lock->held));

   while (test_and_set(&lock->held));
   //printf("Lock TAS\n");
   return;
}

void tas_release_lock(TAS_lock *lock) {
   //printf("Unlock TAS\n");
   lock->held = false;
   return;
}

// void tas_init_lock(TAS_lock *lock) {
//    lock->held = false;
// }

// void tas_acquire_lock(TAS_lock *lock) {
//    //printf("Lock TAS\n");
//    while (TestAndSet(&lock->held))
//    ; // spin-wait (do nothing)
// }

// void tas_release_lock(TAS_lock *lock) {
//    //printf("Unlock TAS\n");
//    lock->held = false;
// }