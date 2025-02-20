#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define PREPROCESSOR 6
#define COMMENT 7

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
    int size;
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;

int isKeyword(char *lexeme) {
    // PHP keywords
    const char *keywords[] = {"int", "float", "string", "bool", "array", "if", "else", 
                             "while", "for", "foreach", "function", "class", "public", 
                             "private", "protected", "return", "require", "include", 
                             "namespace", "use", "const", "static", "var"};
    for (int i = 0; i < 23; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

Token getNextToken(FILE *fp, int *row, int *col) {
    Token t = {-1, -1, 0, ""};
    char c, lexeme[100];
    int lexemeIndex = 0, inStringLiteral = 0, inComment = 0;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;
        if (isspace(c)) {
            if (c == '\n') { (*row)++; *col = 0; }
            continue;
        }
        
        // Handle PHP opening tag
        if (c == '<' && *col == 1) {
            char peek = fgetc(fp);
            if (peek == '?') {
                t.type = PREPROCESSOR;
                t.value[0] = c;
                t.value[1] = peek;
                
                // Check if it's "<?php"
                char php[4] = {0};
                for (int i = 0; i < 3; i++) {
                    php[i] = fgetc(fp);
                    strncat(t.value, &php[i], 1);
                }
                php[3] = '\0';
                
                if (strncmp(php, "php", 3) != 0) {
                    // Skip to end of line if not "<?php"
                    while ((c = fgetc(fp)) != '\n' && c != EOF) {
                        strncat(t.value, &c, 1);
                    }
                }
                (*row)++;
                (*col) = 0;
                return t;
            } else {
                ungetc(peek, fp);
            }
        }
        
        // Handle PHP closing tag
        if (c == '?' && (peek = fgetc(fp)) == '>') {
            t.type = PREPROCESSOR;
            t.value[0] = c;
            t.value[1] = peek;
            t.value[2] = '\0';
            return t;
        } else if (peek != EOF) {
            ungetc(peek, fp);
        }

        // Handle comments (both PHP style)
        if (!inStringLiteral && c == '/') {
            char peek = fgetc(fp);
            if (peek == '/') {
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
            } else if (peek == '*') {
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
                return t;
            } else {
                ungetc(peek, fp);
            }
        }
        
        // Handle PHP specific comment: #
        if (c == '#' && !inStringLiteral) {
            t.type = COMMENT;
            t.value[0] = c;
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                strncat(t.value, &c, 1);
            }
            (*row)++;
            (*col) = 0;
            return t;
        }

        // Handle string literals (both single and double quotes in PHP)
        if ((c == '"' || c == '\'') && !inComment) {
            char quote = c;
            inStringLiteral = !inStringLiteral;
            t.type = STRING_LITERAL;
            t.value[0] = c;
            while ((c = fgetc(fp)) != quote && c != EOF) {
                if (c == '\\') {
                    strncat(t.value, &c, 1);
                    c = fgetc(fp);  // Skip escaped character
                }
                strncat(t.value, &c, 1);
            }
            if (c == quote) {
                strncat(t.value, &c, 1);  // Add closing quote
            }
            return t;
        }

        // Handle variables (PHP variables start with $)
        if (c == '$') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            t.type = IDENTIFIER;
            strcpy(t.value, lexeme);
            (*col) += lexemeIndex;
            return t;
        }

        // Handle keywords and identifiers
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
            (*col) += lexemeIndex;
            return t;
        }

        // Handle numerical constants
        if (isdigit(c)) {
            lexeme[lexemeIndex++] = c;
            while (isdigit(c = fgetc(fp)) || c == '.') {
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

        // Handle operators and special symbols (PHP specific ones added)
        if (strchr("+-*/=<>!&|^%,;(){}[].:?=>@", c)) {
            t.row = *row;
            t.col = *col;
            t.type = OPERATOR;
            t.value[0] = c;
            
            // Check for two-character operators
            char peek = fgetc(fp);
            if ((c == '=' && peek == '=') || 
                (c == '!' && peek == '=') ||
                (c == '>' && peek == '=') ||
                (c == '<' && peek == '=') ||
                (c == '=' && peek == '>') ||
                (c == '&' && peek == '&') ||
                (c == '|' && peek == '|') ||
                (c == '+' && peek == '+') ||
                (c == '-' && peek == '-') ||
                (c == '-' && peek == '>')) {
                t.value[1] = peek;
                t.value[2] = '\0';
            } else {
                t.value[1] = '\0';
                if (peek != EOF) ungetc(peek, fp);
            }
            return t;
        }
    }

    t.row = *row;
    t.col = *col;
    strcpy(t.value, "EOF");
    return t;
}

void insertToken(char *name, char *type, int size) {
    // Check if token already exists in symbol table
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return; // Skip duplicates
        }
    }
    
    if (symbolTableSize < 100) {
        symbolTable[symbolTableSize].index = symbolTableSize + 1;
        strcpy(symbolTable[symbolTableSize].name, name);
        strcpy(symbolTable[symbolTableSize].type, type);
        symbolTable[symbolTableSize].size = size;
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\n--- PHP Symbol Table ---\n");
    printf("Index\tName\tType\tSize\n-----\t----\t----\t----\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%d\t%s\t%s\t%d\n", symbolTable[i].index, symbolTable[i].name, symbolTable[i].type, symbolTable[i].size);
    }
}

int getSizeForType(const char *type) {
    if (strcmp(type, "int") == 0) return 4;
    if (strcmp(type, "float") == 0) return 8;
    if (strcmp(type, "string") == 0) return 16;
    if (strcmp(type, "bool") == 0) return 1;
    if (strcmp(type, "array") == 0) return 24;
    return 8; // Default size for PHP variables
}

int main() {
    FILE *fp = fopen("sample.php", "r");
    if (!fp) {
        printf("Error: Could not open sample.php\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    char currentType[20] = "";
    int inVarDeclaration = 0;

    printf("Starting PHP lexical analysis...\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        // Skip PHP tags
        if (t.type == PREPROCESSOR) {
            if (strstr(t.value, "<?php") != NULL) {
                printf("Found PHP opening tag: %s\n", t.value);
            } else if (strstr(t.value, "?>") != NULL) {
                printf("Found PHP closing tag: %s\n", t.value);
            }
            continue;
        }
        
        if (t.type == KEYWORD) {
            // Check if the keyword is a PHP type
            if (strcmp(t.value, "int") == 0 || 
                strcmp(t.value, "float") == 0 || 
                strcmp(t.value, "string") == 0 ||
                strcmp(t.value, "bool") == 0 ||
                strcmp(t.value, "array") == 0) {
                strcpy(currentType, t.value);
                inVarDeclaration = 1;
            }
        } else if (t.type == IDENTIFIER) {
            // For PHP variables (starting with $)
            if (t.value[0] == '$') {
                // Determine variable type
                char varType[20] = "var"; // Default to 'var' type
                if (strcmp(currentType, "") != 0) {
                    strcpy(varType, currentType);
                }
                
                int size = getSizeForType(varType);
                insertToken(t.value, varType, size);
                
                if (inVarDeclaration) {
                    strcpy(currentType, ""); // Reset type after declaration
                    inVarDeclaration = 0;
                }
            } else if (strcmp(t.value, "function") == 0) {
                // Handle function declarations
                Token funcName = getNextToken(fp, &row, &col);
                if (funcName.type == IDENTIFIER) {
                    insertToken(funcName.value, "function", 0);
                }
            } else if (strcmp(t.value, "class") == 0) {
                // Handle class declarations
                Token className = getNextToken(fp, &row, &col);
                if (className.type == IDENTIFIER) {
                    insertToken(className.value, "class", 0);
                }
            }
        }
    }

    displaySymbolTable();
    fclose(fp);
    printf("\nPHP lexical analysis completed.\n");
    return 0;
}
