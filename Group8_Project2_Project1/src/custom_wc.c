#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    // Check if the user provided a filename
    if (argc < 2) {
        printf("Usage: custom_wc <filename>\n");
        return 1;
    }

    // Open the file in read mode
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int chars = 0;
    int words = 0;
    int lines = 0;
    int in_word = 0; // State variable to track if we are currently reading a word
    int ch;

    // Read the file character by character until the End of File (EOF)
    while ((ch = fgetc(file)) != EOF) {
        chars++; // Increment character count for every character read

        if (ch == '\n') {
            lines++; // Increment line count when a newline is encountered
        }

        // Check for whitespace (spaces, tabs, newlines)
        if (isspace(ch)) {
            in_word = 0; // We are outside a word
        } else if (!in_word) {
            in_word = 1; // We just transitioned into a new word
            words++;
        }
    }

    fclose(file);

    // Print the results (Standard UNIX format: lines, words, chars, filename)
    printf("%d %d %d %s\n", lines, words, chars, argv[1]);

    return 0;
}
