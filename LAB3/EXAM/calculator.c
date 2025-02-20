#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define NUMBER 1
#define OPERATOR 2
#define PARENTHESIS 3
#define END 4

// Token structure
typedef struct {
    int type;
    char value[50];
} Token;

// Function to classify and extract tokens
Token getNextToken(char *expr, int *index) {
    Token t;
    t.value[0] = '\0'; // Reset token value

    // Skip whitespace
    while (expr[*index] == ' ') {
        (*index)++;
    }

    char c = expr[*index];

    // If end of expression, return END token
    if (c == '\0') {
        t.type = END;
        strcpy(t.value, "EOF");
        return t;
    }

    // Recognize numbers (including decimals)
    if (isdigit(c)) {
        int i = 0;
        while (isdigit(expr[*index]) || expr[*index] == '.') {
            t.value[i++] = expr[*index];
            (*index)++;
        }
        t.value[i] = '\0';
        t.type = NUMBER;
        return t;
    }

    // Recognize operators
    if (strchr("+-*/", c)) {
        t.type = OPERATOR;
        t.value[0] = c;
        t.value[1] = '\0';
        (*index)++;
        return t;
    }

    // Recognize parentheses
    if (c == '(' || c == ')') {
        t.type = PARENTHESIS;
        t.value[0] = c;
        t.value[1] = '\0';
        (*index)++;
        return t;
    }

    // Invalid character handling
    t.type = -1;
    t.value[0] = c;
    t.value[1] = '\0';
    (*index)++;
    return t;
}

int main() {
    char expr[100];
    printf("Enter an arithmetic expression: ");
    fgets(expr, sizeof(expr), stdin); // Read input

    int index = 0;
    Token t;

    // Tokenize the input
    while ((t = getNextToken(expr, &index)).type != END) {
        if (t.type == -1) {
            printf("Error: Invalid character '%s'\n", t.value);
            continue;
        }
        printf("Token Type: %d, Value: %s\n", t.type, t.value);
    }

    return 0;
}
