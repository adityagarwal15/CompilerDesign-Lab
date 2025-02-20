#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Define token types
#define KEYWORD 1
#define IDENTIFIER 2
#define OPERATOR 3
#define SPECIAL_OP 4
#define DOLLAR_ID 5
#define NUMERIC 6
#define COMMENT 7
#define SHEBANG 8
#define DELIMITER 9

typedef struct {
    int type;
    char value[100];
} Token;

// Function to convert token type to string for display
const char* getTokenName(int type) {
    switch(type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case OPERATOR: return "OPERATOR";
        case SPECIAL_OP: return "SPECIAL_OP";
        case DOLLAR_ID: return "DOLLAR_ID";
        case NUMERIC: return "NUMERIC";
        case COMMENT: return "COMMENT";
        case SHEBANG: return "SHEBANG";
        case DELIMITER: return "DELIMITER";
        default: return "UNKNOWN";
    }
}

int isKeyword(const char* str) {
    return (strcmp(str, "scalar") == 0 || 
            strcmp(str, "foreach") == 0);
}

Token getNextToken(FILE* fp) {
    Token token = {0, ""};
    static int in_first_line = 1;
    char c;
    
    // Skip whitespace
    while ((c = fgetc(fp)) != EOF && isspace(c));
    
    if (c == EOF) {
        strcpy(token.value, "EOF");
        return token;
    }

    // Handle shebang (#!) in first line
    if (in_first_line && c == '#') {
        char next = fgetc(fp);
        if (next == '!') {
            token.type = SHEBANG;
            token.value[0] = '#';
            token.value[1] = '!';
            int i = 2;
            // Read until end of line
            while ((c = fgetc(fp)) != EOF && c != '\n') {
                token.value[i++] = c;
            }
            token.value[i] = '\0';
            in_first_line = 0;
            return token;
        }
        ungetc(next, fp);
    }
    in_first_line = 0;

    // Handle comments (#)
    if (c == '#') {
        char next = fgetc(fp);
        if (next != '!') { // Regular comment
            token.type = COMMENT;
            token.value[0] = '#';
            int i = 1;
            while ((c = fgetc(fp)) != EOF && c != '\n') {
                token.value[i++] = c;
            }
            token.value[i] = '\0';
            return token;
        }
        ungetc(next, fp);
    }

    // Handle dollar identifiers ($n, $sum, etc.)
    if (c == '$') {
        token.type = DOLLAR_ID;
        token.value[0] = c;
        int i = 1;
        
        c = fgetc(fp);
        if (isalpha(c) || c == '_') {
            token.value[i++] = c;
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                token.value[i++] = c;
            }
            ungetc(c, fp);
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle special operators (@_, +=)
    if (c == '@' || c == '+') {
        char next = fgetc(fp);
        if ((c == '@' && next == '_') || 
            (c == '+' && next == '=')) {
            token.type = SPECIAL_OP;
            token.value[0] = c;
            token.value[1] = next;
            token.value[2] = '\0';
            return token;
        }
        ungetc(next, fp);
    }

    // Handle identifiers and keywords
    if (isalpha(c) || c == '_') {
        int i = 0;
        token.value[i++] = c;
        while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
            token.value[i++] = c;
        }
        ungetc(c, fp);
        token.value[i] = '\0';
        
        token.type = isKeyword(token.value) ? KEYWORD : IDENTIFIER;
        return token;
    }

    // Handle numeric values
    if (isdigit(c)) {
        token.type = NUMERIC;
        int i = 0;
        token.value[i++] = c;
        while ((c = fgetc(fp)) != EOF && isdigit(c)) {
            token.value[i++] = c;
        }
        ungetc(c, fp);
        token.value[i] = '\0';
        return token;
    }

    // Handle delimiters
    if (strchr("(){};", c)) {
        token.type = DELIMITER;
        token.value[0] = c;
        token.value[1] = '\0';
        return token;
    }

    // Handle remaining operators
    if (strchr("=+-*/", c)) {
        token.type = OPERATOR;
        token.value[0] = c;
        token.value[1] = '\0';
        return token;
    }

    return token;
}

void printToken(Token token) {
    printf("<%s, %s>\n", getTokenName(token.type), token.value);
}

int main() {
    FILE* fp;
    char filename[100];
    Token token;
    int tokenCount = 0;

    printf("Enter the Perl script filename: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Unable to open file %s\n", filename);
        return 1;
    }

    printf("\nLexical Analysis Results:\n");
    printf("=========================\n\n");
    printf("Token Stream:\n");
    printf("-------------\n");

    while ((token = getNextToken(fp)).value[0] != '\0') {
        if (strcmp(token.value, "EOF") == 0) break;
        
        printToken(token);
        tokenCount++;
    }

    printf("\nSummary:\n");
    printf("--------\n");
    printf("Total tokens found: %d\n", tokenCount);

    fclose(fp);
    return 0;
}
