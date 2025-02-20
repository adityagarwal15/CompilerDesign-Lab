#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types for C#
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define VERBATIM_STRING 6
#define COMMENT 7
#define PREPROCESSOR 8
#define ATTRIBUTE 9
#define PUNCTUATOR 10

typedef struct {
    int row;
    int col;
    int type;
    char value[256];
} Token;

typedef struct {
    int index;
    char name[100];
    char type[50];     // Increased for C# types
    char modifier[30];  // public, private, etc.
    char namespace[50]; // Namespace tracking
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;
char currentNamespace[50] = "global";
char currentModifier[30] = "";

int isKeyword(char *lexeme) {
    // C# keywords
    const char *keywords[] = {
        "abstract", "as", "base", "bool", "break", "byte", "case", "catch",
        "char", "checked", "class", "const", "continue", "decimal", "default",
        "delegate", "do", "double", "else", "enum", "event", "explicit",
        "extern", "false", "finally", "fixed", "float", "for", "foreach",
        "goto", "if", "implicit", "in", "int", "interface", "internal",
        "is", "lock", "long", "namespace", "new", "null", "object",
        "operator", "out", "override", "params", "private", "protected",
        "public", "readonly", "ref", "return", "sbyte", "sealed",
        "short", "sizeof", "stackalloc", "static", "string", "struct",
        "switch", "this", "throw", "true", "try", "typeof", "uint",
        "ulong", "unchecked", "unsafe", "ushort", "using", "virtual",
        "void", "volatile", "while", "var", "async", "await"
    };
    
    for (int i = 0; i < 80; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

int isModifier(char *lexeme) {
    const char *modifiers[] = {
        "public", "private", "protected", "internal", "static",
        "readonly", "const", "virtual", "abstract", "override",
        "sealed", "async"
    };
    
    for (int i = 0; i < 12; i++) {
        if (strcmp(lexeme, modifiers[i]) == 0) return 1;
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

        // Handle preprocessor directives
        if (c == '#') {
            t.type = PREPROCESSOR;
            t.value[0] = c;
            int i = 1;
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                t.value[i++] = c;
            }
            t.value[i] = '\0';
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle attributes [Attribute]
        if (c == '[') {
            char next = fgetc(fp);
            if (isalpha(next)) {
                t.type = ATTRIBUTE;
                t.value[0] = '[';
                int i = 1;
                t.value[i++] = next;
                while ((c = fgetc(fp)) != ']' && c != EOF) {
                    t.value[i++] = c;
                }
                t.value[i++] = ']';
                t.value[i] = '\0';
                t.row = *row;
                t.col = *col;
                return t;
            } else {
                ungetc(next, fp);
            }
        }

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
                t.value[i++] = '/';
                t.value[i] = '\0';
                t.row = *row;
                t.col = *col;
                return t;
            } else {
                ungetc(next, fp);
            }
        }

        // Handle string literals
        if (c == '"') {
            char prev = fgetc(fp);
            ungetc(prev, fp);
            if (prev == '"') {  // Verbatim string @""
                t.type = VERBATIM_STRING;
                t.value[0] = '@';
                t.value[1] = '"';
                int i = 2;
                while ((c = fgetc(fp)) != EOF) {
                    t.value[i++] = c;
                    if (c == '"' && (c = fgetc(fp)) != '"') {
                        ungetc(c, fp);
                        break;
                    }
                }
                t.value[i] = '\0';
            } else {  // Regular string
                t.type = STRING_LITERAL;
                t.value[0] = c;
                int i = 1;
                while ((c = fgetc(fp)) != '"' && c != EOF) {
                    if (c == '\\') {
                        t.value[i++] = c;
                        c = fgetc(fp);
                    }
                    t.value[i++] = c;
                }
                t.value[i++] = '"';
                t.value[i] = '\0';
            }
            t.row = *row;
            t.col = *col;
            return t;
        }

        // Handle identifiers and keywords
        if (isalpha(c) || c == '_' || c == '@') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
                lexeme[lexemeIndex++] = c;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            t.row = *row;
            t.col = *col;
            
            if (isKeyword(lexeme)) {
                t.type = KEYWORD;
                if (isModifier(lexeme)) {
                    strcpy(currentModifier, lexeme);
                }
            } else {
                t.type = IDENTIFIER;
            }
            
            strcpy(t.value, lexeme);
            return t;
        }

        // Handle numeric literals
        if (isdigit(c) || (c == '.' && isdigit(fgetc(fp)))) {
            ungetc(c, fp);
            lexeme[lexemeIndex++] = c;
            int hasDecimal = (c == '.');
            while ((c = fgetc(fp)) != EOF) {
                if (isdigit(c) || 
                    (c == '.' && !hasDecimal) || 
                    c == 'f' || c == 'F' || 
                    c == 'd' || c == 'D' || 
                    c == 'm' || c == 'M' ||
                    c == 'e' || c == 'E') {
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
            
            // Handle C# specific operators
            if ((c == '=' && (next == '=' || next == '>')) ||
                (c == '!' && next == '=') ||
                (c == '<' && (next == '=' || next == '<')) ||
                (c == '>' && (next == '=' || next == '>')) ||
                (c == '+' && (next == '=' || next == '+')) ||
                (c == '-' && (next == '=' || next == '-')) ||
                (c == '*' && next == '=') ||
                (c == '/' && next == '=') ||
                (c == '%' && next == '=') ||
                (c == '&' && (next == '&' || next == '=')) ||
                (c == '|' && (next == '|' || next == '=')) ||
                (c == '^' && next == '=') ||
                (c == '?' && next == '?')) {
                t.value[1] = next;
                t.value[2] = '\0';
                
                // Check for >>= and <<=
                if ((c == '>' || c == '<') && next == c) {
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
        strcpy(symbolTable[symbolTableSize].modifier, currentModifier);
        strcpy(symbolTable[symbolTableSize].namespace, currentNamespace);
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\nSymbol Table\n");
    printf("Index\tName\t\tType\t\tModifier\tNamespace\n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%d\t%-15s\t%-15s\t%-15s\t%s\n", 
               symbolTable[i].index, 
               symbolTable[i].name, 
               symbolTable[i].type, 
               symbolTable[i].modifier,
               symbolTable[i].namespace);
    }
}

const char* getTokenTypeName(int type) {
    switch(type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case OPERATOR: return "OPERATOR";
        case NUMERIC_CONSTANT: return "NUMERIC";
        case STRING_LITERAL: return "STRING";
        case VERBATIM_STRING: return "VERBATIM_STR";
        case COMMENT: return "COMMENT";
        case PREPROCESSOR: return "PREPROCESSOR";
        case ATTRIBUTE: return "ATTRIBUTE";
        case PUNCTUATOR: return "PUNCTUATOR";
        default: return "UNKNOWN";
    }
}

int main() {
    FILE *fp = fopen("sample.cs", "r");
    if (!fp) {
        printf("Error: Could not open sample.cs\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    char lastType[50] = "";
    
    printf("Lexical Analysis\n");
    printf("Row\tCol\tType\t\tValue\n");
    printf("------------------------------------------------\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        // Print token information
        printf("%d\t%d\t%-15s\t%s\n", 
               t.row, t.col, 
               getTokenTypeName(t.type), 
               t.value);

        // Track namespace
        if (t.type == KEYWORD && strcmp(t.value, "namespace") == 0) {
            Token namespaceToken = getNextToken(fp, &row, &col);
            strcpy(currentNamespace, namespaceToken.value);
        }

        // Update symbol table
        if (t.type == IDENTIFIER) {
            Token nextToken = getNextToken(fp, &row, &col);
            ungetc(nextToken.value[0], fp);

            if (strcmp(lastType, "class") == 0) {
                insertToken(t.value, "class");
            } else if (strcmp(lastType, "interface") == 0) {
                insertToken(t.value, "interface");
            } else if (strcmp(lastType, "enum") == 0) {
                insertToken(t.value, "enum");
            } else if (nextToken.value[0] == '(') {
                insertToken(t.value, "method");
            } else {
                insertToken(t.value, "variable");
            }
        }

        if (t.type == KEYWORD) {
            strcpy(lastType, t.value);
        }
    }

    displaySymbolTable();
    fclose(fp);
    return 0;
}
