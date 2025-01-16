#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fa, *fb;
    int ca;

    // Open the input file in read mode
    fa = fopen("input.txt", "r");
    if (fa == NULL) {
        printf("Cannot open input file\n");
        exit(0);
    }

    // Open the output file in write mode
    fb = fopen("output.txt", "w");
    if (fb == NULL) {
        printf("Cannot open output file\n");
        exit(0);
    }

    // Read the first character from the input file
    ca = fgetc(fa);

    // Flag to track if the previous character was a space or tab
    int lastWasSpace = 0;

    // Process the file until EOF
    while (ca != EOF) {
        // If the character is a space or tab, replace it with a single space
        if (ca == ' ' || ca == '\t') {
            // Only write a space if the last character was not a space
            if (!lastWasSpace) {
                fputc(' ', fb);
                lastWasSpace = 1;
            }
        } else {
            // Otherwise, write the character to the output file
            fputc(ca, fb);
            lastWasSpace = 0;
        }

        // Read the next character from the input file
        ca = fgetc(fa);
    }

    // Close the files
    fclose(fa);
    fclose(fb);

    printf("Processing complete. Output written to 'output.txt'.\n");

    return 0;
}

