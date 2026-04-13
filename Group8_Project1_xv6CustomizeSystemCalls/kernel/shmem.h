// shmem.h — Shared Memory Region definitions for xv6

#ifndef SHMEM_H
#define SHMEM_H

#include "types.h"

// Maximum number of shared memory regions (see param.h for NSHMEM)
struct shmregion {
    int    key;       // user-supplied key (0 = free slot)
    int    refcount;  // number of processes currently attached
    void  *kpage;     // kernel virtual address of the physical page
};

#endif // SHMEM_H
