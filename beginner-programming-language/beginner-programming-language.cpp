#include <iostream>

#include "src/Lexer.h"

int main()
{
    std::string test = "number x = 10\nnumbery = 5\nnumber z = x + y\nprint(z)";
    Lexer lexer = Lexer(std::move(test));

    std::vector<Token> tokens = lexer.Tokenize();
    
    for (Token t : tokens) {
        std::cout << (int)t.type << std::endl;
    }

}
