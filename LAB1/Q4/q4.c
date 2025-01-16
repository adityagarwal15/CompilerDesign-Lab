#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Hash Table Size
#define TABLE_SIZE 50

// Hash Table
char *hashTable[TABLE_SIZE] = {NULL};

// Auxiliary Verbs List
const char *auxiliary_verbs[] = {"is", "am", "are", "was", "were", "be", "being", "been", NULL};

// Function to Hash a String
int hash(const char *str) {
    int sum = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        sum += str[i];
    }
    return sum % TABLE_SIZE;
}

// Function to Insert a Verb into the Hash Table
void insert(char *verb) {
    int index = hash(verb);
    while (hashTable[index] != NULL) {
        index = (index + 1) % TABLE_SIZE;
    }
    hashTable[index] = strdup(verb); // Duplicate and store
    printf("Verb '%s' inserted at index %d.\n", verb, index);
}

// Function to Search for a Verb
int search(const char *verb) {
    int index = hash(verb);
    int start = index;
    while (hashTable[index] != NULL) {
        if (strcmp(hashTable[index], verb) == 0) {
            return index; // Verb found
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == start) break; // Full loop
    }
    return -1; // Verb not found
}

// Function to Check if a Word is a Verb
int is_verb(const char *word) {
    // Check for auxiliary verbs
    for (int i = 0; auxiliary_verbs[i] != NULL; i++) {
        if (strcmp(word, auxiliary_verbs[i]) == 0) {
            return 1;
        }
    }
    // Check for common verb endings
    int len = strlen(word);
    if (len > 3 && strcmp(&word[len - 3], "ing") == 0) return 1; // -ing ending
    if (len > 2 && strcmp(&word[len - 2], "ed") == 0) return 1;  // -ed ending
    if (len > 1 && word[len - 1] == 's') return 1;               // -s ending
    return 0;
}

int main() {
    char sentence[256];
    printf("Enter a sentence: ");
    fgets(sentence, sizeof(sentence), stdin);

    // Tokenize Sentence
    char *word = strtok(sentence, " .,!?");
    while (word != NULL) {
        // Check if Word is a Verb
        if (is_verb(word)) {
            if (search(word) == -1) { // Avoid duplicates
                insert(word);
            }
        }
        word = strtok(NULL, " .,!?");
    }

    // Search for a Verb
    char verb[50];
    printf("\nEnter a verb to search: ");
    scanf("%s", verb);
    int index = search(verb);
    if (index != -1) {
        printf("Verb '%s' found at index %d in the hash table.\n", verb, index);
    } else {
        printf("Verb '%s' not found in the hash table.\n", verb);
    }

    return 0;
}

