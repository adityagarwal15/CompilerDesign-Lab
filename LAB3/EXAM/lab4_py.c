#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types for Python
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define COMMENT 6
#define INDENTATION 7
#define NEWLINE 8

typedef struct {
    int row;
    int col;
    int type;
    char value[100];
} Token;

typedef struct {
    int index;
    char name[100];
    char type[20];
    int scope;  // Added scope for Python variables
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;
int currentScope = 0;

int isKeyword(char *lexeme) {
    // Python keywords
    const char *keywords[] = {
        "False", "None", "True", "and", "as", "assert", "break", 
        "class", "continue", "def", "del", "elif", "else", "except", 
        "finally", "for", "from", "global", "if", "import", "in", 
        "is", "lambda", "nonlocal", "not", "or", "pass", "raise", 
        "return", "try", "while", "with", "yield"
    };
    for (int i = 0; i < 33; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

Token getNextToken(FILE *fp, int *row, int *col) {
    Token t = {-1, -1, 0, ""};
    char c, lexeme[100];
    int lexemeIndex = 0;
    int inStringLiteral = 0;
    int indentationLevel = 0;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;
        
        // Handle indentation at start of line
        if (*col == 1) {
            while (c == ' ' || c == '\t') {
                indentationLevel++;
                c = fgetc(fp);
                (*col)++;
            }
            if (indentationLevel > 0) {
                ungetc(c, fp);
                (*col)--;
                t.type = INDENTATION;
                sprintf(t.value, "%d", indentationLevel);
                t.row = *row;
                t.col = *col;
                return t;
            }
        }

        if (c == '\n') {
            t.type = NEWLINE;
            t.row = *row;
            t.col = *col;
            strcpy(t.value, "\\n");
            (*row)++;
            *col = 0;
            return t;
        }

        if (isspace(c)) continue;

        // Handle comments
        if (c == '#') {
            t.type = COMMENT;
            t.value[0] = c;
            int i = 1;
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                t.value[i++] = c;
            }
            t.value[i] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle string literals (both single and double quotes)
        if (c == '"' || c == '\'') {
            char quote = c;
            t.type = STRING_LITERAL;
            t.value[0] = c;
            int i = 1;
            while ((c = fgetc(fp)) != quote && c != EOF) {
                t.value[i++] = c;
                if (c == '\\') {  // Handle escape sequences
                    c = fgetc(fp);
                    t.value[i++] = c;
                }
            }
            t.value[i++] = quote;
            t.value[i] = '\0';
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle identifiers and keywords
        if (isalpha(c) || c == '_') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            t.type = isKeyword(lexeme) ? KEYWORD : IDENTIFIER;
            strcpy(t.value, lexeme);
            return t;
        }

        // Handle numeric literals
        if (isdigit(c)) {
            lexeme[lexemeIndex++] = c;
            while (isdigit(c = fgetc(fp)) || c == '.' || c == 'e' || c == 'E') {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            t.type = NUMERIC_CONSTANT;
            strcpy(t.value, lexeme);
            return t;
        }

        // Handle Python operators
        if (strchr("+-*/%<>=!&|^~:()[]{}.,", c)) {
            t.type = OPERATOR;
            t.value[0] = c;
            // Handle double-character operators
            char next = fgetc(fp);
            if ((c == '=' && strchr("=<>!", next)) ||
                (c == '<' && next == '<') ||
                (c == '>' && next == '>') ||
                (c == '*' && next == '*') ||
                (c == '/' && next == '/')) {
                t.value[1] = next;
                t.value[2] = '\0';
            } else {
                ungetc(next, fp);
                t.value[1] = '\0';
            }
            t.row = *row;
            t.col = *col;
            return t;
        }
    }

    t.row = *row;
    t.col = *col;
    strcpy(t.value, "EOF");
    return t;
}

void insertToken(char *name, int scope) {
    if (symbolTableSize < 100) {
        symbolTable[symbolTableSize].index = symbolTableSize + 1;
        strcpy(symbolTable[symbolTableSize].name, name);
        strcpy(symbolTable[symbolTableSize].type, "variable");  // Python is dynamically typed
        symbolTable[symbolTableSize].scope = scope;
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\nSymbol Table\n");
    printf("Index\tName\t\tType\t\tScope\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%d\t%-15s\t%-15s\t%d\n", 
               symbolTable[i].index, 
               symbolTable[i].name, 
               symbolTable[i].type, 
               symbolTable[i].scope);
    }
}

const char* getTokenTypeName(int type) {
    switch(type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case OPERATOR: return "OPERATOR";
        case NUMERIC_CONSTANT: return "NUMERIC";
        case STRING_LITERAL: return "STRING";
        case COMMENT: return "COMMENT";
        case INDENTATION: return "INDENT";
        case NEWLINE: return "NEWLINE";
        default: return "UNKNOWN";
    }
}

int main() {
    FILE *fp = fopen("sample.py", "r");
    if (!fp) {
        printf("Error: Could not open sample.py\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    
    printf("Lexical Analysis\n");
    printf("Row\tCol\tType\t\tValue\n");
    printf("----------------------------------------\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        // Print token information
        printf("%d\t%d\t%-15s\t%s\n", 
               t.row, t.col, 
               getTokenTypeName(t.type), 
               t.value);

        // Update symbol table for identifiers
        if (t.type == IDENTIFIER) {
            insertToken(t.value, currentScope);
        }
        
        // Track scope changes
        if (t.type == INDENTATION) {
            currentScope++;
        } else if (t.type == NEWLINE && col == 1) {
            if (currentScope > 0) currentScope--;
        }
    }

    displaySymbolTable();
    fclose(fp);
    return 0;
}
