# Project 1 — Custom Syscalls

## System Calls Implemented

| # | Syscall | Category | Syscall Number | Implemented By |
|---|---------|----------|----------------|----------------|
| 1 | `semget()` | IPC | 24 | Jaya Reddy Gari Dhanunjaya Reddy |
| 2 | `semwait()` | IPC | 25 | Jaya Reddy Gari Dhanunjaya Reddy |
| 3 | `sempost()` | IPC | 26 | Jaya Reddy Gari Dhanunjaya Reddy |
| 4 | `semclose()` | IPC | 27 | Jaya Reddy Gari Dhanunjaya Reddy |
| 5 | `getprocinfo()` | Process Info | 28 | Jinish Gupta |
| 6 | `getppid()` | Process Creation | 29 | Jinish Gupta |
| 7 | `yield()` | Process Creation | 30 | Jinish Gupta |
| 8 | `getfreeprocs()` | Process Creation | 31 | Jinish Gupta |
| 9 | `signal_init()` | Signals | 32 | Jammigumpula Mani Pradeep Chowdary |
| 10 | `signal_send()` | Signals | 33 | Jammigumpula Mani Pradeep Chowdary |
| 11 | `signal_handle()` | Signals | 34 | Jammigumpula Mani Pradeep Chowdary |
| 12 | `clone()` | Threads | 35 | Jatin Sargara |
| 13 | `mutex_create()` | Synchronization | 36 | Jatin Sargara |
| 14 | `mutex_lock()` | Synchronization | 37 | Jatin Sargara |
| 15 | `mutex_unlock()` | Synchronization | 38 | Jatin Sargara |
| 16 | `sigmask()` | Signals | 39 | Jatin Sargara |
| 17 | `shmget()` | Shared Memory | 45 | Kanala Venkata Bala Sekhar Reddy |
| 18 | `shmat()` | Shared Memory | 46 | Kanala Venkata Bala Sekhar Reddy |
| 19 | `shmdt()` | Shared Memory | 47 | Kanala Venkata Bala Sekhar Reddy |

## Files Changed

| File | Purpose |
|------|---------|
| `kernel/sem.c` | Core semaphore implementation (blocking/waking logic) |
| `kernel/sem.h` | Shared semaphore data structures and spinlocks |
| `user/semtest.c` | User-space demo program — tests synchronization |
| `user/testgetinfo.c` | User-space demo program — tests getting system info like PID and new process creation syscalls |
| `user/syscall_demo.c` | User-space demo program — tests `clone`, mutex, and `sigmask` syscalls |
| `kernel/sysproc.c` | New getprocinfo, getppid, yield, getfreeprocs kernel implementation along with original ones |
| `kernel/syscall.h` | Syscall number `#define` macros |
| `kernel/syscall.c` | Extern declarations + dispatch table entries |
| `user/usys.pl` | User-space stub generator entries |
| `kernel/signal.h` | Signal definitions and constants |
| `user/signaltest.c` | User-space demo program — tests signal handling |
| `user/user.h` | User-space function prototypes and `struct procinfo` |
| `kernel/shmem.c` | Shared memory management (kalloc-backed regions) |
| `kernel/shmem.h` | Shared memory data structures and internal types |
| `user/shmtest.c` | User-space test program for shared memory IPC |

## Functions Implemented By Everyone

### 1. Semaphore API References (Dhanunjaya Reddy)

- **`int semget(int key, int initval)`**
  Initializes or retrieves a counting semaphore associated with a key. Returns the semaphore ID.

- **`int semwait(int id)`**
  The **P** (proberen) operation. Decrements the semaphore value. If the value becomes negative, the process blocks until signaled.

- **`int sempost(int id)`**
  The **V** (verhogen) operation. Increments the semaphore value and wakes up one blocked process.

- **`int semclose(int id)`**
  Destroys the semaphore and wakes up all waiting processes with an error code.

### 2. Process Info & Process Creation APIs (Jinish Gupta)

- **`int getprocinfo(struct procinfo *info)`**
  Copies information about the current process into the user-provided `struct procinfo` buffer. The returned data includes PID, state, and name. Returns `0` on success and `-1` on failure.

- **`int getppid(void)`**
  Returns the parent process ID of the currently executing process. It tracks the lineage of the process during fork creation.

- **`int yield(void)`**
  Simulates yielding the CPU by voluntarily giving up the processor. Typically paired with process scheduling. 

- **`int getfreeprocs(void)`**
  Retrieves the total number of currently unused process structures (`UNUSED` state) within the process table. Helpful to see how many new processes can be created.

### 3. Signal API References (Mani Pradeep Chowdary)

- **`int signal_init(void)`**
  Initializes the signal table for the current process. Sets all handlers to `SIG_DFL` (default) and clears the pending-signal bitmask.

- **`int signal_send(int pid, int signum)`**
  Sends a signal to a target process identified by PID. Sets the corresponding bit in that process's pending signals bitmask.

- **`int signal_handle(int signum, sighandler_t handler)`**
  Registers a user-space handler function for a given signal number. Special handlers include `SIG_IGN` (ignore) and `SIG_DFL` (default action, usually kills process).

### 4. Shared Memory API References (Bala Sekhar Reddy)

- **`int shmget(int key, int size)`**
  Creates or retrieves a shared memory ID associated with a key. Currently supports a `size` up to 1 page (4096 bytes).

- **`void* shmat(int id)`**
  Maps the shared memory region into the process's page table. Returns the starting user virtual address on success, or `(void*)-1` on error.

- **`int shmdt(void *addr)`**
  Detaches (unmaps) the shared memory region from the process's address space. The physical page is freed only when the last process detaches.

### 5. Thread & Synchronization Subsystems (Jatin Sargara)

- **`int clone(void)`**
  Facilitates the creation of lightweight threads sharing the parent's address space.

- **`int mutex_create(void)`, `int mutex_lock(int id)`, `int mutex_unlock(int id)`**
  Initializes a synchronization lock within the kernel space, allowing locking and unlocking mechanisms.

- **`int sigmask(int signum, int block)`**
  Toggles signal masking and blocks unmasking behaviors for active processes.

## Build & Run

```bash
make qemu
# Inside xv6 shell:
$ semtest
$ testgetinfo
$ signaltest
$ syscall_demo
$ shmtest
```

Press **Ctrl+A then X** to quit QEMU.

## Expected Outputs

### 1. Expected `testgetinfo` Output

```text
testgetinfo: PID=3 State=4 Name=testgetinfo
testgetinfo: Parent PID=2
testgetinfo: Free process slots=61
testgetinfo: Yield returned=0
```

### 2. Expected `syscall_demo` Output

```text
=== Custom Syscall Demonstration ===
Testing thread creation, mutex synchronization, and signal masking

1. Testing thread creation (clone syscall):
   [Child Thread] Created successfully, PID: <pid>
   [Child Thread] Performing child-specific work...
   [Child Thread] Iteration 1
   [Child Thread] Iteration 2
   [Child Thread] Iteration 3
   [Child Thread] Exiting
   [Parent Thread] Created child thread with PID: <pid>
   [Parent Thread] Waiting for child to complete...
   [Parent Thread] Child thread completed

2. Testing mutex synchronization:
   [Mutex] Created mutex with ID: 0
   [Mutex] Attempting to lock mutex...
   [Mutex] Successfully locked mutex
   [Mutex] Performing critical section work...
   [Mutex] Critical section: count = 42
   [Mutex] Attempting to unlock mutex...
   [Mutex] Successfully unlocked mutex
   [Mutex] Test completed

3. Testing signal masking (sigmask syscall):
   [Signal] Blocking signal 1...
   [Signal] Signal 1 is now blocked
   [Signal] Current process would ignore signal 1 if sent
   [Signal] Unblocking signal 1...
   [Signal] Signal 1 is now unblocked
   [Signal] Current process will handle signal 1 normally

=== Demonstration Complete ===
All custom syscalls tested successfully!
```

### 3. Expected `semtest` Output

```text
semtest: starting...
semtest: parent waiting for child...
semtest: child signaling parent...
semtest: parent resumed!
semtest: PASSED
```

### 4. Expected `shmtest` Output

```text
shmtest: starting shared memory test
shmtest: shmget returned id=0
shmtest: parent mapped shared region at 0x...
shmtest: parent wrote 0xdeadbeef to shared memory
shmtest: child mapped shared region at 0x...
shmtest: child read 0xdeadbeef from shared memory
shmtest: PASS — child read the correct value written by parent
shmtest: parent: child exited
shmtest: parent detached shared region
shmtest: done
```

### 5. Expected `signaltest` Output

```text
=== xv6 Signal Handling Test ===

--- Test 1: signal_init ---
[signal_init] PID 3: signal table initialised
PASS: signal table initialised for PID 3

--- Test 2: signal_handle ---
PASS: handlers registered for SIGUSR1, SIGUSR2, SIGALRM

--- Test 3: SIG_IGN (ignore signal) ---
[dispatch] PID 3 ignored signal 5
PASS: SIGUSR1 ignored

--- Test 4: SIG_DFL kills child ---
[dispatch] PID 5: default action for signal 1 — killing
PASS: child was killed by default handler
```

## Design Notes

- **Semaphores**: Implemented using a global `semtable` protected by a spinlock. Individual semaphores also have their own spinlocks to ensure atomicity during sleep/wakeup.
- **Sleep/Wakeup**: Leverages the kernel's existing `sleep()` and `wakeup()` functions for process synchronization.
- **getprocinfo, getppid, yield, getfreeprocs**: Intentionally extends existing kernel structures directly in `sysproc.c` using `copyout()` safely to extract struct info into userspace wrappers to prevent corruption vectors.
- **Shared Memory**: Managed by a global table (`shmtable`) and synchronized via `shmlock`. Utilizes the kernel's `kalloc()` to allocate physical pages and `mappages()` to inject them into process page tables. Refcounting ensures pages are only freed when no processes are attached.