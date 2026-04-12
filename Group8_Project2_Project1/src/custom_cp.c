#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *src, *dest;
    char buffer[1024];
    size_t bytes;
    char choice;

    // Step 1: Check arguments
    if (argc != 3) {
        printf("Usage: custom_cp <source> <destination>\n");
        return 1;
    }

    // Step 2: Open source file
    src = fopen(argv[1], "rb");
    if (src == NULL) {
        printf("Error: Cannot open source file.\n");
        return 1;
    }

    // Step 3: Check if destination already exists
    dest = fopen(argv[2], "rb");
    if (dest != NULL) {
        fclose(dest);
        printf("File already exists. Overwrite? (y/n): ");
        scanf(" %c", &choice);

        if (choice != 'y' && choice != 'Y') {
            printf("Copy cancelled.\n");
            fclose(src);
            return 0;
        }
    }

    // Step 4: Open destination file for writing
    dest = fopen(argv[2], "wb");
    if (dest == NULL) {
        printf("Error: Cannot create destination file.\n");
        fclose(src);
        return 1;
    }

    // Step 5: Copy file using buffer (fast method)
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    printf("File copied successfully.\n");

    // Step 6: Close files
    fclose(src);
    fclose(dest);

    return 0;
}