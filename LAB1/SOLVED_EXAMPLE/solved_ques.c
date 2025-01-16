#include <stdio.h>
#include <stdlib.h> // For exit()

int main() {
    FILE *fptr1, *fptr2; // File pointers for source and destination files
    char filename[100], c;

    // Step 1: Enter the source filename
    printf("Enter the filename to open for reading: \n");
    scanf("%s", filename);

    // Step 2: Open the source file for reading
    fptr1 = fopen(filename, "r");
    if (fptr1 == NULL) {
        printf("Cannot open file %s \n", filename);
        exit(0); // Exit if file cannot be opened
    }

    // Step 3: Enter the destination filename
    printf("Enter the filename to open for writing: \n");
    scanf("%s", filename);

    // Step 3: Open the destination file for writing
    fptr2 = fopen(filename, "w+");
    if (fptr2 == NULL) {
        printf("Cannot open file %s \n", filename);
        fclose(fptr1); // Close the source file before exiting
        exit(0);
    }

    // Step 4: Read from the source file and write to the destination file
    c = fgetc(fptr1); // Read a character from the source file
    while (c != EOF) {
        fputc(c, fptr2); // Write the character to the destination file
        c = fgetc(fptr1); // Read the next character
    }

    // Display success message
    printf("\nContents copied to %s", filename);

    // Close both files
    fclose(fptr1);
    fclose(fptr2);

    return 0;
}

