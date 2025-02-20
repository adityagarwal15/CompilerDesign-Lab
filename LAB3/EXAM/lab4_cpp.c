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
    char modifier[50]; // For C++ access/storage modifiers
    int isPointer;     // To track pointers
    int isReference;   // To track references
    char scope[100];   // For namespace/class scope
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;

int isKeyword(char *lexeme) {
    // C++ keywords
    const char *keywords[] = {
        "auto", "break", "case", "catch", "char", "class", "const", "continue",
        "default", "delete", "do", "double", "else", "enum", "explicit", "export",
        "extern", "false", "float", "for", "friend", "goto", "if", "inline",
        "int", "long", "mutable", "namespace", "new", "operator", "private",
        "protected", "public", "register", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "template", "this", "throw", "true", "try",
        "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual",
        "void", "volatile", "wchar_t", "while", "bool", "const_cast", "dynamic_cast",
        "reinterpret_cast", "static_cast", "nullptr"
    };
    
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) return 1;
    }
    return 0;
}

int isModifier(char *lexeme) {
    const char *modifiers[] = {
        "public", "private", "protected", "static", "const", "virtual", 
        "explicit", "extern", "inline", "register", "volatile", "mutable", "friend"
    };
    
    int num_modifiers = sizeof(modifiers) / sizeof(modifiers[0]);
    for (int i = 0; i < num_modifiers; i++) {
        if (strcmp(lexeme, modifiers[i]) == 0) return 1;
    }
    return 0;
}

int isDataType(char *lexeme) {
    const char *types[] = {
        "void", "char", "short", "int", "long", "float", "double", "bool", 
        "wchar_t", "auto", "signed", "unsigned"
    };
    
    int num_types = sizeof(types) / sizeof(types[0]);
    for (int i = 0; i < num_types; i++) {
        if (strcmp(lexeme, types[i]) == 0) return 1;
    }
    return 0;
}

int isBooleanLiteral(char *lexeme) {
    return (strcmp(lexeme, "true") == 0 || strcmp(lexeme, "false") == 0 || 
            strcmp(lexeme, "TRUE") == 0 || strcmp(lexeme, "FALSE") == 0);
}

Token getNextToken(FILE *fp, int *row, int *col) {
    Token t = {-1, -1, 0, ""};
    char c, lexeme[100];
    int lexemeIndex = 0, inStringLiteral = 0, inCharLiteral = 0, inComment = 0;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;
        if (isspace(c)) {
            if (c == '\n') { (*row)++; *col = 0; }
            continue;
        }

        // Handle preprocessor directives
        if (c == '#' && *col == 1) {
            t.type = PREPROCESSOR;
            t.row = *row;
            t.col = *col;
            t.value[0] = c;
            int valueIndex = 1;
            
            // Get the rest of the preprocessor line
            while ((c = fgetc(fp)) != '\n' && c != EOF) {
                // Handle line continuation with backslash
                if (c == '\\') {
                    if (valueIndex < 98) {
                        t.value[valueIndex++] = c;
                    }
                    c = fgetc(fp);
                    if (c == '\n') {
                        (*row)++;
                        (*col) = 0;
                        continue;
                    } else {
                        ungetc(c, fp);
                    }
                } else if (valueIndex < 99) {
                    t.value[valueIndex++] = c;
                }
            }
            
            if (c == '\n') {
                (*row)++;
                (*col) = 0;
            }
            
            t.value[valueIndex] = '\0';
            return t;
        }

        // Handle comments
        if (!inStringLiteral && !inCharLiteral && c == '/') {
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
        if (c == '"' && !inCharLiteral && !inComment) {
            inStringLiteral = !inStringLiteral;
            t.type = STRING_LITERAL;
            t.row = *row;
            t.col = *col;
            t.value[0] = c;
            int valueIndex = 1;
            
            while (1) {
                c = fgetc(fp);
                
                if (c == EOF || (c == '"' && t.value[valueIndex-1] != '\\')) {
                    break;
                }
                
                // Handle escape sequences
                if (c == '\\') {
                    if (valueIndex < 98) {
                        t.value[valueIndex++] = c;
                    }
                    c = fgetc(fp);
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
        if (c == '\'' && !inStringLiteral && !inComment) {
            inCharLiteral = !inCharLiteral;
            t.type = STRING_LITERAL; // Using the same type for simplicity
            t.row = *row;
            t.col = *col;
            t.value[0] = c;
            int valueIndex = 1;
            
            while (1) {
                c = fgetc(fp);
                
                if (c == EOF || (c == '\'' && t.value[valueIndex-1] != '\\')) {
                    break;
                }
                
                // Handle escape sequences
                if (c == '\\') {
                    if (valueIndex < 98) {
                        t.value[valueIndex++] = c;
                    }
                    c = fgetc(fp);
                }
                
                if (valueIndex < 99) {
                    t.value[valueIndex++] = c;
                }
                
                (*col)++;
            }
            
            if (c == '\'') {
                if (valueIndex < 99) {
                    t.value[valueIndex++] = c;
                }
                (*col)++;
            }
            
            t.value[valueIndex] = '\0';
            return t;
        }

        // Handle keywords, identifiers, and boolean literals
        if (isalpha(c) || c == '_') {
            lexeme[lexemeIndex++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
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

        // Handle numerical constants (with C++ specific features)
        if (isdigit(c) || (c == '.' && isdigit(fgetc(fp)) && ungetc(c, fp))) {
            lexeme[lexemeIndex++] = c;
            int hasDecimal = (c == '.');
            int hasExponent = 0;
            int hexadecimal = 0;
            int binary = 0;
            
            // Check for hex/binary prefix
            if (c == '0') {
                char peek = fgetc(fp);
                if (peek == 'x' || peek == 'X') {
                    hexadecimal = 1;
                    lexeme[lexemeIndex++] = peek;
                    (*col)++;
                } else if (peek == 'b' || peek == 'B') {
                    binary = 1;
                    lexeme[lexemeIndex++] = peek;
                    (*col)++;
                } else {
                    ungetc(peek, fp);
                }
            }
            
            while (1) {
                c = fgetc(fp);
                
                if (isdigit(c) || 
                    (hexadecimal && ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) ||
                    (binary && (c == '0' || c == '1'))) {
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                } else if (c == '.' && !hasDecimal && !hexadecimal && !binary) {
                    hasDecimal = 1;
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                    }
                } else if ((c == 'e' || c == 'E') && !hasExponent && !hexadecimal && !binary) {
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
                } else if ((c == 'l' || c == 'L' || c == 'u' || c == 'U' || 
                           c == 'f' || c == 'F' || c == 'd' || c == 'D') && 
                           lexemeIndex > 0) {
                    // Suffix for long, unsigned, float, or double
                    if (lexemeIndex < 99) {
                        lexeme[lexemeIndex++] = c;
                        
                        // Check for combined suffixes (UL, LL)
                        char peek = fgetc(fp);
                        if (((c == 'u' || c == 'U') && (peek == 'l' || peek == 'L')) ||
                            ((c == 'l' || c == 'L') && (peek == 'l' || peek == 'L'))) {
                            if (lexemeIndex < 99) {
                                lexeme[lexemeIndex++] = peek;
                            }
                            (*col)++;
                        } else {
                            ungetc(peek, fp);
                        }
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

        // Handle operators and special symbols with C++ specific operators
        if (strchr("+-*/=<>!&|^%.,;:(){}[]?~", c)) {
            t.row = *row;
            t.col = *col;
            t.type = OPERATOR;
            t.value[0] = c;
            
            // Handle multi-character operators
            char peek = fgetc(fp);
            
            // C++ scope resolution operator
            if (c == ':' && peek == ':') {
                t.value[1] = peek;
                t.value[2] = '\0';
                (*col)++;
                return t;
            }
            
            // Other multi-character operators
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
                
                // Check for compound assignment operators with shift operators
                if ((c == '>' && peek == '>' || c == '<' && peek == '<') && 
                    (peek = fgetc(fp)) == '=') {
                    t.value[2] = peek;
                    t.value[3] = '\0';
                    (*col) += 2;
                } else {
                    t.value[2] = '\0';
                    (*col)++;
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

void insertToken(char *name, char *type, int size, char *modifier, int isPointer, int isReference, char *scope) {
    // Handle fully qualified names (with scope resolution operator)
    char localName[100];
    strcpy(localName, name);
    
    // Check if token already exists
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i].name, localName) == 0 && 
            strcmp(symbolTable[i].scope, scope) == 0) {
            // Update existing entry
            if (strcmp(type, "") != 0) {
                strcpy(symbolTable[i].type, type);
            }
            if (size > 0) {
                symbolTable[i].size = size;
            }
            if (strcmp(modifier, "") != 0) {
                strcpy(symbolTable[i].modifier, modifier);
            }
            if (isPointer) {
                symbolTable[i].isPointer = isPointer;
            }
            if (isReference) {
                symbolTable[i].isReference = isReference;
            }
            return;
        }
    }
    
    // Add new entry
    if (symbolTableSize < 100) {
        symbolTable[symbolTableSize].index = symbolTableSize + 1;
        strcpy(symbolTable[symbolTableSize].name, localName);
        strcpy(symbolTable[symbolTableSize].type, type);
        symbolTable[symbolTableSize].size = size;
        strcpy(symbolTable[symbolTableSize].modifier, modifier);
        symbolTable[symbolTableSize].isPointer = isPointer;
        symbolTable[symbolTableSize].isReference = isReference;
        strcpy(symbolTable[symbolTableSize].scope, scope);
        symbolTableSize++;
    }
}

void displaySymbolTable() {
    printf("\n--- C++ Symbol Table ---\n");
    printf("%-5s %-20s %-15s %-6s %-12s %-5s %-5s %-15s\n", 
           "Index", "Name", "Type", "Size", "Modifier", "Ptr", "Ref", "Scope");
    printf("%-5s %-20s %-15s %-6s %-12s %-5s %-5s %-15s\n",
           "-----", "--------------------", "---------------", "------", "------------", "-----", "-----", "---------------");
    
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%-5d %-20s %-15s %-6d %-12s %-5s %-5s %-15s\n", 
               symbolTable[i].index, 
               symbolTable[i].name, 
               symbolTable[i].type, 
               symbolTable[i].size,
               symbolTable[i].modifier,
               symbolTable[i].isPointer ? "Yes" : "No",
               symbolTable[i].isReference ? "Yes" : "No",
               symbolTable[i].scope);
    }
}

int getSizeForType(const char *type) {
    if (strcmp(type, "char") == 0) return 1;
    if (strcmp(type, "short") == 0) return 2;
    if (strcmp(type, "int") == 0) return 4;
    if (strcmp(type, "long") == 0) return 4;
    if (strcmp(type, "long long") == 0) return 8;
    if (strcmp(type, "float") == 0) return 4;
    if (strcmp(type, "double") == 0) return 8;
    if (strcmp(type, "bool") == 0) return 1;
    if (strcmp(type, "wchar_t") == 0) return 2;
    if (strcmp(type, "void") == 0) return 0;
    if (strstr(type, "*") != NULL) return 8;  // Pointer size
    if (strstr(type, "&") != NULL) return 8;  // Reference size
    return 4; // Default size
}

int main() {
    FILE *fp = fopen("sample.cpp", "r");
    if (!fp) {
        printf("Error: Could not open sample.cpp\n");
        return 1;
    }

    int row = 1, col = 1;
    Token t;
    char currentType[50] = "";
    char currentModifier[50] = "";
    int inClassDeclaration = 0;
    int inStructDeclaration = 0;
    int inNamespaceDeclaration = 0;
    int inTemplateDeclaration = 0;
    char currentScope[100] = "global";
    
    printf("Starting C++ lexical analysis...\n");

    while ((t = getNextToken(fp, &row, &col)).type != 0) {
        if (t.type == COMMENT || t.type == PREPROCESSOR) {
            continue;  // Skip comments and preprocessor directives
        }
        
        // Track namespace declarations
        if (t.type == KEYWORD && strcmp(t.value, "namespace") == 0) {
            Token namespaceName = getNextToken(fp, &row, &col);
            if (namespaceName.type == IDENTIFIER) {
                char newScope[100];
                strcpy(newScope, namespaceName.value);
                insertToken(namespaceName.value, "namespace", 0, "", 0, 0, currentScope);
                strcpy(currentScope, newScope);
                inNamespaceDeclaration = 1;
            }
            continue;
        }
        
        // Track class declarations
        if (t.type == KEYWORD && strcmp(t.value, "class") == 0) {
            Token className = getNextToken(fp, &row, &col);
            if (className.type == IDENTIFIER) {
                char newScope[100];
                sprintf(newScope, "%s::%s", currentScope, className.value);
                insertToken(className.value, "class", 0, currentModifier, 0, 0, currentScope);
                strcpy(currentScope, newScope);
                inClassDeclaration = 1;
            }
            strcpy(currentModifier, ""); // Reset modifier
            continue;
        }
        
        // Track struct declarations
        if (t.type == KEYWORD && strcmp(t.value, "struct") == 0) {
            Token structName = getNextToken(fp, &row, &col);
            if (structName.type == IDENTIFIER) {
                char newScope[100];
                sprintf(newScope, "%s::%s", currentScope, structName.value);
                insertToken(structName.value, "struct", 0, currentModifier, 0, 0, currentScope);
                strcpy(currentScope, newScope);
                inStructDeclaration = 1;
            }
            strcpy(currentModifier, ""); // Reset modifier
            continue;
        }
        
        // Track template declarations
        if (t.type == KEYWORD && strcmp(t.value, "template") == 0) {
            inTemplateDeclaration = 1;
            // Skip the template parameters
            int bracketCount = 0;
            Token templateToken;
            do {
                templateToken = getNextToken(fp, &row, &col);
                if (templateToken.type == OPERATOR) {
                    if (strcmp(templateToken.value, "<") == 0) bracketCount++;
                    if (strcmp(templateToken.value, ">") == 0) bracketCount--;
                }
            } while (bracketCount > 0 && templateToken.type != 0);
            continue;
        }
        
        // Track access modifiers
        if (t.type == MODIFIER || 
            (t.type == KEYWORD && (strcmp(t.value, "const") == 0 || strcmp(t.value, "static") == 0))) {
            if (strlen(currentModifier) > 0) {
                strcat(currentModifier, " ");
            }
            strcat(currentModifier, t.value);
            continue;
        }
        
        // Track scope changes with braces
        if (t.type == OPERATOR && strcmp(t.value, "{") == 0) {
            // Opening a new scope block
            continue;
        }
        
        if (t.type == OPERATOR && strcmp(t.value, "}") == 0) {
            // Closing a scope block
            if (inClassDeclaration || inStructDeclaration || inNamespaceDeclaration) {
                // Return to parent scope
                char *lastScopeDelimiter = strrchr(currentScope, ':');
                if (lastScopeDelimiter != NULL && lastScopeDelimiter > currentScope + 1) {
                    *(lastScopeDelimiter - 1) = '\0';
                } else {
                    strcpy(currentScope, "global");
                }
                
                inClassDeclaration = 0;
                inStructDeclaration = 0;
                inNamespaceDeclaration = 0;
            }
            continue;
        }
        
        // Track data types for variable declarations
        if ((t.type == KEYWORD && isDataType(t.value)) || 
            (inClassDeclaration && t.type == IDENTIFIER)) {
            
            // Save the current type
            if (t.type == KEYWORD && isDataType(t.value)) {
                strcpy(currentType, t.value);
            }
            
            // Handle modifiers for primitive types
            if (t.type == KEYWORD) {
                // Check for combined types like "unsigned int"
                Token next = getNextToken(fp, &row, &col);
                if (next.type == KEYWORD && isDataType(next.value)) {
                    strcat(currentType, " ");
                    strcat(currentType, next.value);
                    next = getNextToken(fp, &row, &col);
                }
                
                // Now next should be an identifier, pointer/reference indicator, or other token
                if (next.type == IDENTIFIER) {
                    // Basic variable declaration
                    int isPointer = 0;
                    int isReference = 0;
                    
                    // Check for pointers or references
                    Token peek = getNextToken(fp, &row, &col);
                    while (peek.type == OPERATOR && 
                          (strcmp(peek.value, "*") == 0 || strcmp(peek.value, "&") == 0)) {
                        if (strcmp(peek.value, "*") == 0) isPointer = 1;
                        if (strcmp(peek.value, "&") == 0) isReference = 1;
                        peek = getNextToken(fp, &row, &col);
                    }
                    
                    // Add the variable to symbol table
                    int size = getSizeForType(currentType);
                    insertToken(next.value, currentType, size, currentModifier, isPointer, isReference, currentScope);
                    
                    // Skip to the end of this declaration
                    while (peek.type != OPERATOR || strcmp(peek.value, ";") != 0) {
                        if (peek.type == OPERATOR && strcmp(peek.value, ",") == 0) {
                            // Handle multiple declarations
                            Token nextVar = getNextToken(fp, &row, &col);
                            if (nextVar.type == IDENTIFIER) {
                                insertToken(nextVar.value, currentType, size, currentModifier, isPointer, isReference, currentScope);
                            }
                        }
                        peek = getNextToken(fp, &row, &col);
                        if (peek.type == 0) break; // EOF
                    }
                }
            }
            
            strcpy(currentModifier, ""); // Reset modifier
            strcpy(currentType, "");     // Reset type
            continue;
        }
    }

    displaySymbolTable();
    fclose(fp);
    printf("\nC++ lexical analysis completed.\n");
    return 0;
}
