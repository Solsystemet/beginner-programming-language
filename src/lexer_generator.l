%{
   #include "tokens.h"
   #include <string.h>
%}

%%

\n                              { return NEW_LINE; }
[0-9]+(\.[0-9]+)?               { return DECIMAL; }
"("                             { return OPEN_PARANTHESIS; }
")"                             { return CLOSED_PARANTHESIS; }
"="                             { return EQUAL; }
"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return MULTIPLY; }
"/"                             { return DIVIDE; }
","                             { return ARGUMENTSPLIT; }

[ ]+                            ; // Ignorer mellemrum

[a-zA-Z_][a-zA-Z_0-9]* {
    if (strcmp(yytext, "number") == 0) return NUMBER;
    if (strcmp(yytext, "print") == 0) return PRINT;
    if (strcmp(yytext, "function") == 0) return FUNCTION;
    if (strcmp(yytext, "and") == 0) return AND;
    if (strcmp(yytext, "or") == 0) return OR;
    if (strcmp(yytext, "not") == 0) return NOT;
    if (strcmp(yytext, "mod") == 0) return MODULO;
    if (strcmp(yytext, "true") == 0) return TRUE;
    if (strcmp(yytext, "false") == 0) return FALSE;
    return IDENTIFIER;
}

.                               ; // Ignorer udefinerede tegn (valgfrit, ellers print fejl)
%%

int yywrap(void) {
   return 1;
}
