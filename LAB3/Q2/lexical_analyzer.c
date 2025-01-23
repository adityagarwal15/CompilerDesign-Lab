#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types for better readability
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define PREPROCESSOR 6
#define COMMENT 7

// Token structure to store row, column, type, and value
typedef struct {
    int row;
    int col;
    int type;
    char value[100];
} Token;

// Function to check if the token is a keyword
int isKeyword(char *lexeme) {
    const char *keywords[] = {"int", "char", "if", "else", "return", "for", "while", "void"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to print token information
void printToken(Token t) {
    const char *types[] = {"Unknown", "Keyword", "Identifier", "Operator", "Numerical Constant", "String Literal", "Preprocessor Directive", "Comment"};
    printf("Row: %d, Col: %d, Type: %s, Value: %s\n", t.row, t.col, types[t.type], t.value);
}

// Function to get the next token
Token getNextToken(FILE *fp, int *row, int *col) {
    Token t = {-1, -1, 0, ""}; // Initialize token
    char c;
    char lexeme[100];
    int lexemeIndex = 0;
    int inStringLiteral = 0;
    int inComment = 0;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;
        
        // Skip whitespace and handle newlines for row counting
        if (isspace(c)) {
            if (c == '\n') {
                (*row)++;
                *col = 0;
            }
            continue;
        }

        // Skip preprocessor directives
        if (c == '#' && *col == 1) {
            t.type = PREPROCESSOR;
            t.value[0] = c;
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                strncat(t.value, &c, 1);
            }
            (*row)++;
            (*col) = 0;
            return t;
        }

        // Handle comments
        if (!inStringLiteral && c == '/') {
            c = fgetc(fp);
            if (c == '/') {
                inComment = 1; // Single-line comment
                t.type = COMMENT;
                t.value[0] = '/';
                t.value[1] = '/';
                while ((c = fgetc(fp)) != '\n' && c != EOF) {
                    strncat(t.value, &c, 1);
                }
                (*row)++;
                (*col) = 0;
                return t;
            } else if (c == '*') {
                inComment = 1; // Multi-line comment
                t.type = COMMENT;
                t.value[0] = '/';
                t.value[1] = '*';
                while ((c = fgetc(fp)) != EOF) {
                    strncat(t.value, &c, 1);
                    if (c == '*' && (c = fgetc(fp)) == '/') {
                        strncat(t.value, &c, 1);
                        break;
                    }
                }
                (*row)++;
                (*col) = 0;
                return t;
            }
        }

        // Handle string literals
        if (c == '"' && !inComment) {
            inStringLiteral = !inStringLiteral;
            t.type = STRING_LITERAL;
            t.value[0] = c;
            while ((c = fgetc(fp)) != '"' && c != EOF) {
                strncat(t.value, &c, 1);
            }
            t.value[strlen(t.value)] = '"'; // Closing quote
            (*row)++;
            (*col) = 0;
            return t;
        }

        // Handle keywords and identifiers
        if (isalpha(c) || c == '_') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp); // Put the non-identifier character back
            t.row = *row;
            t.col = *col;
            if (isKeyword(lexeme)) {
                t.type = KEYWORD;
            } else {
                t.type = IDENTIFIER;
            }
            strcpy(t.value, lexeme);
            (*col) += lexemeIndex;
            return t;
        }

        // Handle numerical constants
        if (isdigit(c)) {
            lexeme[lexemeIndex++] = c;
            while (isdigit(c = fgetc(fp))) {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            t.type = NUMERIC_CONSTANT;
            strcpy(t.value, lexeme);
            (*col) += lexemeIndex;
            return t;
        }

        // Handle operators and special symbols
        if (strchr("+-*/=<>!&|^%,;(){}", c)) {
            t.row = *row;
            t.col = *col;
            t.type = OPERATOR;
            t.value[0] = c;
            t.value[1] = '\0';
            return t;
        }
    }

    t.row = *row;
    t.col = *col;
    t.type = 0;
    strcpy(t.value, "EOF");
    return t;
}

int main() {
    FILE *fp = fopen("sample.c", "r"); // Open the source file
    if (fp == NULL) {
        printf("Cannot open file\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        printToken(t); // Print the token details
    }

    fclose(fp);
    return 0;
}

