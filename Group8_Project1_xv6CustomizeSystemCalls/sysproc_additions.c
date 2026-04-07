// Add these functions to xv6's existing sysproc.c file
// Location in xv6 repo: kernel/sysproc.c

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

struct procinfo {
    int pid;
    int ppid;
    int state;      // 1=SLEEPING, 2=RUNNABLE, 3=RUNNING
    char name[16];
};

uint64
sys_getprocinfo(void)
{
    uint64 addr;
    struct procinfo info;
    struct proc *p = myproc();

    argaddr(0, &addr);

    info.pid   = p->pid;
    info.ppid  = p->parent ? p->parent->pid : 0;
    info.state = p->state; 
    safestrcpy(info.name, p->name, sizeof(info.name));

    if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;
    return 0;
}
