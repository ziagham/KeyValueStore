#include "ttas-lock.h"

static bool test_and_set(bool *flag) {
   bool prior = *flag;
   *flag = true;
   return prior;
}

void acquire_lock(struct TrieNode *node) {
   while(true) {
      while(node->lockHeld == true);
      if(!test_and_set(&node->lockHeld)) break;
    }
    return;
}

void release_lock(struct TrieNode *node) {
   node->lockHeld = false;
   return;
}