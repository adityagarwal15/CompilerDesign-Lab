%{
#include <stdio.h>

int pos_int = 0;
int neg_int = 0;
int pos_float = 0;
int neg_float = 0;
%}

%%

[1-9][0-9]*           { pos_int++; }      // Matches positive integers
-[1-9][0-9]*          { neg_int++; }      // Matches negative integers
[1-9][0-9]*\.[0-9]+   { pos_float++; }    // Matches positive floating-point numbers
-[1-9][0-9]*\.[0-9]+  { neg_float++; }    // Matches negative floating-point numbers

%%

int yywrap() { return 1; }

int main() {
    printf("Enter the text (Ctrl+D to stop): \n");

    yylex();  // Start lexical analysis

    // Output the counts after analysis
    printf("Positive Integers: %d\n", pos_int);
    printf("Negative Integers: %d\n", neg_int);
    printf("Positive Floating-Point Numbers: %d\n", pos_float);
    printf("Negative Floating-Point Numbers: %d\n", neg_float);

    return 0;
}

