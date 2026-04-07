# Project 1 — Custom xv6 System Calls

## System Calls Implemented

| # | Syscall | Category | Syscall Number |
|---|---------|----------|----------------|
| 1 | `getprocinfo()` | Process Info | 22 |

## Files in This Folder

| File | Purpose |
|------|---------|
| `sysproc_additions.c` | All new syscall kernel implementations |
| `syscall_additions.h` | Syscall number `#define` macros |
| `syscall_table_additions.c` | Extern declarations + dispatch table entries |
| `usys_additions.pl` | User-space stub generator entries |
| `user_additions.h` | User-space function prototypes and `struct procinfo` |
| `testcalls.c` | User demo program — tests all syscalls |

## How to Integrate into xv6

### 1. Clone xv6
```bash
git clone https://github.com/mit-pdos/xv6-riscv.git
cd xv6-riscv
```

### 2. kernel/syscall.h
Append the contents of `syscall_additions.h` (the 1 `#define` line) before the `#endif`.

### 3. kernel/sysproc.c
Append the entire contents of `sysproc_additions.c` at the bottom of the file.

### 4. kernel/syscall.c
- Add the `extern uint64 sys_*` declarations from `syscall_table_additions.c` with the existing externs.
- Add the `[SYS_*]` entries inside the `syscalls[]` array.

### 5. user/usys.pl
Append the `entry(...)` line from `usys_additions.pl`.

### 6. user/user.h
Append `struct procinfo` and the function prototype from `user_additions.h`.

### 7. user/testcalls.c
Copy `testcalls.c` here, then in the **Makefile** add to `UPROGS`:
```makefile
$U/_testcalls \
```

## Build & Run

```bash
make qemu
# Inside xv6 shell:
$ testcalls
```

Press **Ctrl+A then X** to quit QEMU.

## Expected Output

```
=== xv6 Custom System Call Demo ===

-- Test 1: getprocinfo --
  PID=3  PPID=2  State=3  Name=testcalls

=== All tests completed ===
```

## Design Notes

- **getprocinfo**: Uses `copyout()` to safely transfer kernel struct data to user space.
