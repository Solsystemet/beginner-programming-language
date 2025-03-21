%{
   #include "tokens.h"
   #include <string.h>
%}

%%

\n                              { return NEW_LINE; }
[0-9]+(\.[0-9]+)?               { return DECIMAL; }
"("                             { return OPEN_PARANTHESIS; }
")"                             { return CLOSED_PARANTHESIS; }
"["                             { return OPEN_SQUAREBRACKET; }
"]"                             { return CLOSED_SQUAREBRACKET; }
"="                             { return EQUAL; }
"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return MULTIPLY; }
"/"                             { return DIVIDE; }
","                             { return ARGUMENTSPLIT; }
"."                             { return DOT; }
":"                             { return COLON; }

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
    if (strcmp(yytext, "for") == 0) return FOR;
    if (strcmp(yytext, "while") == 0) return WHILE;
    if (strcmp(yytext, "if") == 0) return IF;
    if (strcmp(yytext, "else") == 0) return ELSE;
    if (strcmp(yytext, "return") == 0) return RETURN;
    if (strcmp(yytext, "IS") == 0) return IS;
    return IDENTIFIER;
}

.                               ; // Ignorer udefinerede tegn
%%

int yywrap(void) {
   return 1;
}
