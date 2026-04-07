// Location in xv6 repo: user/testcalls.c
// Add "testcalls" to the UPROGS list in the Makefile.
//
// Compile & run inside xv6 shell:
//   $ testcalls

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct procinfo {
    int  pid;
    int  ppid;
    int  state;
    char name[16];
};

int main(void)
{
    printf("\n=== xv6 Custom System Call Demo ===\n\n");

    printf("-- Test 1: getprocinfo --\n");
    struct procinfo info;
    if (getprocinfo(&info) == 0) {
        printf("  PID=%d  PPID=%d  State=%d  Name=%s\n",
               info.pid, info.ppid, info.state, info.name);
    } else {
        printf("  getprocinfo FAILED\n");
    }

    printf("\n=== All tests completed ===\n");
    exit(0);
}
