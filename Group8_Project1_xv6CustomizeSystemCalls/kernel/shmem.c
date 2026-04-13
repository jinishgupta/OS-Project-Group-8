// shmem.c — Kernel Shared Memory subsystem for xv6
//
// Provides three operations exposed via system calls:
//   shmem_get(key, size) -> id    : create/find a shared region
//   shmem_at(id)         -> uva   : map region into current process
//   shmem_dt(uva)        -> 0/-1  : unmap/detach from current process

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "shmem.h"

// Global shared memory table protected by a single lock
static struct spinlock shmlock;
static struct shmregion shmtable[NSHMEM];

// Called once at kernel boot from main()
void
shmem_init(void)
{
  int i;
  initlock(&shmlock, "shmem");
  for(i = 0; i < NSHMEM; i++){
    shmtable[i].key      = 0;
    shmtable[i].refcount = 0;
    shmtable[i].kpage    = 0;
  }
}

// shmem_get: create or open a shared memory region identified by key.
// size is ignored beyond checking it is <= PGSIZE (one page max).
// Returns the region id (index) on success, -1 on failure.
int
shmem_get(int key, int size)
{
  int i;
  int free_slot = -1;

  if(key <= 0 || size <= 0 || size > PGSIZE)
    return -1;

  acquire(&shmlock);

  // Search for an existing region with this key
  for(i = 0; i < NSHMEM; i++){
    if(shmtable[i].key == key){
      // Found existing — just return its id
      release(&shmlock);
      return i;
    }
    if(shmtable[i].key == 0 && free_slot == -1)
      free_slot = i;
  }

  // No existing region — allocate a new one
  if(free_slot == -1){
    release(&shmlock);
    return -1;   // table full
  }

  // Allocate one physical page
  void *page = kalloc();
  if(page == 0){
    release(&shmlock);
    return -1;
  }
  memset(page, 0, PGSIZE);

  shmtable[free_slot].key      = key;
  shmtable[free_slot].refcount = 0;
  shmtable[free_slot].kpage    = page;

  release(&shmlock);
  return free_slot;
}

// shmem_at: map shared region 'id' into the calling process's address space.
// Returns the user virtual address on success, 0 on failure.
uint64
shmem_at(int id)
{
  struct proc *p = myproc();
  uint64 uva;
  void  *kpage;

  if(id < 0 || id >= NSHMEM)
    return 0;

  acquire(&shmlock);

  if(shmtable[id].key == 0 || shmtable[id].kpage == 0){
    release(&shmlock);
    return 0;
  }

  kpage = shmtable[id].kpage;

  // Pick a user virtual address just above the current process size,
  // aligned to a page boundary.
  uva = PGROUNDUP(p->sz);

  // Map the physical page (backing kpage) into the process page table
  // PTE_R | PTE_W | PTE_U
  if(mappages(p->pagetable, uva, PGSIZE,
              (uint64)kpage, PTE_R | PTE_W | PTE_U) < 0){
    release(&shmlock);
    return 0;
  }

  p->sz = uva + PGSIZE;
  shmtable[id].refcount++;

  release(&shmlock);
  return uva;
}

// shmem_dt: detach a shared region that was previously mapped at user VA 'uva'.
// Decrements the reference count; frees the physical page when count hits 0.
// Returns 0 on success, -1 on failure.
int
shmem_dt(uint64 uva)
{
  int i;
  struct proc *p = myproc();

  // uva must be page-aligned and within the process
  if(uva == 0 || uva % PGSIZE != 0 || uva >= p->sz)
    return -1;

  acquire(&shmlock);

  // Find which region this VA belongs to by matching the physical page
  uint64 pa = walkaddr(p->pagetable, uva);
  if(pa == 0){
    release(&shmlock);
    return -1;
  }

  for(i = 0; i < NSHMEM; i++){
    if(shmtable[i].kpage == (void *)pa){
      // Unmap the page from the process (do NOT free the physical page yet)
      uvmunmap(p->pagetable, uva, 1, 0);

      shmtable[i].refcount--;
      if(shmtable[i].refcount <= 0){
        // Last reference — free the physical page and clear the slot
        kfree(shmtable[i].kpage);
        shmtable[i].key      = 0;
        shmtable[i].refcount = 0;
        shmtable[i].kpage    = 0;
      }
      release(&shmlock);
      return 0;
    }
  }

  release(&shmlock);
  return -1;
}
