# Project 2 — Custom UNIX-like Shell & Utilities

## Overview

A complete interactive shell (`myshell`) built from scratch in C, with a unified custom utility for listing contents. 
Every utility is implemented **from scratch** using only POSIX system calls — no coreutils, no shell scripts.

## Files

```
src/
  shell.c          Main interactive shell
  custom_ls.c      List directory contents
Makefile           Build system
README.md          This file
```

## Build

```bash
# Build everything (shell + standalone utility)
make

# Build only the shell
make shell

# Build only standalone utilities
make standalones

# Clean
make clean
```

Binaries are placed in `bin/`.

## Run

### Interactive shell
```bash
./bin/myshell
```

### Standalone utilities
```bash
./bin/custom_ls -la 
```

## Quick Output Check For ls Command

```text
drwxrwxr-x   2 user     user        4096 Apr 11 15:07 .
drwxrwxr-x   4 user     user        4096 Apr 11 16:09 ..
-rw-rw-r--   1 user     user        4294 Apr 11 15:07 custom_ls.c
-rw-rw-r--   1 user     user        6077 Apr 11 15:07 shell.c
```

## Command Reference

### custom_ls — List directory contents
```
custom_ls [OPTIONS] [DIR]
  -l    Long format (permissions, owner, size, date)
  -a    Show hidden files (starting with .)
  -h    Human-readable sizes (K, M, G)
```

Exit codes:

- `0` on success
- non-zero on error (for example, invalid option or directory open failure)

## Shell Features

| Feature | Example |
|---------|---------|
| Run built-in commands | `custom_ls -la` |
| Change directory | `cd /tmp` |
| Output redirection | `custom_ls > out.txt` |
| Input redirection | `custom_ls < in.txt` |
| Pipe | `custom_ls | cat` |
| External commands | `echo hello` |
| Help | `help` |
| Exit | `exit` or `quit` |

Notes:

- Input redirection is supported by the shell, but `custom_ls` does not normally consume stdin.
- If multiple directory arguments are passed to `custom_ls`, the last non-option argument is used.