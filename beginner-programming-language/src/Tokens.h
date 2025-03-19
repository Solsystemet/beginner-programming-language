#pragma once
#include <string>
#include <optional>

enum class TokenType {
	number = 0,
	equal = 1,
	plus = 2,
	identifier = 3,
	print = 4,
	open_paranthesis = 5,
	closed_paranthesis = 6,
	new_line = 7,
	decimal = 8,
	end_of_file = 0xFFFFFFFF,
};

struct Token
{
	TokenType type;
	std::optional<std::string> val;
};