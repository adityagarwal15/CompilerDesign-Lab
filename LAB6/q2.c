#include <stdio.h>
#include <stdlib.h>

int curr = 0;
char str[100];

// Function declarations
void S();
void U();
void V();
void W();
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

// S → UVW
void S() {
    skipSpaces();
    U();  // Parse U
    V();  // Parse V
    W();  // Parse W
}

// U → (S) | aSb | d
void U() {
    skipSpaces();
    
    switch(str[curr]) {
        case '(':
            match('(');
            S();
            match(')');
            break;
            
        case 'a':
            match('a');
            S();
            match('b');
            break;
            
        case 'd':
            match('d');
            break;
            
        default:
            printf("Invalid symbol in U: '%c'\n", str[curr]);
            invalid();
    }
}

// V → aV | 𝜖
void V() {
    skipSpaces();
    
    // Check if we have 'a', otherwise assume epsilon
    while (str[curr] == 'a') {
        match('a');
    }
    // epsilon production requires no action
}

// W → cW | 𝜖
void W() {
    skipSpaces();
    
    // Check if we have 'c', otherwise assume epsilon
    while (str[curr] == 'c') {
        match('c');
    }
    // epsilon production requires no action
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
