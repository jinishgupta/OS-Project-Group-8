# Project 2 — Custom UNIX-like Shell & Utilities

## Overview

A complete interactive shell (`myshell`) built from scratch in C, with a suite of custom utilities.
Every utility is implemented **from scratch** using only POSIX system calls — no coreutils, no shell scripts.

## Team Contributions

This project was collaboratively developed by our team. Below is the breakdown of who implemented which command/utility:

| Component/Utility | Implemented By |
|-------------------|----------------|
| `myshell` (Interactive Shell) | Jinish Gupta |
| `custom_ls` | Jinish Gupta |
| `custom_grep` | Jaya Reddy Gari Dhanunjaya Reddy |
| `custom_cat` | Jammigumpula Mani Pradeep Chowdary |
| `custom_cp` | Jatin Sargara |
| `custom_wc` | Jay Solanki |
| `custom_rm` | Kanala Venkata Bala Sekhar Reddy |


## Files

```
src/
  shell.c          Main interactive shell
  custom_ls.c      List directory contents
  custom_grep.c    Search for patterns in files
  custom_cat.c     Concatenate and print files
  custom_cp.c      Copy files with overwrite prompt
  custom_wc.c      Count characters, words, and lines
  custom_rm.c      Remove files and directories
Makefile           Build system
README.md          This file
```

## Build

```bash
# Build everything (shell + standalone utilities)
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
./bin/custom_grep -in "pattern" file.txt
./bin/custom_cat -n file.txt
./bin/custom_cp source.txt dest.txt
./bin/custom_wc file.txt
./bin/custom_rm file.txt
```

## Command Reference

### custom_ls — List directory contents

```
custom_ls [OPTIONS] [DIR]
  -l    Long format (permissions, owner, size, date)
  -a    Show hidden files (starting with .)
  -h    Human-readable sizes (K, M, G)
```

### custom_cat — Concatenate and print files

```
custom_cat [OPTIONS] [FILE...]
  -n    Number all output lines
  -h    Display help message
```

**Arguments:**

| Argument | Description |
|----------|-------------|
| `FILE...` | One or more files to concatenate. If omitted, reads from stdin |

### custom_cp — Copy files with overwrite prompt

```
custom_cp <source> <destination>
```

**Behavior:**

- Copies the source file to the destination path using a buffered read/write loop.
- If the destination file already exists, prompts the user to confirm overwrite (`y`/`n`).
- Prints a success message on completion or an error message if the source cannot be opened or the destination cannot be created.

### custom_wc — Word, line, and character count
```
custom_wc <filename>
```
**Behavior:**
- Analyzes the specified file to calculate the total number of lines, words, and characters.
- Utilizes state-tracking logic to identify word boundaries by detecting transitions from whitespace to non-whitespace characters.
- Outputs the final results in a single line formatted as: lines words characters filename.
- Includes error handling to notify the user if the filename argument is missing or if the target file cannot be opened.


### custom_grep — Search for a pattern in files

```
custom_grep [OPTIONS] PATTERN [FILE...]
  -i    Ignore case (case-insensitive matching)
  -v    Invert match (show lines NOT matching the pattern)
  -n    Show line numbers alongside matching lines
  -c    Only show a count of matching lines, not the lines themselves
```
**Arguments:**

| Argument | Description |
|----------|-------------|
| `PATTERN` | The string to search for |
| `FILE...` | One or more files to search. If omitted, reads from stdin |

**Behavior:**

- When multiple files are provided, each matching line is prefixed with the filename (e.g. `file.txt:line`).
- Flags can be combined freely (e.g. `-ivn` applies ignore-case, invert, and line numbers together).
- The `-c` flag suppresses normal output and prints only the match count per file. When combined with multiple files, each file's count is prefixed with its filename.
- When reading from stdin (no files given), the source is labeled `(stdin)` in multi-file prefix output.
- Case-insensitive search (`-i`) uses `strcasestr` internally.

**Exit codes:**

| Code | Meaning |
|------|---------|
| `0`  | At least one match was found |
| `1`  | No matches found, or an error occurred (e.g. unknown option, file not found) |

### custom_rm — Remove files and directories

```
custom_rm [OPTIONS] <path>...
  -r    Recursively remove a directory and all its contents
  -f    Force: silently ignore non-existent files (no error)
  -rf   Recursive + force combined
```

**Arguments:**

| Argument | Description |
|----------|-------------|
| `path...` | One or more files or directories to remove |

**Behavior:**

- Without flags, removes a single file. Prints an error if the path does not exist or is a directory.
- With `-r`, recursively removes a directory tree — traverses all subdirectories and deletes all files and folders bottom-up using `opendir`/`readdir`/`unlink`/`rmdir`.
- With `-f`, suppresses errors for paths that do not exist (exit code is still `0`). Non-existence is silently ignored.
- With `-rf`, combines recursive removal with the force flag — useful for removing directories without confirmation errors.
- Multiple paths can be provided; each is processed in order.

**Exit codes:**

| Code | Meaning |
|------|---------|
| `0`  | All specified paths were successfully removed (or silently skipped with `-f`) |
| `1`  | One or more paths could not be removed (permission denied, path in use, etc.) |

## Shell Features

The interactive shell (`myshell`) supports running all custom utilities as built-in commands, as well as any external program available on the system.

| Feature | Example |
|---------|---------|
| Run built-in commands | `custom_ls -la` or `custom_grep pattern` |
| Remove files/dirs | `custom_rm -rf build/` |
| Change directory | `cd /tmp` |
| Output redirection | `custom_ls > out.txt` |
| Input redirection | `custom_grep pattern < in.txt` |
| Pipe | `custom_ls | custom_grep pattern` |
| External commands | `echo hello` |
| Help | `help` |
| Exit | `exit` or `quit` |

**Built-in commands available in the shell:**

| Command | Description |
|---------|-------------|
| `custom_ls` | List directory contents |
| `custom_grep` | Search for a pattern in files |
| `custom_cat` | Display file contents |
| `custom_cp` | Copy files |
| `custom_rm` | Remove files or directories |
| `cd` | Change current directory |
| `help` | Show available commands |
| `exit` / `quit` | Exit the shell |


## Notes

- Hidden files and subdirectories are included in recursive removal.
- Symlinks are removed as files (the link itself is deleted, not the target).
- Input redirection is supported by the shell, but `custom_ls` does not normally consume stdin.
- If multiple directory arguments are passed to `custom_ls`, the last non-option argument is used.
- `custom_grep` fully supports both input redirection and pipe input from the shell.
- `custom_rm -rf` inside the shell runs in a forked child process, keeping the shell itself safe.
- `custom_rm` will refuse to remove a directory without the `-r` flag and prints a descriptive error.