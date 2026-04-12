
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_INPUT  1024
#define MAX_ARGS   64
#define MAX_PATH   256

// Forward declarations for built-in commands
int cmd_ls(int argc, char *argv[]);
int cmd_grep(int argc, char *argv[]);
int cmd_cat(int argc, char *argv[]);   
int cmd_cp(int argc, char *argv[]);

// Built-in command table
typedef struct {
    const char *name;
    int (*func)(int, char **);
    const char *description;
} Command;

static Command commands[] = {
    { "custom_ls",   cmd_ls,        "List directory contents" },
    { "custom_grep", cmd_grep,      "Search for a pattern in files" },
    { "custom_cat",  cmd_cat,       "Display file contents" }, 
    { "custom_cp",   cmd_cp,        "Copy files" },
    { NULL, NULL, NULL }
};

// Print shell prompt
static void print_prompt(void)
{
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)))
        printf("\033[1;32mmyshell\033[0m:\033[1;34m%s\033[0m$ ", cwd);
    else
        printf("myshell$ ");
    fflush(stdout);
}

// Tokenise input line into argv[]
static int tokenise(char *line, char *argv[])
{
    int argc = 0;
    char *tok = strtok(line, " \t\n");
    while (tok && argc < MAX_ARGS - 1) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;
    return argc;
}

// Handle I/O redirection (<, >)
static int handle_redirection(char *argv[], int *argc_out)
{
    int argc = 0;
    int new_argc = 0;
    while (argv[argc]) argc++;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], ">") == 0 && i + 1 < argc) {
            int fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("open"); return -1; }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            i++;
        } else if (strcmp(argv[i], "<") == 0 && i + 1 < argc) {
            int fd = open(argv[i + 1], O_RDONLY);
            if (fd < 0) { perror("open"); return -1; }
            dup2(fd, STDIN_FILENO);
            close(fd);
            i++;
        } else {
            argv[new_argc++] = argv[i];
        }
    }
    argv[new_argc] = NULL;
    *argc_out = new_argc;
    return 0;
}

// Run a pipeline: cmd1 | cmd2
static void run_pipeline(char *left_cmd, char *right_cmd)
{
    int pfd[2];
    if (pipe(pfd) < 0) { perror("pipe"); return; }

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]); close(pfd[1]);

        char *argv[MAX_ARGS];
        int argc = tokenise(left_cmd, argv);
        handle_redirection(argv, &argc);

        for (int i = 0; commands[i].name; i++) {
            if (strcmp(argv[0], commands[i].name) == 0) {
                exit(commands[i].func(argc, argv));
            }
        }

        execvp(argv[0], argv);
        exit(1);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]); close(pfd[1]);

        char *argv[MAX_ARGS];
        int argc = tokenise(right_cmd, argv);
        handle_redirection(argv, &argc);

        for (int i = 0; commands[i].name; i++) {
            if (strcmp(argv[0], commands[i].name) == 0) {
                exit(commands[i].func(argc, argv));
            }
        }

        execvp(argv[0], argv);
        exit(1);
    }

    close(pfd[0]); close(pfd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

// Built-in: help
static void print_help(void)
{
    printf("\n  Available built-in commands:\n");
    for (int i = 0; commands[i].name; i++)
        printf("    %-16s  %s\n", commands[i].name, commands[i].description);

    printf("    %-16s  %s\n", "cd <dir>",    "Change directory");
    printf("    %-16s  %s\n", "help",        "Show this help");
    printf("    %-16s  %s\n", "exit/quit",   "Exit the shell");
    printf("\n  Supports: I/O redirection (< >) and pipes (|)\n\n");
}

// Main shell loop
int main(void)
{
    char input[MAX_INPUT];
    char *argv[MAX_ARGS];

    printf("\n  Welcome to MyShell — Custom UNIX-like Shell\n");
    printf("  Type 'help' for available commands.\n\n");

    while (1) {
        print_prompt();

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        char *pipe_pos = strchr(input, '|');
        if (pipe_pos) {
            *pipe_pos = '\0';
            run_pipeline(input, pipe_pos + 1);
            continue;
        }

        int argc = tokenise(input, argv);
        if (argc == 0) continue;

        if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (strcmp(argv[0], "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(argv[0], "cd") == 0) {
            const char *dir = (argc > 1) ? argv[1] : getenv("HOME");
            if (chdir(dir) < 0) perror("cd");
            continue;
        }

        int found = 0;
        for (int i = 0; commands[i].name; i++) {
            if (strcmp(argv[0], commands[i].name) == 0) {
                found = 1;

                pid_t pid = fork();
                if (pid == 0) {
                    int new_argc = argc;
                    handle_redirection(argv, &new_argc);
                    exit(commands[i].func(new_argc, argv));
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                }
                break;
            }
        }

        if (!found) {
            pid_t pid = fork();
            if (pid == 0) {
                execvp(argv[0], argv);
                fprintf(stderr, "myshell: command not found: %s\n", argv[0]);
                exit(127);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    return 0;
}