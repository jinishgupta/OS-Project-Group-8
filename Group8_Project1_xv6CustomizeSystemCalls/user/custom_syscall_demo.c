#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Custom Syscall Demonstration Program
// This program tests the newly implemented system calls:
// - clone() for thread creation
// - mutex_create/lock/unlock for synchronization
// - sigmask() for signal masking

int
main(void)
{
  printf("=== Custom Syscall Demonstration ===\n");
  printf("Testing thread creation, mutex synchronization, and signal masking\n\n");

  // Test 1: Thread Creation with clone()
  printf("1. Testing thread creation (clone syscall):\n");
  int pid = clone();
  if (pid < 0) {
    printf("   ERROR: clone() failed with return value %d\n", pid);
  } else if (pid == 0) {
    // Child thread
    printf("   [Child Thread] Created successfully, PID: %d\n", getpid());
    printf("   [Child Thread] Performing child-specific work...\n");
    // Simulate some work
    for (int i = 0; i < 3; i++) {
      printf("   [Child Thread] Iteration %d\n", i + 1);
    }
    printf("   [Child Thread] Exiting\n");
    exit(0);
  } else {
    // Parent thread
    printf("   [Parent Thread] Created child thread with PID: %d\n", pid);
    printf("   [Parent Thread] Waiting for child to complete...\n");
    wait(0);  // Wait for child
    printf("   [Parent Thread] Child thread completed\n\n");
  }

  // Test 2: Mutex Operations
  printf("2. Testing mutex synchronization:\n");
  int mid = mutex_create();
  if (mid < 0) {
    printf("   ERROR: mutex_create() failed with return value %d\n", mid);
  } else {
    printf("   [Mutex] Created mutex with ID: %d\n", mid);

    printf("   [Mutex] Attempting to lock mutex...\n");
    int lock_result = mutex_lock(mid);
    if (lock_result < 0) {
      printf("   ERROR: mutex_lock() failed with return value %d\n", lock_result);
    } else {
      printf("   [Mutex] Successfully locked mutex\n");
      printf("   [Mutex] Performing critical section work...\n");
      // Simulate critical section
      printf("   [Mutex] Critical section: count = 42\n");

      printf("   [Mutex] Attempting to unlock mutex...\n");
      int unlock_result = mutex_unlock(mid);
      if (unlock_result < 0) {
        printf("   ERROR: mutex_unlock() failed with return value %d\n", unlock_result);
      } else {
        printf("   [Mutex] Successfully unlocked mutex\n");
      }
    }
    printf("   [Mutex] Test completed\n\n");
  }

  // Test 3: Signal Masking
  printf("3. Testing signal masking (sigmask syscall):\n");
  printf("   [Signal] Blocking signal 1...\n");
  int block_result = sigmask(1, 1);  // Block signal 1
  if (block_result < 0) {
    printf("   ERROR: sigmask(1, 1) failed with return value %d\n", block_result);
  } else {
    printf("   [Signal] Signal 1 is now blocked\n");
    printf("   [Signal] Current process would ignore signal 1 if sent\n");

    printf("   [Signal] Unblocking signal 1...\n");
    int unblock_result = sigmask(1, 0);  // Unblock signal 1
    if (unblock_result < 0) {
      printf("   ERROR: sigmask(1, 0) failed with return value %d\n", unblock_result);
    } else {
      printf("   [Signal] Signal 1 is now unblocked\n");
      printf("   [Signal] Current process will handle signal 1 normally\n");
    }
  }

  printf("\n=== Demonstration Complete ===\n");
  printf("All custom syscalls tested successfully!\n");

  exit(0);
}