%{
#include "bison.tab.h"
#include <string.h>
#include <stdlib.h>
extern YYSTYPE yylval;
char* yytext;
%}

%%
"decl"      { return DECLARATION_MODE; }
"decision"  { return DECISION_MODE; }
"infix"     { return INFIX_MODE; }
"postfix"   { return POSTFIX_MODE; }

"int"       { return INT; }
"float"     { return FLOAT; }
"char"      { return CHAR; }
"double"    { return DOUBLE; }

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"do"        { return DO; }
"return"    { return RETURN; }

"=="        { return EQ; }
"!="        { return NE; }
"<"         { return LT; }
">"         { return GT; }
"<="        { return LE; }
">="        { return GE; }
"&&"        { return AND; }
"||"        { return OR; }
"!"         { return NOT; }

"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
";"         { return SEMICOLON; }
","         { return COMMA; }

[0-9]+(\.[0-9]+)?    { yylval.val = atof(yytext); return NUMBER; }
[a-zA-Z][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }
\"[^\"]*\"            { yylval.str = strdup(yytext); return STRING; }

[ \t]      { /* ignore whitespace */ }
\n         { return NEWLINE; }
.          { return yytext[0]; }
%%

int yywrap() {
    return 1;
}
