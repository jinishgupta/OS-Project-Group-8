#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    printf("\n--- Starting Thread Management Test ---\n");
    
    printf("\n[Test 1] Calling thread_create()...\n");
    int tid = thread_create();
    printf("-> thread_create() returned Thread ID: %d\n", tid);

    printf("\n[Test 2] Calling tjoin() on Thread ID %d...\n", tid);
    int join_res = tjoin(tid);
    printf("-> tjoin() returned: %d\n", join_res);

    printf("\n[Test 3] Calling texit()...\n");
    int exit_res = texit();
    printf("-> texit() returned: %d\n", exit_res);

    printf("\n--- Thread Management Test Complete ---\n");
    exit(0);
}
