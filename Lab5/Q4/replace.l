%{
#include <stdio.h>

int count_scanf = 0;  // Count the number of scanf
int count_printf = 0; // Count the number of printf

// Declare the input and output files globally
FILE *yyin;  // Input file
FILE *yyout; // Output file
%}

%%

scanf\([^\)]*\)   {  // Match 'scanf' with any characters inside parentheses
    count_scanf++;   // Increment the scanf count
    fprintf(yyout, "READ%s", yytext + 5);  // Replace 'scanf' with 'READ'
}

printf\([^\)]*\)   {  // Match 'printf' with any characters inside parentheses
    count_printf++;   // Increment the printf count
    fprintf(yyout, "WRITE%s", yytext + 6);  // Replace 'printf' with 'WRITE'
}

.|\n {  // For any other characters, print them as-is
    fputc(yytext[0], yyout);  // Write the current character to the output file
}

%%

int yywrap() {
    return 1;  // End the scanning when the input is finished
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Open the input file for reading
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Open the output file for writing
    yyout = fopen(argv[2], "w");
    if (yyout == NULL) {
        perror("Error opening output file");
        fclose(yyin);
        return 1;
    }

    // Perform lexical analysis
    yylex();

    // Output the counts after the analysis
    printf("Number of 'scanf' replaced: %d\n", count_scanf);
    printf("Number of 'printf' replaced: %d\n", count_printf);

    // Close the files
    fclose(yyin);
    fclose(yyout);

    return 0;
}

