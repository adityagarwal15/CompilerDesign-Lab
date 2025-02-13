#include <stdio.h>
#include <stdlib.h>

int curr = 0;
char str[100];

// Function declarations
void S();
void A();
void B();
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

// S → aAcBe
void S() {
    skipSpaces();
    match('a');  // Match 'a'
    A();         // Parse A
    match('c');  // Match 'c'
    B();         // Parse B
    match('e');  // Match 'e'
}

// Original: A → Ab | b
// Transformed to: A → bA'
// A' → bA' | ε
void A() {
    skipSpaces();
    match('b');  // There must be at least one 'b'
    
    // Handle any additional b's (right recursion)
    while (str[curr] == 'b') {
        match('b');
    }
}

// B → d
void B() {
    skipSpaces();
    match('d');
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
