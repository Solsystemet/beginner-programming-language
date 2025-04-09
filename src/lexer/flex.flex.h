#ifndef FLEX_FLEX_H
#define FLEX_FLEX_H
#include <stdio.h>

int yylex();

extern FILE* yyin;
extern char* yytext;

#endif 
