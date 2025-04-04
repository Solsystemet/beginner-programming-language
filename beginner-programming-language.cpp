#include <iostream>
#include <string.h>
#include <vector>
#include "src/lexer.h"
#include "src/parser/parser.hpp"

int main(int argc, char* argv[])
{
    /*
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    */
    FILE* input = fopen("parsetest.txt", "w");
    fprintf(input,  "number z = 2 + 2\nprint(z)");
    fclose(input);

    Lexer lexer = Lexer(input);
    std::vector<Token> result = lexer.Tokenize(); 

    Parser parser = Parser(result);
    node::NodeProg ast = parser.parse_prog();
    


    fclose(yyin);
    return 0;
}

