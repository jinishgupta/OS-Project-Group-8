#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sem.h"

struct sem semtable[NSEM];
static struct spinlock semtable_lock;

// Called once at boot from main()
void
seminit(void)
{
  initlock(&semtable_lock, "semtable");
  for(int i = 0; i < NSEM; i++){
    initlock(&semtable[i].lock, "sem");
    semtable[i].used = 0;
  }
}

// Create and initialize a new semaphore.
// Returns semaphore ID (index 0..NSEM-1) on success, -1 on failure.
int
sem_get(int key, int initval)
{
  if(initval < 0)
    return -1;

  acquire(&semtable_lock);
  for(int i = 0; i < NSEM; i++){
    if(!semtable[i].used){
      semtable[i].used     = 1;
      semtable[i].key      = key;
      semtable[i].value    = initval;
      semtable[i].nwaiting = 0;
      release(&semtable_lock);
      return i;
    }
  }
  release(&semtable_lock);
  return -1;
}

//wait operation: decrement; block if value becomes negative.
int
sem_wait(int id)
{
  if(id < 0 || id >= NSEM)
    return -1;

  struct sem *s = &semtable[id];
  acquire(&s->lock);

  if(!s->used){
    release(&s->lock);
    return -1;
  }

  s->value--;
  while(s->value < 0){
    s->nwaiting++;
    sleep(s, &s->lock);
    s->nwaiting--;
    if(!s->used){
      release(&s->lock);
      return -1;
    }
  }

  release(&s->lock);
  return 0;
}

//post operation: increment and wake one waiter.
int
sem_post(int id)
{
  if(id < 0 || id >= NSEM)
    return -1;

  struct sem *s = &semtable[id];
  acquire(&s->lock);

  if(!s->used){
    release(&s->lock);
    return -1;
  }

  s->value++;
  wakeup(s);
  release(&s->lock);
  return 0;
}

// Destroy the semaphore entirely. Any process still blocked in sem_wait will wake up and get -1.
int
sem_close(int id)
{
  if(id < 0 || id >= NSEM)
    return -1;

  struct sem *s = &semtable[id];
  acquire(&s->lock);

  if(!s->used){
    release(&s->lock);
    return -1;
  }

  s->used = 0;
  wakeup(s);
  release(&s->lock);
  return 0;
}
