#pragma once
#include "../lexer/tokens.h"
#include "../lexer/tokenvalues.h"
#include <iostream>
#include <stdio.h>
class errorHandling
{
public:
	static FILE* file;


	static void syntax_error(Token current_token, Token expected_token);
	static std::string get_line_from_file(int line_num);
	static std::string token_to_string(Token t);
};

