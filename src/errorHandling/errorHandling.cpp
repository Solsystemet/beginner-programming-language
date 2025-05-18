#include "errorHandling.h"

FILE* errorHandling::file = nullptr;

void errorHandling::syntax_error(Token current_token, Token expected_token)
{
    std::cerr << "Issue at line " << current_token.lineNum << " -> " << get_line_from_file(current_token.lineNum) << std::endl << std::endl;
	std::cerr << "On line " << current_token.lineNum << " the expected token was '" << token_to_string(expected_token) << "'" << std::endl;
	std::cerr << "Found '" << token_to_string(current_token) << "' instead" << std::endl;
	std::cerr << "Add the expected token: '" << token_to_string(expected_token) << "'\n" << std::endl;
	exit(EXIT_FAILURE);
}

void errorHandling::semantic_error(Symbol lhs_type, std::string rhs_type)
{
    std::cerr << "Trying to assign symbol: '" << lhs_type.name << "' to a value of type '" << rhs_type << "'" << std::endl;
    std::cerr << "'" << lhs_type.name << "'" << "is of type '" << lhs_type.type << "'" << std::endl;
    std::cerr << "Find '" << lhs_type.name << "' " << "and make sure to assign it to a correct type" << std::endl;
    exit(EXIT_FAILURE);
}

std::string errorHandling::get_line_from_file(int line_num)
{
    if (!file)
        return "";

    int count = 1;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (count == line_num)
            return std::string(line);
        count++;
    }

    return "";  // if line_num is out of range
}


std::string errorHandling::token_to_string(Token t)
{
    switch (t.type)
    {
    case NEW_LINE:
        return "new line";
    case TAB_INDENT:
        return "tab indent";
    case TAB_DEDENT:
        return "tab dedent";
    case DECIMAL:
        return "decimal";
    case NUMBER:
        return "number";
    case PRINT:
        return "print";
    case OPEN_PARANTHESIS:
        return "(";
    case CLOSED_PARANTHESIS:
        return ")";
    case OPEN_SQUAREBRACKET:
        return "[";
    case CLOSED_SQUAREBRACKET:
        return "]";
    case EQUAL:
        return "=";
    case PLUS:
        return "+";
    case MINUS:
        return "-";
    case MULTIPLY:
        return "*";
    case DIVIDE:
        return "/";
    case MODULO:
        return "mod";
    case AND:
        return "and";
    case OR:
        return "or";
    case LESS:
        return "<";
    case NOT:
        return "not";
    case FUNCTION:
        return "function";
    case COMMA:
        return ",";
    case BOOLVAL:
        return "true or false";
    case FOR:
        return "for";
    case WHILE:
        return "while";
    case COLON:
        return ":";
    case IF:
        return "if";
    case ELSE:
        return "else";
    case RETURN:
        return "return";
    case DOT:
        return ".";
    case IS:
        return "is";
    case IDENTIFIER:
        return "some variable";
    case GREATER:
        return ">";
    case BOOLEAN:
        return "boolean";
    case STRING:
        return "string";
    case INPUT:
        return "input";
    case STRING_VAL:
        return "string value like \"Hello, World!\"";
    case EOF:
        return "end_of_file";
    default:
        // error or unrecognized token
        std::cerr << "HOW DID WE GET HERE?";
        exit(EXIT_FAILURE);
        break;
    }
}
