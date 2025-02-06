%{
#include <stdio.h>
#include <stdlib.h>

/* File pointers for input and output */
FILE *yyin, *yyout;
%}

%%
[0-9]+ {
    int num = atoi(yytext);
    fprintf(yyout, "0x%X", num);
}
[ \t\n] ECHO;  /* Echo whitespace characters as they are */
. ECHO;        /* Echo all other characters as they are */
%%

int yywrap(void) {
    return 1;
}

int main(int argc, char *argv[]) {
    /* Check if correct number of arguments */
    if(argc != 3) {
        printf("Usage: %s input-file output-file\n", argv[0]);
        return 1;
    }

    /* Open input file */
    if(!(yyin = fopen(argv[1], "r"))) {
        printf("Cannot open input file\n");
        return 1;
    }

    /* Open output file */
    if(!(yyout = fopen(argv[2], "w"))) {
        printf("Cannot open output file\n");
        return 1;
    }

    /* Run the lexical analyzer */
    yylex();

    /* Close the files */
    fclose(yyin);
    fclose(yyout);
    return 0;
}
