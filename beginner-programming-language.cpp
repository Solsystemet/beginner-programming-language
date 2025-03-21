#include <iostream>
#include "./src/lex.yy.c"
#include "./src/tokens.h"
#include <string.h>
#include <vector>

extern int yylex();
extern FILE* yyin;
extern char* yytext;

struct Token{
    int type;
    std::string value;
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    int ntoken;
    std::vector<Token> result;
    while ((ntoken = yylex())) {
        Token t;
        switch (ntoken)
        {
        case NEW_LINE:
            t.type = NEW_LINE;
            result.push_back(t);
            break;
        
        case DECIMAL:
            t.type = DECIMAL;
            t.value = yytext;
            result.push_back(t);
            break;
        case NUMBER:
            t.type = NUMBER;
            t.value = yytext;
            result.push_back(t);
            break;
        case PRINT:
            t.type = PRINT;
            result.push_back(t);
            break;
        case OPEN_PARANTHESIS:
            t.type = OPEN_PARANTHESIS;
            result.push_back(t);
            break;
        case CLOSED_PARANTHESIS:
            t.type = CLOSED_PARANTHESIS;
            result.push_back(t);
            break;
        case EQUAL:
            t.type = EQUAL;
            result.push_back(t);
            break;
        case PLUS:
            t.type = PLUS;
            result.push_back(t);
            break;
        case MINUS:
            t.type = MINUS;
            result.push_back(t);
            break;
        case MULTIPLY:  
            t.type = MULTIPLY;
            result.push_back(t);
            break;
        case DIVIDE:
            t.type = DIVIDE;
            result.push_back(t);
            break;      
        case MODULO:
            t.type = MODULO;
            result.push_back(t);
            break;
        case AND:
            t.type = AND;
            result.push_back(t);
            break;
        case OR:   
            t.type = OR;
            result.push_back(t);
            break;
        case NOT:
            t.type = NOT;
            result.push_back(t);
            break;
        case FUNCTION:
            t.type = FUNCTION;
            result.push_back(t);
            break;
        case ARGUMENTSPLIT:
            t.type = ARGUMENTSPLIT;
            result.push_back(t);
            break;
        case TRUE:  
            t.type = TRUE;
            result.push_back(t);
            break;
        case FALSE:
            t.type = FALSE;
            result.push_back(t);
            break;
        case IDENTIFIER:
            t.type = IDENTIFIER;
            t.value = yytext;
            result.push_back(t);
            break;
        
        default:
            //error
            break;
        }

    }

    for(Token t : result){
        std::cout << t.type << std::endl;
        std::cout << t.value << std::endl;
    }

    fclose(yyin);
    return 0;
}
