#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fa, *fb;
    int ca;

    // Open the input file in read mode
    fa = fopen("input.c", "r");
    if (fa == NULL) {
        printf("Cannot open input file\n");
        exit(0);
    }

    // Open the output file in write mode
    fb = fopen("output.c", "w");
    if (fb == NULL) {
        printf("Cannot open output file\n");
        exit(0);
    }

    // Read the first character from the input file
    ca = fgetc(fa);

    // Process the file character by character
    while (ca != EOF) {
        // If the character is '#', it's a preprocessor directive
        if (ca == '#') {
            // Skip the entire preprocessor directive
            while (ca != '\n' && ca != EOF) {
                ca = fgetc(fa);
            }
        } else {
            // Write the character to the output file if it's not part of a directive
            fputc(ca, fb);
        }

        // Read the next character
        ca = fgetc(fa);
    }

    // Close the files
    fclose(fa);
    fclose(fb);

    printf("Preprocessor directives discarded. Output written to 'output.c'.\n");

    return 0;
}

