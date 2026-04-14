#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "signal.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trying(void)
{
  return 10;
}

uint64
sys_tryint(void)
{
  int x;
  argint(0, &x);   
  return x * 2;
}

uint64
sys_semget(void)
{
  int key, initval;
  argint(0, &key);
  argint(1, &initval);
  return sem_get(key, initval);
}

uint64
sys_semwait(void)
{
  int id;
  argint(0, &id);
  return sem_wait(id);
}
// ============================================================
// xv6 Signal System Calls — Kernel Side
// ============================================================

uint64
sys_signal_init(void)
{
  struct proc *p = myproc();
  int i;

  if(p == 0)
    return -1;

  for(i = 0; i < NSIG; i++){
    p->signal_handlers[i] = SIG_DFL;
  }
  p->pending_signals = 0;

  printf("[signal_init] PID %d: signal table initialised\n", p->pid);
  return 0;
}

uint64
sys_signal_send(void)
{
  int pid, signum;
  struct proc *p;
  extern struct proc proc[NPROC];

  argint(0, &pid);
  argint(1, &signum);

  if(signum <= 0 || signum >= NSIG)
    return -1;

  // Find the target process
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid && p->state != UNUSED){
      p->pending_signals |= (1 << signum);
      printf("[signal_send] Signal %d sent to PID %d\n", signum, pid);
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  printf("[signal_send] PID %d not found\n", pid);
  return -1;
}

uint64
sys_signal_handle(void)
{
  int signum;
  uint64 handler_addr;

  argint(0, &signum);
  argaddr(1, &handler_addr);

  if(signum <= 0 || signum >= NSIG)
    return -1;

  struct proc *p = myproc();
  p->signal_handlers[signum] = (sighandler_t)handler_addr;

  printf("[signal_handle] PID %d registered handler for signal %d\n",
          p->pid, signum);
  return 0;
}

void
dispatch_signals(struct proc *p)
{
  int i;
  sighandler_t handler;

  for(i = 1; i < NSIG; i++){
    if(p->pending_signals & (1 << i)){
      p->pending_signals &= ~(1 << i);   // clear the bit
      handler = p->signal_handlers[i];

      if(handler == SIG_IGN){
        printf("[dispatch] PID %d ignored signal %d\n", p->pid, i);
        continue;
      }
      if(handler == SIG_DFL){
        // Default action: kill the process
        printf("[dispatch] PID %d: default action for signal %d — killing\n",
                p->pid, i);
        setkilled(p);
        continue;
      }
      
      // Call user handler (Note: direct call from kernel will crash in real xv6-riscv)
      // I'm keeping the user's logic but using the correct print function.
      printf("[dispatch] PID %d: invoking user handler for signal %d\n",
              p->pid, i);
      handler(i);
    }
  }
}

uint64
sys_sempost(void)
{
  int id;
  argint(0, &id);
  return sem_post(id);
}

uint64
sys_semclose(void)
{
  int id;
  argint(0, &id);
  return sem_close(id);
}

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

uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  return p->parent ? p->parent->pid : 0;
}

uint64
sys_yield(void)
{
  yield();
  return 0;
}

uint64
sys_getfreeprocs(void)
{
  struct proc *p;
  extern struct proc proc[NPROC];
  int count = 0;
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->state == UNUSED)
      count++;
    release(&p->lock);
  }
  return count;
}

uint64
sys_clone(void)
{
  // Basic thread-like clone: for now, just fork (no shared memory)
  // In a full implementation, share address space
  return kfork();
}

uint64
sys_mutex_create(void)
{
  // Simple mutex creation: return a fixed ID for demo
  // In full impl, manage a pool
  return 0;  // Dummy ID
}

uint64
sys_mutex_lock(void)
{
  int id;
  argint(0, &id);
  // Dummy lock
  return 0;
}

uint64
sys_mutex_unlock(void)
{
  int id;
  argint(0, &id);
  // Dummy unlock
  return 0;
}

uint64
sys_sigmask(void)
{
  int signum, block;
  argint(0, &signum);
  argint(1, &block);
  struct proc *p = myproc();
  if (block) {
    p->sigmask |= (1 << signum);
  } else {
    p->sigmask &= ~(1 << signum);
  }
  return 0;
}
// ===========================================================
// Thread System Calls
// ===========================================================
uint64 sys_thread_create(void) {
  printf("Kernel: Executing thread_create...\n");
  // The actual heavy lifting (similar to fork, but sharing vm) goes here.
  // Usually involves calling a custom clone() function in proc.c
  return 1; // Return dummy thread ID
}

uint64 sys_tjoin(void) {
  int target_tid;
  
  // Grab the thread ID passed from user space
  argint(0, &target_tid);
  
  printf("Kernel: Executing tjoin to wait for thread %d...\n", target_tid);
  // Logic to put the calling thread to sleep until target_tid terminates
  return 0; 
}

uint64 sys_texit(void) {
  printf("Kernel: Executing texit...\n");
  // Logic to clean up the specific thread's stack without killing the whole process
  return 0; 
}

// ============================================================
// Shared Memory System Calls
// ============================================================

uint64
sys_shmget(void)
{
  int key, size;
  argint(0, &key);
  argint(1, &size);
  return shmem_get(key, size);
}

uint64
sys_shmat(void)
{
  int id;
  argint(0, &id);
  return shmem_at(id);
}

uint64
sys_shmdt(void)
{
  uint64 addr;
  argaddr(0, &addr);
  return shmem_dt(addr);
}
