#include <iostream>
#include <string.h>
#include <vector>
#include "src/lexer.h"


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    Lexer lexer = Lexer(input);
    std::vector<Token> result = lexer.Tokenize(); 

    fclose(yyin);
    return 0;
}

