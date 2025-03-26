%{
#include <stdio.h>
#include <string.h>

int count_scanf = 0;  // Count the number of scanf
int count_printf = 0; // Count the number of printf

FILE *yyin;  // Input file
FILE *yyout; // Output file
%}

%%

"scanf"[\t ]*\([^)]+\) {  
    count_scanf++;  
    fprintf(yyout, "READ%s", yytext + 5);  
}

"printf"[\t ]*\([^)]+\) {  
    count_printf++;  
    fprintf(yyout, "WRITE%s", yytext + 6);  
}

.|\n {  
    fprintf(yyout, "%s", yytext);
}

%%

int yywrap() {
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Error opening input file");
        return 1;
    }

    yyout = fopen(argv[2], "w");
    if (yyout == NULL) {
        perror("Error opening output file");
        fclose(yyin);
        return 1;
    }

    yylex();

    printf("Number of 'scanf' replaced: %d\n", count_scanf);
    printf("Number of 'printf' replaced: %d\n", count_printf);

    fclose(yyin);
    fclose(yyout);

    return 0;
}
