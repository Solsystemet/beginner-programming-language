#pragma once
#include "../lexer/tokens.h"
#include "../mpark/variant.hpp"
#include "./parser.hpp"
#include "../../nodes.hpp"

int op_prec(int t);

Token* peek(int offset = 0);

Token& consume();

Token* try_consume(int type, const std::string& err_msg = "");

node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorDecimal* factor_decimal);

node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorIdentifier* factor_identifier);

node::NodeFactor* try_consume_symbol(Token*& t, node::NodeArithmeticExpr* arithmetic_expr, Parser* parser);

void try_consume_arithmetic(int tokenType, node::NodeArithmeticExpr* expr, node::NodeExprAdd* add, Parser* parser);

void try_consume_arithmetic(int tokenType, node::NodeArithmeticExpr* expr, node::NodeExprSubtract* sub, Parser* parser);

void try_consume_arithmetic(int tokenType, node::NodeTerm* term, node::NodeExprMult* mult, Parser* parser);

void try_consume_arithmetic(int tokenType, node::NodeTerm* term, node::NodeExprDivide* div, Parser* parser);

void try_consume_arithmetic( int tokenType, node::NodeTerm* term, node::NodeExprModulo* mod, Parser* parser);






