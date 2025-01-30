#include <stdio.h>

#define PI 3.14  // Constant definition

// Structure definition
struct Student {
    int id;
    char name[50];
    float marks;
};

// Function prototype
int add(int a, int b);

int main() {
    int count = 5;
    float temperature = 36.5;
    char grade = 'A';

    // Array declaration
    int numbers[5] = {1, 2, 3, 4, 5};

    // Pointer declaration
    int *ptr = &count;

    // Loop example
    for(int i = 0; i < count; i++) {
        printf("Number: %d\n", numbers[i]);
    }

    // Conditional example
    if(temperature > 37.0) {
        printf("Fever detected!\n");
    } else {
        printf("Normal temperature.\n");
    }

    // Function call
    int result = add(count, 10);
    
    // Structure usage
    struct Student s1;
    s1.id = 101;
    s1.marks = 95.5;

    return 0;
}

// Function definition
int add(int a, int b) {
    return a + b;
}

