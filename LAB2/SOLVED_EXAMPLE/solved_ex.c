#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fa, *fb;
    int ca, cb;
    
    // Open the input file in read mode
    fa = fopen("solved_ex.c", "r");
    if (fa == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    // Open the output file in write mode
    fb = fopen("solved_output.c", "w");

    // Read the first character from the input file
    ca = getc(fa);

    // Process the file until EOF
    while (ca != EOF) {
        // If the character is a space, write it to the output file
        if (ca == ' ') {
            putc(ca, fb);
            // Skip additional spaces
            while (ca == ' ') {
                ca = getc(fa);
            }
        }
        
        // If the character is '/', check for comment types
        if (ca == '/') {
            cb = getc(fa);
            // Handle single-line comment
            if (cb == '/') {
                while (ca != '\n') {
                    ca = getc(fa);
                }
            }
            // Handle multi-line comment
            else if (cb == '*') {
                do {
                    // Skip characters until '*/' is found
                    while (ca != '*') {
                        ca = getc(fa);
                    }
                    ca = getc(fa);
                } while (ca != '/');
            } else {
                // If not a comment, write both '/' and next character
                putc(ca, fb);
                putc(cb, fb);
            }
        } else {
            // If the character is not part of a comment, write it to the output file
            putc(ca, fb);
        }
        // Read the next character from the input file
        ca = getc(fa);
    }

    // Close the files
    fclose(fa);
    fclose(fb);

    return 0;
}

