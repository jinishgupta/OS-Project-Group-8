# Project 2 — Custom UNIX-like Shell & Utilities

## Overview

A complete interactive shell (`myshell`) built from scratch in C, with a unified custom utility for listing contents. 
Every utility is implemented **from scratch** using only POSIX system calls — no coreutils, no shell scripts.

## Files

```
src/
  shell.c          Main interactive shell
  custom_ls.c      List directory contents
  custom_grep.c    Search for patterns in files
  custom_cat.c     Concatenate and print files
  custom_cp.c      Copy files with overwrite prompt\
  custom_wc.c      counts the number of characters words and lines.
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
./bin/custom_grep -in "pattern" file.txt
./bin/custom_cat -n file.txt
./bin/custom_cp source.txt dest.txt
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

---

### custom_cp — Copy files with overwrite prompt

```
custom_cp <source> <destination>
```

**Behavior:**

- Copies the source file to the destination path using a buffered read/write loop.
- If the destination file already exists, prompts the user to confirm overwrite (`y`/`n`).
- Prints a success message on completion or an error message if the source cannot be opened or the destination cannot be created.

---

### custom_wc — Word, line, and character count
```
custom_wc <filename>
```
**Behavior**
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

**Examples:**

```bash
# Basic search
custom_grep "hello" file.txt

# Case-insensitive search with line numbers
custom_grep -in "error" app.log

# Show lines that do NOT contain "debug"
custom_grep -v "debug" app.log

# Count matches across multiple files
custom_grep -c "TODO" src/*.c

# Search stdin via pipe
cat file.txt | custom_grep "pattern"

# Combine with input redirection in the shell
custom_grep "pattern" < input.txt
```

---

## Shell Features

| Feature | Example |
|---------|---------|
| Run built-in commands | `custom_ls -la` or `custom_grep pattern` |
| Change directory | `cd /tmp` |
| Output redirection | `custom_ls > out.txt` |
| Input redirection | `custom_grep pattern < in.txt` |
| Pipe | `custom_ls | custom_grep pattern` |
| External commands | `echo hello` |
| Help | `help` |
| Exit | `exit` or `quit` |

**Notes:**

- Input redirection is supported by the shell, but `custom_ls` does not normally consume stdin.
- If multiple directory arguments are passed to `custom_ls`, the last non-option argument is used.
- `custom_grep` fully supports both input redirection and pipe input from the shell.
