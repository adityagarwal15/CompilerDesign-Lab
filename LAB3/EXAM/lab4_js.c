#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types for JavaScript
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define TEMPLATE_LITERAL 6
#define COMMENT 7
#define PUNCTUATOR 8
#define REGEX_LITERAL 9

typedef struct {
    int row;
    int col;
    int type;
    char value[256];  // Increased size for template literals
} Token;

typedef struct {
    int index;
    char name[100];
    char type[30];    // Increased size for JS types
    char scope[50];   // Added scope name (global, function, block)
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;
char currentScope[50] = "global";

int isKeyword(char *lexeme) {
    // JavaScript keywords
    const char *keywords[] = {
        "break", "case", "catch", "class", "const", "continue", "debugger",
        "default", "delete", "do", "else", "export", "extends", "finally",
        "for", "function", "if", "import", "in", "instanceof", "let", "new",
        "return", "super", "switch", "this", "throw", "try", "typeof",
        "var", "void", "while", "with", "yield", "async", "await", "null",
        "undefined", "true", "false"
    };
    for (int i = 0; i < 40; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

Token getNextToken(FILE *fp, int *row, int *col) {
    Token t = {-1, -1, 0, ""};
    char c, lexeme[256];
    int lexemeIndex = 0;
    
    while ((c = fgetc(fp)) != EOF) {
        (*col)++;

        if (c == '\n') {
            (*row)++;
            *col = 0;
            continue;
        }

        if (isspace(c)) continue;

        // Handle comments
        if (c == '/') {
            char next = fgetc(fp);
            if (next == '/') {  // Single-line comment
                t.type = COMMENT;
                t.value[0] = '/';
                t.value[1] = '/';
                int i = 2;
                while ((c = fgetc(fp)) != '\n' && c != EOF) {
                    t.value[i++] = c;
                }
                t.value[i] = '\0';
                t.row = *row;
                t.col = *col;
                return t;
            } else if (next == '*') {  // Multi-line comment
                t.type = COMMENT;
                t.value[0] = '/';
                t.value[1] = '*';
                int i = 2;
                char prev = '\0';
                while ((c = fgetc(fp)) != EOF && !(prev == '*' && c == '/')) {
                    t.value[i++] = c;
                    if (c == '\n') {
                        (*row)++;
                        *col = 0;
                    }
                    prev = c;
                }
                t.value[i] = '/';
                t.value[i+1] = '\0';
                t.row = *row;
                t.col = *col;
                return t;
            } else {
                ungetc(next, fp);
            }
        }

        // Handle template literals
        if (c == '`') {
            t.type = TEMPLATE_LITERAL;
            t.value[0] = c;
            int i = 1;
            while ((c = fgetc(fp)) != '`' && c != EOF) {
                t.value[i++] = c;
                if (c == '$' && (c = fgetc(fp)) == '{') {
                    t.value[i++] = '{';
                    int braceCount = 1;
                    while (braceCount > 0 && (c = fgetc(fp)) != EOF) {
                        t.value[i++] = c;
                        if (c == '{') braceCount++;
                        if (c == '}') braceCount--;
                    }
                }
            }
            t.value[i++] = '`';
            t.value[i] = '\0';
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle string literals
        if (c == '"' || c == '\'') {
            char quote = c;
            t.type = STRING_LITERAL;
            t.value[0] = c;
            int i = 1;
            while ((c = fgetc(fp)) != quote && c != EOF) {
                if (c == '\\') {
                    t.value[i++] = c;
                    c = fgetc(fp);
                }
                t.value[i++] = c;
            }
            t.value[i++] = quote;
            t.value[i] = '\0';
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle identifiers and keywords
        if (isalpha(c) || c == '_' || c == '$') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_' || c == '$') {
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

        // Handle numeric literals (including hex, octal, binary, and scientific notation)
        if (isdigit(c) || (c == '.' && isdigit(fgetc(fp)))) {
            ungetc(c, fp);
            lexeme[lexemeIndex++] = c;
            int hasDecimal = (c == '.');
            while ((c = fgetc(fp)) != EOF) {
                if (isdigit(c) || (c == '.' && !hasDecimal) || 
                    c == 'e' || c == 'E' || c == 'x' || c == 'X' ||
                    (lexeme[lexemeIndex-1] == 'e' && (c == '+' || c == '-'))) {
                    lexeme[lexemeIndex++] = c;
                    if (c == '.') hasDecimal = 1;
                } else break;
            }
            ungetc(c, fp);
            lexeme[lexemeIndex] = '\0';
            t.type = NUMERIC_CONSTANT;
            strcpy(t.value, lexeme);
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle operators and punctuators
        if (strchr("+-*/%<>=!&|^~?:.,;()[]{}",c)) {
            t.type = OPERATOR;
            t.value[0] = c;
            char next = fgetc(fp);
            // Handle two-character operators
            if ((c == '+' && (next == '+' || next == '=')) ||
                (c == '-' && (next == '-' || next == '=' || next == '>')) ||
                (c == '*' && (next == '*' || next == '=')) ||
                (c == '/' && next == '=') ||
                (c == '%' && next == '=') ||
                (c == '&' && (next == '&' || next == '=')) ||
                (c == '|' && (next == '|' || next == '=')) ||
                (c == '^' && next == '=') ||
                (c == '=' && (next == '=' || next == '>')) ||
                (c == '!' && next == '=') ||
                (c == '<' && (next == '<' || next == '=')) ||
                (c == '>' && (next == '>' || next == '='))) {
                t.value[1] = next;
                t.value[2] = '\0';
                // Check for === and !==
                if ((c == '=' || c == '!') && next == '=') {
                    next = fgetc(fp);
                    if (next == '=') {
                        t.value[2] = '=';
                        t.value[3] = '\0';
                    } else {
                        ungetc(next, fp);
                    }
                }
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

void insertToken(char *name, char *type) {
    if (symbolTableSize < 100) {
        symbolTable[symbolTableSize].index = symbolTableSize + 1;
        strcpy(symbolTable[symbolTableSize].name, name);
        strcpy(symbolTable[symbolTableSize].type, type);
        strcpy(symbolTable[symbolTableSize].scope, currentScope);
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\nSymbol Table\n");
    printf("Index\tName\t\tType\t\tScope\n");
    printf("------------------------------------------------\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%d\t%-15s\t%-15s\t%s\n", 
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
        case TEMPLATE_LITERAL: return "TEMPLATE";
        case COMMENT: return "COMMENT";
        case PUNCTUATOR: return "PUNCTUATOR";
        case REGEX_LITERAL: return "REGEX";
        default: return "UNKNOWN";
    }
}

int main() {
    FILE *fp = fopen("sample.js", "r");
    if (!fp) {
        printf("Error: Could not open sample.js\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    int braceLevel = 0;  // Track nested scopes
    
    printf("Lexical Analysis\n");
    printf("Row\tCol\tType\t\tValue\n");
    printf("------------------------------------------------\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        // Print token information
        printf("%d\t%d\t%-15s\t%s\n", 
               t.row, t.col, 
               getTokenTypeName(t.type), 
               t.value);

        // Track scope changes
        if (t.type == KEYWORD) {
            if (strcmp(t.value, "function") == 0) {
                strcpy(currentScope, "function");
            }
        }
        
        // Handle scope with braces
        if (t.type == OPERATOR) {
            if (t.value[0] == '{') {
                braceLevel++;
                if (strcmp(currentScope, "global") == 0) {
                    sprintf(currentScope, "block_%d", braceLevel);
                }
            } else if (t.value[0] == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    strcpy(currentScope, "global");
                }
            }
        }

        // Update symbol table for identifiers
        if (t.type == IDENTIFIER) {
            Token nextToken = getNextToken(fp, &row, &col);
            ungetc(nextToken.value[0], fp);  // Put back the next token
            
            // Determine type based on context
            if (strcmp(nextToken.value, "=") == 0) {
                insertToken(t.value, "variable");
            } else if (strcmp(nextToken.value, "(") == 0) {
                insertToken(t.value, "function");
            } else {
                insertToken(t.value, "identifier");
            }
        }
    }

    displaySymbolTable();
    fclose(fp);
    return 0;
}
