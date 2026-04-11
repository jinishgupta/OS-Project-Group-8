#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    printf("testgetinfo: ");
    struct procinfo info;
    if (getprocinfo(&info) == 0) {
        printf("PID=%d  PPID=%d  State=%d  Name=%s\n",
               info.pid, info.ppid, info.state, info.name);
    } else {
        printf("  getprocinfo FAILED\n");
    }
    exit(0);
}
