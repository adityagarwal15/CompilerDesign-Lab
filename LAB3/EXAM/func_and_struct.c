#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 100

// List of C keywords
const char *keywords[] = {"int", "float", "char", "void", "return", "struct", "double", "long"};
const int keywordCount = 8;

// Function to check if a word is a keyword
bool isKeyword(char *word) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Function to check if a character is an operator
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<');
}

// Function to check if a character is a delimiter
bool isDelimiter(char c) {
    return (c == ' ' || c == ',' || c == ';' || c == '{' || c == '}' || c == '(' || c == ')');
}

// Function to tokenize the input C code
void tokenize(char *code) {
    int i = 0;
    while (code[i] != '\0') {
        // Skip whitespace
        if (isspace(code[i])) {
            i++;
            continue;
        }

        // Detect numbers
        if (isdigit(code[i])) {
            char num[MAX_LEN] = "";
            int j = 0;
            while (isdigit(code[i]) || code[i] == '.') {
                num[j++] = code[i++];
            }
            num[j] = '\0';
            printf("Number: %s\n", num);
            continue;
        }

        // Detect keywords and identifiers
        if (isalpha(code[i]) || code[i] == '_') {
            char word[MAX_LEN] = "";
            int j = 0;
            while (isalnum(code[i]) || code[i] == '_') {
                word[j++] = code[i++];
            }
            word[j] = '\0';

            if (isKeyword(word)) {
                printf("Keyword: %s\n", word);
            } else {
                printf("Identifier: %s\n", word);
            }
            continue;
        }

        // Detect operators
        if (isOperator(code[i])) {
            printf("Operator: %c\n", code[i]);
            i++;
            continue;
        }

        // Detect punctuation and delimiters
        if (isDelimiter(code[i])) {
            printf("Delimiter: %c\n", code[i]);
            i++;
            continue;
        }

        // Detect strings
        if (code[i] == '"') {
            char str[MAX_LEN] = "";
            int j = 0;
            str[j++] = code[i++];
            while (code[i] != '"' && code[i] != '\0') {
                str[j++] = code[i++];
            }
            if (code[i] == '"') {
                str[j++] = code[i++];
            }
            str[j] = '\0';
            printf("String: %s\n", str);
            continue;
        }

        // Detect comments
        if (code[i] == '/' && code[i + 1] == '/') {
            while (code[i] != '\n' && code[i] != '\0') {
                i++;
            }
            printf("Single-line Comment Detected\n");
            continue;
        }
        if (code[i] == '/' && code[i + 1] == '*') {
            i += 2;
            while (code[i] != '*' || code[i + 1] != '/') {
                i++;
            }
            i += 2;
            printf("Multi-line Comment Detected\n");
            continue;
        }

        // Move to the next character
        i++;
    }
}

int main() {
    char code[MAX_LEN * 10] = 
        "struct Person { char name[50]; int age; }; \n"
        "int sum(int a, int b) { return a + b; } \n"
        "void display() { printf(\"Hello\"); } \n";

    printf("Tokenizing the given C code:\n");
    tokenize(code);
    return 0;
}
