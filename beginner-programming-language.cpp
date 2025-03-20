#include <iostream>
#include "./src/lex.c"
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
