%{
   #include "tokenvalues.h"
   #include <string.h>
%}

%%

\n                              { return NEW_LINE; }
\t                             { return TAB_INDENT; }

(-)?([0-9]+(\.[0-9]+)?)      { return DECIMAL; }


"("                             { return OPEN_PARANTHESIS; }
")"                             { return CLOSED_PARANTHESIS; }
"["                             { return OPEN_SQUAREBRACKET; }
"]"                             { return CLOSED_SQUAREBRACKET; }
"="                             { return EQUAL; }
"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return MULTIPLY; }
"/"                             { return DIVIDE; }
","                             { return COMMA; }
"."                             { return DOT; }
":"                             { return COLON; }
"<"                             { return LESS; }
">"                             { return GREATER; }
[ ]+                            ; // Ignore spaces

 \"([^\\\"]|\\.)*\"  { return STRING_VAL; }

[a-zA-Z_][a-zA-Z_0-9]* {
    if (strcmp(yytext, "number") == 0) return NUMBER;
    if (strcmp(yytext, "string") == 0) return STRING;
    if (strcmp(yytext, "boolean") == 0) return BOOLEAN;
    if (strcmp(yytext, "print") == 0) return PRINT;
    if (strcmp(yytext, "function") == 0) return FUNCTION;
    if (strcmp(yytext, "and") == 0) return AND;
    if (strcmp(yytext, "or") == 0) return OR;
    if (strcmp(yytext, "not") == 0) return NOT;
    if (strcmp(yytext, "mod") == 0) return MODULO;
    if (strcmp(yytext, "true") == 0 || strcmp(yytext, "false") == 0) return BOOLVAL;
    if (strcmp(yytext, "for") == 0) return FOR;
    if (strcmp(yytext, "while") == 0) return WHILE;
    if (strcmp(yytext, "if") == 0) return IF;
    if (strcmp(yytext, "else") == 0) return ELSE;
    if (strcmp(yytext, "return") == 0) return RETURN;
    if (strcmp(yytext, "is") == 0) return IS;
    if (strcmp(yytext, "input") == 0) return INPUT;
    if (strcmp(yytext, "break") == 0) return BREAK;
    if (strcmp(yytext, "continue") == 0) return CONTINUE;
    return IDENTIFIER;
}

.                               ; // Ignore undefined characters

%%

int yywrap(void) {
   return 1;
}
