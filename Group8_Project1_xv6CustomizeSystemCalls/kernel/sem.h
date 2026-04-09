// kernel/sem.h — semaphore descriptor

#ifndef XV6_SEM_H
#define XV6_SEM_H

struct sem {
  struct spinlock lock;
  int    key;       // user-supplied key
  int    value;     // current semaphore count
  int    used;      // 1 if this slot is allocated
  int    nwaiting;  // number of processes blocked in sem_wait
};

#endif // XV6_SEM_H
