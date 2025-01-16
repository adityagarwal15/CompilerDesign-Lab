#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_KEYWORDS 32

// List of C keywords
const char *keywords[MAX_KEYWORDS] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", 
    "else", "enum", "extern", "for", "goto", "if", "inline", "int", "long", 
    "register", "restrict", "return", "short", "signed", "sizeof", "static", 
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

// Function to check if a word is a C keyword
int is_keyword(const char *word) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1; // It's a keyword
        }
    }
    return 0; // Not a keyword
}

// Function to convert a string to uppercase
void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char) *str);
        str++;
    }
}

int main() {
    FILE *fa, *fb;
    char word[100]; // Buffer to store words
    int ca, i = 0;

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

    // Read characters from the input file
    ca = fgetc(fa);
    while (ca != EOF) {
        // If the character is part of a word (alphanumeric or underscore), collect it
        if (isalnum(ca) || ca == '_') {
            word[i++] = ca;  // Add character to the word buffer
        } else {
            // If we have a word and it is a keyword, convert it to uppercase
            if (i > 0) {
                word[i] = '\0'; // Null-terminate the word
                if (is_keyword(word)) {
                    to_uppercase(word); // Convert to uppercase if it's a keyword
                }
                fputs(word, fb); // Write the word to the output file
                i = 0; // Reset the word index
            }

            // Write non-alphanumeric characters (like spaces, operators, etc.) as they are
            fputc(ca, fb);
        }

        // Read the next character
        ca = fgetc(fa);
    }

    // Handle any remaining word after the last character
    if (i > 0) {
        word[i] = '\0'; // Null-terminate the word
        if (is_keyword(word)) {
            to_uppercase(word); // Convert to uppercase if it's a keyword
        }
        fputs(word, fb); // Write the word to the output file
    }

    // Close the files
    fclose(fa);
    fclose(fb);

    printf("Processing complete. Output written to 'output.c'.\n");

    return 0;
}

