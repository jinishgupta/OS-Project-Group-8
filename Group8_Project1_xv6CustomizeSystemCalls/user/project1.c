#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    printf("\n--- Starting Project 1 Demonstrations ---\n");
    
    printf("\n1. Testing Process Feature:\n");
    getproccount();

    printf("\n2. Testing IPC Feature:\n");
    sendmsg();

    printf("\n3. Testing Lock Feature:\n");
    createlock();

    printf("\n4. Testing Thread Feature:\n");
    threadcreate();

    printf("\n5. Testing Signal Feature:\n");
    sendsignal();

    printf("\n--- Demonstrations Complete ---\n");
    exit(0);
}
