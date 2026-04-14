#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    struct procinfo info;
    if (getprocinfo(&info) == 0) {
        printf("testgetinfo: PID=%d  State=%d  Name=%s\n",
               info.pid, info.state, info.name);
    } else {
        printf("  getprocinfo FAILED\n");
    }

    printf("testgetinfo: Parent PID=%d\n", getppid());
    printf("testgetinfo: Free process slots=%d\n", getfreeprocs());
    
    int yield_ret = yield();
    printf("testgetinfo: Yield returned=%d\n", yield_ret);

    exit(0);
}
