#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1024
#define MAX_SYMBOLS 256

// ------------------- DATA STRUCTURES -------------------

struct token {
    char lexeme[64];
    int row, col;
    char type[30];
};

struct symbol {
    int index;
    char name[64];
    char type[30];
    int size;
};

struct token tokenList[MAX_TOKENS];
int tokenCount = 0;     // number of valid tokens
int currentTokenIndex = 0; // parser's current index into tokenList

int row = 1, col = 1;   // used during lexical analysis

void Program();
void Declarations();
void DataType();
void IdentifierList();
void AssignStat();

void StatementList();
void Statement();
void DecisionStat();
void Dprime();
void Expn();
void Eprime();
void SimpleExpn();
void Seprime();
void Term();
void Tprime();
void Factor();


void parse_error(const char *expected, struct token t);
void match(const char *expectedLexeme);
void matchType(const char *expectedType);

struct symbol symTable[MAX_SYMBOLS];
int symCount = 0;

// ------------------- LEXICAL ANALYZER ------------------- (Unchanges portion)

struct token getNextToken(FILE *fin) {
    struct token t;
    t.row = -1;  
    char c;
    while ((c = getc(fin)) != EOF) {
        if (c == '\n') {
            row++;
            col = 1;
            continue;
        }
        if (isspace(c)) {
            col++;
            continue;
        }
        if (c == '/') {
            int next = getc(fin);
            if (next == '/') {
                // Single-line comment
                while ((c = getc(fin)) != EOF && c != '\n') col++;
                if (c == '\n') {
                    row++;
                    col = 1;
                }
                continue;
            } else if (next == '*') {
                // Multi-line comment
                int prev = 0;
                while ((c = getc(fin)) != EOF) {
                    col++;
                    if (c == '\n') {
                        row++;
                        col = 1;
                    }
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            } else {
                ungetc(next, fin);
                t.row = row; 
                t.col = col;
                t.lexeme[0] = '/'; 
                t.lexeme[1] = '\0';
                strcpy(t.type, "Operator");
                col++;
                return t;
            }
        }

        char specialSymbols[] = {'#', '?', ';', ':', ',', '(', ')', '{', '}', '.'};
        int isSpecial = 0;
        for (int i = 0; i < (int)(sizeof(specialSymbols)/sizeof(char)); i++) {
            if (c == specialSymbols[i]) {
                isSpecial = 1; 
                break;
            }
        }
        if (isSpecial) {
            t.row = row; 
            t.col = col;
            t.lexeme[0] = c; 
            t.lexeme[1] = '\0';
            strcpy(t.type, "Special");
            col++;
            return t;
        }

        if (c == '-' || c == '+' || c == '*' || c == '%') {
            int startCol = col, startRow = row;
            char op[3] = {c, '\0', '\0'};
            int next = getc(fin);
            if (next == '=' || ((c == '+' || c == '-') && next == c)) {
                op[1] = next; 
                op[2] = '\0';
                col += 2;
            } else {
                ungetc(next, fin);
                col++;
            }
            t.row = startRow; 
            t.col = startCol;
            strcpy(t.lexeme, op);
            strcpy(t.type, "Operator");
            return t;
        }

        if (c == '=' || c == '<' || c == '>' || c == '!') {
            int startCol = col, startRow = row;
            char op[3] = {c, '\0', '\0'};
            int next = getc(fin);
            if (next == '=') {
                op[1] = '='; 
                op[2] = '\0';
                col += 2;
            } else {
                ungetc(next, fin);
                col++;
            }
            t.row = startRow; 
            t.col = startCol;
            strcpy(t.lexeme, op);
            strcpy(t.type, "Operator");
            return t;
        }

        if (isdigit(c)) {
            int startCol = col, startRow = row;
            char num[64];
            int i = 0;
            num[i++] = c;
            col++;
            while ((c = getc(fin)) != EOF && isdigit(c)) {
                num[i++] = c;
                col++;
            }
            num[i] = '\0';
            if (c != EOF) {
                ungetc(c, fin);
                col--;
            }
            t.row = startRow; 
            t.col = startCol;
            strcpy(t.lexeme, num);
            strcpy(t.type, "Number");
            return t;
        }

        if (isalpha(c) || c == '_') {
            int startCol = col, startRow = row;
            char ident[64];
            int i = 0;
            ident[i++] = c;
            col++;
            while ((c = getc(fin)) != EOF && (isalnum(c) || c == '_')) {
                ident[i++] = c;
                col++;
            }
            ident[i] = '\0';
            if (c != EOF) {
                ungetc(c, fin);
                col--;
            }
            t.row = startRow; 
            t.col = startCol;
            strcpy(t.lexeme, ident);
            const char *keywords[] = {
                "for", "if", "else", "while", "do", "break", "continue",
                "return", "int", "double", "float", "char", "long", "short",
                "sizeof", "typedef", "switch", "case", "struct", "const",
                "void", "exit"
            };
            int isKeyword = 0;
            for (int j = 0; j < (int)(sizeof(keywords)/sizeof(keywords[0])); j++) {
                if (strcmp(ident, keywords[j]) == 0) {
                    isKeyword = 1;
                    break;
                }
            }
            if (isKeyword) strcpy(t.type, "Keyword");
            else strcpy(t.type, "Identifier");
            return t;
        }

        if (c == '"') {
            int startCol = col, startRow = row;
            char strLit[64];
            int i = 0;
            strLit[i++] = c;
            col++;
            while ((c = getc(fin)) != EOF && c != '"') {
                strLit[i++] = c;
                col++;
            }
            if (c == '"') {
                strLit[i++] = c;
                col++;
            }
            strLit[i] = '\0';
            t.row = startRow; 
            t.col = startCol;
            strcpy(t.lexeme, strLit);
            strcpy(t.type, "StringLiteral");
            return t;
        }

        col++;
    }
    t.row = -1;
    return t;
}


void parse_error(const char *expected, struct token t) {
    fprintf(stderr, 
            "Error at row %d, col %d: expected '%s' but found '%s'\n",
            t.row, t.col, expected, t.lexeme);
    exit(1);
}

void match(const char *expectedLexeme) {
    if (currentTokenIndex >= tokenCount) {
        fprintf(stderr, "Error: unexpected end of input while expecting '%s'\n", expectedLexeme);
        exit(1);
    }
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.lexeme, expectedLexeme) == 0) {
        currentTokenIndex++;
    } else {
        parse_error(expectedLexeme, t);
    }
}

void matchType(const char *expectedType) {
    if (currentTokenIndex >= tokenCount) {
        fprintf(stderr, "Error: unexpected end of input while expecting type '%s'\n", expectedType);
        exit(1);
    }
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.type, expectedType) == 0) {
        currentTokenIndex++;
    } else {
        char msg[100];
        sprintf(msg, "token of type '%s'", expectedType);
        parse_error(msg, t);
    }
}

void Program() {
    // Program -> main() { declarations statement_list }
    match("main");
    match("(");
    match(")");
    match("{");
    Declarations();
    StatementList();  
    match("}");
}


void Declarations() {
    if (currentTokenIndex >= tokenCount) return;

    struct token t = tokenList[currentTokenIndex];

    if ((strcmp(t.lexeme, "int") == 0) || (strcmp(t.lexeme, "char") == 0)) {
        
        DataType();
        IdentifierList();
        match(";");
        Declarations();
    }
    // else epsilon
}

void DataType() {
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.lexeme, "int") == 0) {
        match("int");
    } else if (strcmp(t.lexeme, "char") == 0) {
        match("char");
    } else {
        // error
        parse_error("data-type (int or char)", t);
    }
}

void IdentifierList() {

    matchType("Identifier");

    if (currentTokenIndex < tokenCount && 
        strcmp(tokenList[currentTokenIndex].lexeme, "[") == 0) 
    {
        match("[");
        matchType("Number");
        match("]");
    }

    if (currentTokenIndex < tokenCount && 
        strcmp(tokenList[currentTokenIndex].lexeme, ",") == 0) 
    {
        match(",");
        IdentifierList();
    }
}


void AssignStat() {
    matchType("Identifier");  
    match("=");

    if (currentTokenIndex >= tokenCount) {
        fprintf(stderr, "Error: unexpected end of input in assign_stat\n");
        exit(1);
    }
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.type, "Identifier") == 0) {
        matchType("Identifier");
    } else if (strcmp(t.type, "Number") == 0) {
        matchType("Number");
    } else {
        parse_error("Identifier or Number", t);
    }

    match(";");
}



// statement_list -> statement statement_list | epsilon
void StatementList() {
    if (currentTokenIndex >= tokenCount) {
        // No more tokens -> epsilon
        return;
    }

    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.type, "Identifier") == 0 || strcmp(t.lexeme, "if") == 0) {
        Statement();
        StatementList();  // parse more statements
    }
    // else epsilon
}

// statement -> assign_stat | decision_stat
void Statement() {
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.type, "Identifier") == 0) {
        AssignStat();
    } 
    else if (strcmp(t.lexeme, "if") == 0) {
        DecisionStat();
    }
    else {
        parse_error("Identifier or 'if'", t);
    }
}

// decision_stat -> if ( expn ) { statement_list } dprime
void DecisionStat() {
    match("if");
    match("(");
    Expn();
    match(")");
    match("{");
    StatementList();
    match("}");
    Dprime();
}

// dprime -> else { statement_list } | epsilon
void Dprime() {
    if (currentTokenIndex < tokenCount 
        && strcmp(tokenList[currentTokenIndex].lexeme, "else") == 0) 
    {
        match("else");
        match("{");
        StatementList();
        match("}");
    }
    // else epsilon
}

// expn -> simple-expn eprime
void Expn() {
    SimpleExpn();
    Eprime();
}

// eprime -> relop simple-expn | epsilon
// relop -> == | != | <= | >= | > | <
void Eprime() {
    if (currentTokenIndex >= tokenCount) return;

    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.lexeme, "==") == 0 || strcmp(t.lexeme, "!=") == 0 ||
        strcmp(t.lexeme, "<=") == 0 || strcmp(t.lexeme, ">=") == 0 ||
        strcmp(t.lexeme, "<")  == 0 || strcmp(t.lexeme, ">")  == 0) 
    {
        match(t.lexeme);   // match the relop
        SimpleExpn();
    }
    // else epsilon
}

// simple-expn -> term seprime
void SimpleExpn() {
    Term();
    Seprime();
}

// seprime -> addop term seprime | epsilon
// addop -> + | -
void Seprime() {
    if (currentTokenIndex >= tokenCount) return;

    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.lexeme, "+") == 0 || strcmp(t.lexeme, "-") == 0) {
        match(t.lexeme); // match the addop
        Term();
        Seprime();
    }
    // else epsilon
}

// term -> factor tprime
void Term() {
    Factor();
    Tprime();
}

// tprime -> mulop factor tprime | epsilon
// mulop -> * | / | %
void Tprime() {
    if (currentTokenIndex >= tokenCount) return;

    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.lexeme, "*") == 0 || 
        strcmp(t.lexeme, "/") == 0 || 
        strcmp(t.lexeme, "%") == 0) 
    {
        match(t.lexeme);  // match the mulop
        Factor();
        Tprime();
    }
    // else epsilon
}

// factor -> id | num
void Factor() {
    struct token t = tokenList[currentTokenIndex];
    if (strcmp(t.type, "Identifier") == 0) {
        matchType("Identifier");
    } 
    else if (strcmp(t.type, "Number") == 0) {
        matchType("Number");
    } 
    else {
        parse_error("Identifier or Number", t);
    }
}





int main() {
    FILE *fin = fopen("q1.c", "r");
    if (!fin) {
        printf("Error opening file.\n");
        return 1;
    }

    struct token t;
    while ((t = getNextToken(fin)).row != -1) {
        
        tokenList[tokenCount++] = t;
        if (tokenCount >= MAX_TOKENS) {
            fprintf(stderr, "Too many tokens.\n");
            exit(1);
        }
    }
    fclose(fin);

    Program();

    if (currentTokenIndex < tokenCount) {
        struct token extra = tokenList[currentTokenIndex];
        fprintf(stderr, 
                "Error: extra tokens after valid Program at row %d, col %d: '%s'\n",
                extra.row, extra.col, extra.lexeme);
        return 1;
    }

    printf("Parsing successful!\n");
    return 0;
}
