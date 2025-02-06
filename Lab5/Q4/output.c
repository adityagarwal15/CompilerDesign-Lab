#include <stdio.h>

int main() {
    int a;
    READ("%d", &a);
    WRITE("The value is: %d", a);
    READ("%d", &a);
    return 0;
}

