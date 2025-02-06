%{
#include <stdio.h>
#include <stdlib.h>

int vowels = 0;
int consonants = 0;
%}

%%
[aAeEiIoOuU]    { vowels++; }    /* Matches vowels */
[b-df-hj-np-tv-zB-DF-HJ-NP-TV-Z]    { consonants++; }    /* Matches consonants */
.|\n    { /* Ignore other characters */ }
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Enter a string: ");
    yylex();
    
    /* Print the final counts */
    printf("\nResults:\n");
    printf("Number of vowels: %d\n", vowels);
    printf("Number of consonants: %d\n", consonants);
    
    return 0;
}
