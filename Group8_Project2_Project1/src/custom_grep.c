#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// custom_grep: Search for a pattern in files or stdin
// Handles flags: -i (ignore case), -v (invert match), -n (line numbers), -c (count)

int cmd_grep(int argc, char *argv[])
{
    int flag_i = 0, flag_v = 0, flag_n = 0, flag_c = 0;
    int arg_idx = 1;

    // Parse options
    while (arg_idx < argc && argv[arg_idx][0] == '-') {
        for (int j = 1; argv[arg_idx][j]; j++) {
            if (argv[arg_idx][j] == 'i') flag_i = 1;
            else if (argv[arg_idx][j] == 'v') flag_v = 1;
            else if (argv[arg_idx][j] == 'n') flag_n = 1;
            else if (argv[arg_idx][j] == 'c') flag_c = 1;
            else {
                fprintf(stderr, "custom_grep: unknown option -%c\n", argv[arg_idx][j]);
                return 1;
            }
        }
        arg_idx++;
    }

    if (arg_idx >= argc) {
        fprintf(stderr, "Usage: custom_grep [-ivnc] pattern [file...]\n");
        return 1;
    }

    char *pattern = argv[arg_idx++];
    int file_count = argc - arg_idx;
    int total_match_count = 0;

    // Process files one by one (or stdin if no files)
    do {
        FILE *fp = stdin;
        char *filename = "(stdin)";
        if (file_count > 0) {
            filename = argv[arg_idx];
            fp = fopen(filename, "r");
            if (!fp) {
                perror(filename);
                arg_idx++;
                continue;
            }
        }

        char *line = NULL;
        size_t len = 0;
        int line_num = 0;
        int file_match_count = 0;

        while (getline(&line, &len, fp) != -1) {
            line_num++;
            char *found = flag_i ? strcasestr(line, pattern) : strstr(line, pattern);
            int is_match = (found != NULL);
            if (flag_v) is_match = !is_match;

            if (is_match) {
                file_match_count++;
                if (!flag_c) {
                    if (file_count > 1) printf("%s:", filename);
                    if (flag_n) printf("%d:", line_num);
                    printf("%s", line);
                }
            }
        }

        if (flag_c) {
            if (file_count > 1) printf("%s:", filename);
            printf("%d\n", file_match_count);
        }

        total_match_count += file_match_count;
        free(line);
        if (fp != stdin) fclose(fp);
        arg_idx++;
    } while (arg_idx < argc);

    return (total_match_count > 0) ? 0 : 1;
}

// Standalone entry point
#ifndef SHELL_BUILD
int main(int argc, char *argv[])
{
    return cmd_grep(argc, argv);
}
#endif
