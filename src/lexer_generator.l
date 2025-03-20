%{
   #include "tokens.h"
%}

%%
\n              { return NEW_LINE; }
([1-9][0-9]*|0)\.[0-9]+ { return DECIMAL; }
"number"       { return NUMBER; }
"print"        { return PRINT; }
"("            { return OPEN_PARANTHESIS; }
")"            { return CLOSED_PARANTHESIS; }
"="            { return EQUAL; }
"+"            { return PLUS; }
[ ]+ ;
[a-zA-Z_][a-zA-Z_0-9]*            { return IDENTIFIER; }
%%

int yywrap(void) {
   return 1;
}
