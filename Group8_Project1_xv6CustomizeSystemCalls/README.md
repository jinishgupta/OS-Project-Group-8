# Project 1 — Counting Semaphores & Custom Syscalls

## System Calls Implemented

| # | Syscall | Category | Syscall Number |
|---|---------|----------|----------------|
| 1 | `semget()` | IPC | 24 |
| 2 | `semwait()` | IPC | 25 |
| 3 | `sempost()` | IPC | 26 |
| 4 | `semclose()` | IPC | 27 |
| 5 | `getprocinfo()` | Process Info | 28 |
| 6 | `signal_init()` | Signals | 29 |
| 7 | `signal_send()` | Signals | 30 |
| 8 | `signal_handle()` | Signals | 31 |


## Files in This Folder

| File | Purpose |
|------|---------|
| `kernel/sem.c` | Core semaphore implementation (blocking/waking logic) |
| `kernel/sem.h` | Shared semaphore data structures and spinlocks |
| `user/semtest.c` | User-space demo program — tests synchronization |
| `user/testgetinfo.c` | User-space demo program — tests getting system info like PID |
| `sysproc_additions.c` | New getprocinfo syscall kernel implementation along with original ones |
| `syscall_additions.h` | Syscall number `#define` macros |
| `syscall_table_additions.c` | Extern declarations + dispatch table entries |
| `usys_additions.pl` | User-space stub generator entries |
| `kernel/signal.h` | Signal definitions and constants |
| `user/signaltest.c` | User-space demo program — tests signal handling |
| `user_additions.h` | User-space function prototypes and `struct procinfo` |

## Semaphore API Reference

### 1. `int semget(int key, int initval)`
Initializes or retrieves a counting semaphore associated with a key. Returns the semaphore ID.

### 2. `int semwait(int id)`
The **P** (proberen) operation. Decrements the semaphore value. If the value becomes negative, the process blocks until signaled.

### 3. `int sempost(int id)`
The **V** (verhogen) operation. Increments the semaphore value and wakes up one blocked process.

### 4. `int semclose(int id)`
Destroys the semaphore and wakes up all waiting processes with an error code.

## Signal API Reference

### 1. `int signal_init(void)`
Initializes the signal table for the current process. Sets all handlers to `SIG_DFL` (default) and clears the pending-signal bitmask.

### 2. `int signal_send(int pid, int signum)`
Sends a signal to a target process identified by PID. Sets the corresponding bit in that process's pending signals bitmask.

### 3. `int signal_handle(int signum, sighandler_t handler)`
Registers a user-space handler function for a given signal number. Special handlers include `SIG_IGN` (ignore) and `SIG_DFL` (default action, usually kills process).

## About getprocinfo system call

### 1. `int getprocinfo(struct procinfo *info)`
Copies information about the current process into the user-provided `struct procinfo` buffer.
The returned data includes:

- `pid`: current process ID
- `ppid`: parent process ID
- `state`: current process state
- `name`: process name

Returns `0` on success and `-1` on failure.

## Build & Run

```bash
make qemu
# Inside xv6 shell:
$ semtest
$ testgetinfo
$ signaltest
```

Press **Ctrl+A then X** to quit QEMU.

## Expected `semtest` Output

```text
semtest: starting...
semtest: parent waiting for child...
semtest: child signaling parent...
semtest: parent resumed!
semtest: PASSED
```

### Expected `testgetinfo` Output

```text
testgetinfo: PID=3  PPID=2  State=4  Name=testgetinfo
```

### Expected `signaltest` Output

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
- **getprocinfo**: Uses `copyout()` to safely transfer kernel struct data to user space.