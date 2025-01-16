#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *sourceFile, *destinationFile;
    char sourceFilename[100], destinationFilename[100];
    long fileSize;
    char *buffer;

    // Prompt the user for the source file name
    printf("Enter the source filename: ");
    scanf("%s", sourceFilename);

    // Open the source file in read mode
    sourceFile = fopen(sourceFilename, "r");
    if (sourceFile == NULL) {
        printf("Cannot open file %s\n", sourceFilename);
        exit(0);
    }

    // Move the file pointer to the end of the file to get the file size
    fseek(sourceFile, 0, SEEK_END);
    fileSize = ftell(sourceFile);
    printf("Size of the file: %ld bytes\n", fileSize);

    // Allocate memory to hold the file contents
    buffer = (char *)malloc(fileSize * sizeof(char));
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        fclose(sourceFile);
        exit(0);
    }

    // Read the file content into the buffer
    fseek(sourceFile, 0, SEEK_SET); // Move file pointer to the beginning
    fread(buffer, sizeof(char), fileSize, sourceFile);

    // Prompt the user for the destination file name
    printf("Enter the destination filename: ");
    scanf("%s", destinationFilename);

    // Open the destination file in write mode
    destinationFile = fopen(destinationFilename, "w");
    if (destinationFile == NULL) {
        printf("Cannot open file %s\n", destinationFilename);
        free(buffer);
        fclose(sourceFile);
        exit(0);
    }

    // Write the contents in reverse order to the destination file
    for (long i = fileSize - 1; i >= 0; i--) {
        fputc(buffer[i], destinationFile);
    }

    printf("File contents have been reversed and stored in %s\n", destinationFilename);

    // Free the allocated memory and close the files
    free(buffer);
    fclose(sourceFile);
    fclose(destinationFile);

    return 0;
}

