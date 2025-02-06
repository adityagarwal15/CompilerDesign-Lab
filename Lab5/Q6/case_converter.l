%{
#include <stdio.h>
#include <stdlib.h>

/* File pointers for input and output */
FILE *yyin, *yyout;
int in_comment = 0;  /* Flag to track if we're inside a comment */
int in_string = 0;   /* Flag to track if we're inside a string */
%}

%x COMMENT_SINGLE
%x COMMENT_MULTI
%x STRING

%%
"/*"                    { BEGIN(COMMENT_MULTI); fprintf(yyout, "%s", yytext); }
<COMMENT_MULTI>"*/"     { BEGIN(INITIAL); fprintf(yyout, "%s", yytext); }
<COMMENT_MULTI>.|\n     { fprintf(yyout, "%s", yytext); }

"//"                    { BEGIN(COMMENT_SINGLE); fprintf(yyout, "%s", yytext); }
<COMMENT_SINGLE>\n      { BEGIN(INITIAL); fprintf(yyout, "%s", yytext); }
<COMMENT_SINGLE>.       { fprintf(yyout, "%s", yytext); }

\"                      { BEGIN(STRING); fprintf(yyout, "%s", yytext); }
<STRING>\\\"           { fprintf(yyout, "%s", yytext); }  /* Escaped quote in string */
<STRING>\"             { BEGIN(INITIAL); fprintf(yyout, "%s", yytext); }
<STRING>.              { fprintf(yyout, "%s", yytext); }

[A-Z]                   { fprintf(yyout, "%c", yytext[0] + 32); }  /* Convert uppercase to lowercase */
.|\n                    { fprintf(yyout, "%s", yytext); }  /* Copy everything else as is */

%%

int yywrap(void) {
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s input-file output-file\n", argv[0]);
        return 1;
    }

    /* Open input file */
    yyin = fopen(argv[1], "r");
    if(yyin == NULL) {
        printf("Cannot open input file %s\n", argv[1]);
        return 1;
    }

    /* Open output file */
    yyout = fopen(argv[2], "w");
    if(yyout == NULL) {
        printf("Cannot open output file %s\n", argv[2]);
        fclose(yyin);
        return 1;
    }

    /* Run the lexical analyzer */
    yylex();

    /* Close the files */
    fclose(yyin);
    fclose(yyout);
    printf("Conversion completed successfully!\n");
    return 0;
}
