#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define NUMERIC_CONSTANT 4
#define STRING_LITERAL 5
#define COMMENT 7
#define MODIFIER 8
#define BOOLEAN_LITERAL 9

typedef struct {
    int row;
    int col;
    int type;
    char value[100];
} Token;

typedef struct {
    int index;
    char name[100];
    char type[50];
    int size;
    char modifier[50]; // For Java access modifiers
    int isArray;       // To track arrays
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;

int isKeyword(char *lexeme) {
    // Java keywords
    const char *keywords[] = {
        "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
        "class", "const", "continue", "default", "do", "double", "else", "enum",
        "extends", "final", "finally", "float", "for", "if", "implements", "import",
        "instanceof", "int", "interface", "long", "native", "new", "package",
        "private", "protected", "public", "return", "short", "static", "strictfp",
        "super", "switch", "synchronized", "this", "throw", "throws", "transient",
        "try", "void", "volatile", "while"
    };
    for (int i = 0; i < 49; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

int isModifier(char *lexeme) {
    const char *modifiers[] = {
        "public", "private", "protected", "static", "final", "abstract", 
        "synchronized", "volatile", "transient", "native"
    };
    for (int i = 0; i < 10; i++) {
        if (strcmp(lexeme, modifiers[i]) == 0) return 1;
    }
    return 0;
}

int isDataType(char *lexeme) {
    const char *types[] = {
        "int", "char", "byte", "short", "long", "float", "double", "boolean", "void", "String"
    };
    for (int i = 0; i < 10; i++) {
        if (strcmp(lexeme, types[i]) == 0) return 1;
    }
    return 0;
}

int isBooleanLiteral(char *lexeme) {
    return (strcmp(lexeme, "true") == 0 || strcmp(lexeme, "false") == 0);
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

        // Handle comments
        if (!inStringLiteral && c == '/') {
            char peek = fgetc(fp);
            
            if (peek == '/') { // Single-line comment
                t.type = COMMENT;
                t.row = *row;
                t.col = *col;
                t.value[0] = '/';
                t.value[1] = '/';
                int valueIndex = 2;
                
                while ((c = fgetc(fp)) != '\n' && c != EOF) {
                    if (valueIndex < 99) {
                        t.value[valueIndex++] = c;
                    }
                }
                t.value[valueIndex] = '\0';
                
                if (c == '\n') {
                    (*row)++;
                    (*col) = 0;
                }
                return t;
                
            } else if (peek == '*') { // Multi-line comment
                t.type = COMMENT;
                t.row = *row;
                t.col = *col;
                t.value[0] = '/';
                t.value[1] = '*';
                int valueIndex = 2;
                
                char prev = '\0';
                while ((c = fgetc(fp)) != EOF) {
                    if (valueIndex < 99) {
                        t.value[valueIndex++] = c;
                    }
                    
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    
                    if (c == '\n') {
                        (*row)++;
                        (*col) = 0;
                    } else {
                        (*col)++;
                    }
                    
                    prev = c;
                }
                t.value[valueIndex] = '\0';
                return t;
                
            } else { // Just a division operator
                ungetc(peek, fp);
                t.row = *row;
                t.col = *col;
                t.type = OPERATOR;
                t.value[0] = c;
                t.value[1] = '\0';
                return t;
            }
        }

        // Handle string literals
        if (c == '"' && !inComment) {
            inStringLiteral = !inStringLiteral;
            t.type = STRING_LITERAL;
            t.row = *row;
            t.col = *col;
            t.value[0] = c;
            int valueIndex = 1;
            
            while ((c = fgetc(fp)) != '"' && c != EOF) {
                if (c == '\\') { // Handle escape sequences
                    if (valueIndex < 98) {
                        t.value[valueIndex++] = c;
                    }
                    c = fgetc(fp); // Get the escaped character
                }
                
                if (valueIndex < 99) {
                    t.value[valueIndex++] = c;
                }
                
                if (c == '\n') {
                    (*row)++;
                    (*col) = 0;
                } else {
                    (*col)++;
                }
            }
            
            if (c == '"') {
                if (valueIndex < 99) {
                    t.value[valueIndex++] = c;
                }
                (*col)++;
            }
            
            t.value[valueIndex] = '\0';
            return t;
        }

        // Handle char literals
        if (c == '\'') {
            t.type = STRING_LITERAL; // Using the same type for simplicity
            t.row = *row;
            t.col = *col;
            t.value[0] = c;
            int valueIndex = 1;
            
            // Handle escape sequences
            c = fgetc(fp);
            if (c == '\\') {
                t.value[valueIndex++] = c;
                c = fgetc(fp);
            }
            t.value[valueIndex++] = c;
            (*col)++;
            
            c = fgetc(fp);
            if (c == '\'') {
                t.value[valueIndex++] = c;
                (*col)++;
            }
            
            t.value[valueIndex] = '\0';
            return t;
        }

        // Handle keywords, identifiers, and boolean literals
        if (isalpha(c) || c == '_') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_' || c == '.') {
                if (lexemeIndex < 99) {
                    lexeme[lexemeIndex++] = c;
                }
                (*col)++;
            }
            lexeme[lexemeIndex] = '\0';
            ungetc(c, fp);
            
            t.row = *row;
            t.col = *col - lexemeIndex + 1;
            
            if (isKeyword(lexeme)) {
                t.type = KEYWORD;
            } else if (isModifier(lexeme)) {
                t.type = MODIFIER;
            } else if (isBooleanLiteral(lexeme)) {
                t.type = BOOLEAN_LITERAL;
            } else {
                t.type = IDENTIFIER;
            }
            
            strcpy(t.value, lexeme);
            return t;
        }

        // Handle numerical constants
        if (isdigit(c) || (c == '.' && isdigit(fgetc(fp)) && ungetc(c, fp))) {
            lexeme[lexemeIndex++] = c;
            int hasDecimal = (c == '.');
            int hasExponent = 0;
            
            while (1) {
                c = fgetc(fp);
                
                if (isdigit(c)) {
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                } else if (c == '.' && !hasDecimal) {
                    hasDecimal = 1;
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                } else if ((c == 'e' || c == 'E') && !hasExponent) {
                    hasExponent = 1;
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                    
                    // Handle sign in exponent
                    c = fgetc(fp);
                    if (c == '+' || c == '-') {
                        if (lexemeIndex < 99) {
                            lexeme[lexemeIndex++] = c;
                        }
                    } else {
                        ungetc(c, fp);
                    }
                } else if ((c == 'l' || c == 'L' || c == 'f' || c == 'F' || c == 'd' || c == 'D') && 
                           lexemeIndex > 0 && isdigit(lexeme[lexemeIndex-1])) {
                    // Suffix for long, float, or double
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                    break;
                } else {
                    ungetc(c, fp);
                    break;
                }
                
                (*col)++;
            }
            
            lexeme[lexemeIndex] = '\0';
            t.row = *row;
            t.col = *col - lexemeIndex + 1;
            t.type = NUMERIC_CONSTANT;
            strcpy(t.value, lexeme);
            return t;
        }

        // Handle operators and special symbols
        if (strchr("+-*/=<>!&|^%.,;(){}[]?:", c)) {
            t.row = *row;
            t.col = *col;
            t.type = OPERATOR;
            t.value[0] = c;
            
            // Handle multi-character operators
            char peek = fgetc(fp);
            if ((c == '+' && (peek == '+' || peek == '=')) ||
                (c == '-' && (peek == '-' || peek == '=' || peek == '>')) ||
                (c == '*' && peek == '=') ||
                (c == '/' && peek == '=') ||
                (c == '=' && peek == '=') ||
                (c == '!' && peek == '=') ||
                (c == '>' && (peek == '=' || peek == '>')) ||
                (c == '<' && (peek == '=' || peek == '<')) ||
                (c == '&' && (peek == '&' || peek == '=')) ||
                (c == '|' && (peek == '|' || peek == '=')) ||
                (c == '^' && peek == '=') ||
                (c == '%' && peek == '=')) {
                
                t.value[1] = peek;
                (*col)++;
                
                // Handle triple-character operators (>>=, <<=, >>>)
                if ((c == '>' && peek == '>' && (peek = fgetc(fp)) == '>') ||
                    ((c == '>' && peek == '>' || c == '<' && peek == '<') && (peek = fgetc(fp)) == '=')) {
                    t.value[2] = peek;
                    t.value[3] = '\0';
                    (*col)++;
                } else {
                    t.value[2] = '\0';
                    if (peek != EOF) ungetc(peek, fp);
                }
            } else {
                t.value[1] = '\0';
                if (peek != EOF) ungetc(peek, fp);
            }
            
            return t;
        }
    }

    // End of file reached
    t.row = *row;
    t.col = *col;
    strcpy(t.value, "EOF");
    return t;
}

void insertToken(char *name, char *type, int size, char *modifier, int isArray) {
    // Check if token already exists
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            // Update existing entry if needed
            if (strcmp(type, "") != 0) {
                strcpy(symbolTable[i].type, type);
            }
            if (size > 0) {
                symbolTable[i].size = size;
            }
            if (strcmp(modifier, "") != 0) {
                strcpy(symbolTable[i].modifier, modifier);
            }
            symbolTable[i].isArray = isArray;
            return;
        }
    }
    
    // Add new entry
    if (symbolTableSize < 100) {
        symbolTable[symbolTableSize].index = symbolTableSize + 1;
        strcpy(symbolTable[symbolTableSize].name, name);
        strcpy(symbolTable[symbolTableSize].type, type);
        symbolTable[symbolTableSize].size = size;
        strcpy(symbolTable[symbolTableSize].modifier, modifier);
        symbolTable[symbolTableSize].isArray = isArray;
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\n--- Java Symbol Table ---\n");
    printf("Index\tName\t\tType\t\tSize\tModifier\tArray\n");
    printf("-----\t----\t\t----\t\t----\t--------\t-----\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%d\t%-15s\t%-15s\t%d\t%-10s\t%s\n", 
               symbolTable[i].index, 
               symbolTable[i].name, 
               symbolTable[i].type, 
               symbolTable[i].size,
               symbolTable[i].modifier,
               symbolTable[i].isArray ? "Yes" : "No");
    }
}

int getSizeForType(const char *type) {
    if (strcmp(type, "byte") == 0) return 1;
    if (strcmp(type, "short") == 0) return 2;
    if (strcmp(type, "int") == 0) return 4;
    if (strcmp(type, "long") == 0) return 8;
    if (strcmp(type, "float") == 0) return 4;
    if (strcmp(type, "double") == 0) return 8;
    if (strcmp(type, "char") == 0) return 2;
    if (strcmp(type, "boolean") == 0) return 1;
    if (strcmp(type, "String") == 0) return 8; // Reference size
    if (strstr(type, "[]") != NULL) return 8;  // Array reference
    return 4; // Default size for unknown types (reference)
}

int main() {
    FILE *fp = fopen("sample.java", "r");
    if (!fp) {
        printf("Error: Could not open sample.java\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    char currentType[50] = "";
    char currentModifier[50] = "";
    int inClassDeclaration = 0;
    int inMethodDeclaration = 0;
    char currentClassName[100] = "";
    char currentMethodName[100] = "";
    
    printf("Starting Java lexical analysis...\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        if (t.type == COMMENT) {
            continue;  // Skip comments
        }
        
        // Track package and imports
        if (t.type == KEYWORD && strcmp(t.value, "package") == 0) {
            Token packageName = getNextToken(fp, &row, &col);
            if (packageName.type == IDENTIFIER) {
                insertToken(packageName.value, "package", 0, "", 0);
            }
            continue;
        }
        
        if (t.type == KEYWORD && strcmp(t.value, "import") == 0) {
            Token importName = getNextToken(fp, &row, &col);
            if (importName.type == IDENTIFIER) {
                insertToken(importName.value, "import", 0, "", 0);
            }
            continue;
        }
        
        // Track class declarations
        if (t.type == KEYWORD && strcmp(t.value, "class") == 0) {
            Token className = getNextToken(fp, &row, &col);
            if (className.type == IDENTIFIER) {
                strcpy(currentClassName, className.value);
                insertToken(className.value, "class", 0, currentModifier, 0);
                inClassDeclaration = 1;
            }
            strcpy(currentModifier, ""); // Reset modifier
            continue;
        }
        
        // Track interface declarations
        if (t.type == KEYWORD && strcmp(t.value, "interface") == 0) {
            Token interfaceName = getNextToken(fp, &row, &col);
            if (interfaceName.type == IDENTIFIER) {
                insertToken(interfaceName.value, "interface", 0, currentModifier, 0);
            }
            strcpy(currentModifier, ""); // Reset modifier
            continue;
        }
        
        // Track access modifiers
        if (t.type == MODIFIER) {
            strcat(currentModifier, t.value);
            strcat(currentModifier, " ");
            continue;
        }
        
        // Track data types for variable declarations
        if (t.type == KEYWORD && isDataType(t.value)) {
            strcpy(currentType, t.value);
            
            // Check for array declaration
            Token next = getNextToken(fp, &row, &col);
            int isArray = 0;
            
            if (next.type == OPERATOR && strcmp(next.value, "[") == 0) {
                // Skip the closing bracket
                Token closeBracket = getNextToken(fp, &row, &col);
                isArray = 1;
                strcat(currentType, "[]");
                next = getNextToken(fp, &row, &col);
            }
            
            if (next.type == IDENTIFIER) {
                int size = getSizeForType(currentType);
                insertToken(next.value, currentType, size, currentModifier, isArray);
            }
            
            strcpy(currentModifier, ""); // Reset modifier
            continue;
        }
        
        // Track method declarations
        if (inClassDeclaration && isDataType(currentType) && t.type == IDENTIFIER) {
            // Check if next token is opening parenthesis (method declaration)
            Token next = getNextToken(fp, &row, &col);
            if (next.type == OPERATOR && strcmp(next.value, "(") == 0) {
                strcpy(currentMethodName, t.value);
                char methodSignature[200] = "";
                sprintf(methodSignature, "%s %s", currentType, t.value);
                insertToken(t.value, "method", 0, currentModifier, 0);
                inMethodDeclaration = 1;
            }
            
            strcpy(currentType, ""); // Reset type
            strcpy(currentModifier, ""); // Reset modifier
        }
    }

    displaySymbolTable();
    fclose(fp);
    printf("\nJava lexical analysis completed.\n");
    return 0;
}
