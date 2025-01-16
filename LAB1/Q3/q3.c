#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file1, *file2, *resultFile;
    char file1Name[100], file2Name[100], resultFileName[100];
    char line[256]; // Buffer to hold a single line of text

    // Prompt the user for the filenames
    printf("Enter the name of the first file: ");
    scanf("%s", file1Name);

    printf("Enter the name of the second file: ");
    scanf("%s", file2Name);

    printf("Enter the name of the resultant file: ");
    scanf("%s", resultFileName);

    // Open the files
    file1 = fopen(file1Name, "r");
    if (file1 == NULL) {
        printf("Cannot open file %s\n", file1Name);
        exit(0);
    }

    file2 = fopen(file2Name, "r");
    if (file2 == NULL) {
        printf("Cannot open file %s\n", file2Name);
        fclose(file1);
        exit(0);
    }

    resultFile = fopen(resultFileName, "w");
    if (resultFile == NULL) {
        printf("Cannot create file %s\n", resultFileName);
        fclose(file1);
        fclose(file2);
        exit(0);
    }

    // Read and merge lines alternately
    int toggle = 0; // Used to switch between files
    while (1) {
        if (toggle == 0 && fgets(line, sizeof(line), file1)) {
            fputs(line, resultFile);
        } else if (toggle == 1 && fgets(line, sizeof(line), file2)) {
            fputs(line, resultFile);
        } else {
            // If one file is exhausted, continue with the other
            if (feof(file1) && feof(file2)) break;
        }
        toggle = 1 - toggle; // Switch between file1 and file2
    }

    printf("Lines have been merged alternately into %s\n", resultFileName);

    // Close all files
    fclose(file1);
    fclose(file2);
    fclose(resultFile);

    return 0;
}

