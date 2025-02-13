#include <stdio.h>
#include <stdlib.h>

int curr = 0;
char str[100];

// Function declarations
void S();
void T();
void invalid();
void valid();
void match(char expected);
void skipSpaces();

// Error handling
void invalid() {
    printf("-----------------ERROR!----------------\n");
    exit(0);
}

void valid() {
    printf("----------------SUCCESS!---------------\n");
    exit(0);
}

// Utility functions
void skipSpaces() {
    while (str[curr] == ' ' || str[curr] == '\t') {
        curr++;
    }
}

void match(char expected) {
    skipSpaces();
    if (str[curr] == expected) {
        curr++;
    } else {
        printf("Expected '%c' but found '%c'\n", expected, str[curr]);
        invalid();
    }
}

// Grammar rules
void S() {
    // S → a | > | ( T )
    skipSpaces();
    
    switch(str[curr]) {
        case 'a':
            match('a');
            break;
        case '>':
            match('>');
            break;
        case '(':
            match('(');
            T();
            match(')');
            break;
        default:
            printf("Invalid symbol in S: '%c'\n", str[curr]);
            invalid();
    }
}

void T() {
    // T → T, S | S
    S();  // First parse S
    
    skipSpaces();
    while (str[curr] == ',') {
        match(',');
        skipSpaces();
        S();
        skipSpaces();
    }
}

int main() {
    printf("Enter String: ");
    fgets(str, 100, stdin);
    
    // Remove newline if present
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
    
    S();  // Start parsing with S
    skipSpaces();
    
    if (str[curr] == '\0') {
        valid();
    } else {
        printf("Unexpected characters after valid expression: '%s'\n", &str[curr]);
        invalid();
    }
    
    return 0;
}
