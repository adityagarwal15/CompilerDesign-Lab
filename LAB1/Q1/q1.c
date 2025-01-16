#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char filename[100];
    char c;
    int lines = 0, characters = 0;

    // Prompt user to enter the file name
    printf("Enter the filename: ");
    scanf("%s", filename);

    // Open the file in read mode
    file = fopen(filename, "r");

    // Check if the file exists
    if (file == NULL) {
        printf("Cannot open file %s \n", filename);
        exit(0);
    }

    // Read the file character by character
    while ((c = fgetc(file)) != EOF) {
        characters++; // Increment character count

        // Check if the character is a newline
        if (c == '\n') {
            lines++;
        }
    }

    // Close the file
    fclose(file);

    // Print the results
    printf("Number of lines: %d\n", lines);
    printf("Number of characters: %d\n", characters);

    return 0;
}

