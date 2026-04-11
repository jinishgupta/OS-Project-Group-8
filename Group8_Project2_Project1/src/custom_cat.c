#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cat_file(FILE *fp, int number_lines) {
    char buf[4096];
    int line_num = 1, at_line_start = 1;
    while (fgets(buf, sizeof(buf), fp)) {
        if (number_lines && at_line_start)
            printf("%6d\t", line_num++);
        printf("%s", buf);
        at_line_start = (buf[strlen(buf) - 1] == '\n');
    }
}

int cmd_cat(int argc, char *argv[]) {
    int number_lines = 0, file_start = 1;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            for (int j = 1; argv[i][j]; j++) {
                if (argv[i][j] == 'n') number_lines = 1;
                else if (argv[i][j] == 'h') {
                    printf("Usage: custom_cat [-n] [file...]\n");
                    return EXIT_SUCCESS;
                }
            }
            file_start = i + 1;
        } else { file_start = i; break; }
    }
    if (file_start >= argc) { cat_file(stdin, number_lines); return EXIT_SUCCESS; }
    for (int i = file_start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) { fprintf(stderr, "custom_cat: %s: No such file\n", argv[i]); continue; }
        cat_file(fp, number_lines);
        fclose(fp);
    }
    return EXIT_SUCCESS;
}

#ifndef SHELL_BUILD
int main(int argc, char *argv[]) {
    return cmd_cat(argc, argv);
}
#endif
