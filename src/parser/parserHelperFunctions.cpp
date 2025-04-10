#include "parserHelperFunctions.hpp"

int op_prec(int t) {
	switch (t)
	{
	case PLUS:
		return 0;
	default:
		return -1;
	};
}

Token* peek(int offset = 0) {
	if (m_currentIndex + offset >= m_tokens.size())
		return nullptr;
	return &m_tokens.at(m_currentIndex + offset);
}

Token& consume() {
	return m_tokens.at(m_currentIndex++);
}

Token* try_consume(int type, const std::string& err_msg = "") {
	if (peek() != nullptr && peek()->type == type) {
		return &consume();
	}
	if (!err_msg.empty()) {
		std::cerr << err_msg << std::endl;
		exit(EXIT_FAILURE);
	}
	return nullptr;
}

std::vector<Token> m_tokens;
size_t m_currentIndex;

// Overload for DECIMAL
node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorDecimal* factor_decimal) {
	t = try_consume(DECIMAL);
	if (t != nullptr) {
		factor_decimal->decimal = *t;

		auto* factor = new node::NodeFactor();
		factor->var = factor_decimal;
		return factor;
	}
	return nullptr;
}

// Overload for IDENTIFIER
node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorIdentifier* factor_identifier) {
	t = try_consume(IDENTIFIER);
	if (t != nullptr) {
		factor_identifier->identifier = *t;

		auto* factor = new node::NodeFactor();
		factor->var = factor_identifier;
		return factor;
	}
	return nullptr;
}

// Overload for Parentheses (OPEN_PARANTHESIS)
node::NodeFactor* try_consume_symbol(Token*& t, node::NodeArithmeticExpr* arithmetic_expr, Parser* parser) {
	t = try_consume(OPEN_PARANTHESIS);
	if (t != nullptr) {
		arithmetic_expr = parser->parse_arithmetic_expr();
		try_consume(CLOSED_PARANTHESIS, "Expected ')'");
		auto* factor = new node::NodeFactor();
		factor->var = arithmetic_expr;
		return factor;
	}
	return nullptr;
}

void try_consume_arithmetic(
	int tokenType,
	node::NodeArithmeticExpr* expr,
	node::NodeExprAdd* add,
	Parser* parser
) {
	if (peek()->type == tokenType) {
		consume();
		auto* expr_lhs = new node::NodeArithmeticExpr();
		expr_lhs->var = expr->var;
		add->lhs = expr_lhs;

		auto* term_rhs = parser->parse_term();
		if (term_rhs == nullptr) {
			std::cerr << "Expected term after operator" << std::endl;
			exit(EXIT_FAILURE);
		}

		add->rhs = term_rhs;
		expr->var = add;
	}
}

void try_consume_arithmetic(
	int tokenType,
	node::NodeArithmeticExpr* expr,
	node::NodeExprSubtract* sub,
	Parser* parser
) {
	if (peek()->type == tokenType) {
		consume();
		node::NodeArithmeticExpr* expr_lhs = new node::NodeArithmeticExpr();
		expr_lhs->var = expr->var;
		sub->lhs = expr_lhs;

		auto* term_rhs = parser->parse_term();
		if (term_rhs == nullptr) {
			std::cerr << "Expected term after operator" << std::endl;
			exit(EXIT_FAILURE);
		}

		sub->rhs = term_rhs;
		expr->var = sub;
	}
}

void try_consume_arithmetic(
	int tokenType,
	node::NodeTerm* term,
	node::NodeExprMult* mult,
	Parser* parser
) {
	if (peek()->type == tokenType) {
		consume();

		auto* term_mult = new node::NodeExprMult();
		term_mult->lhs = term;
		node::NodeFactor* factor_rhs = parser->parse_factor();
		if (factor_rhs == nullptr) {
			std::cerr << "Invalid factor!" << std::endl;
			exit(EXIT_FAILURE);
		}
		term_mult->rhs = factor_rhs;
		term->var = term_mult;
	}
}

void try_consume_arithmetic(
	int tokenType,
	node::NodeTerm* term,
	node::NodeExprDivide* div,
	Parser* parser
) {
	if (peek()->type == tokenType) {
		consume();

		auto* term_divide = new node::NodeExprDivide();
		term_divide->lhs = term;
		node::NodeFactor* factor_rhs = parser->parse_factor();
		if (factor_rhs == nullptr) {
			std::cerr << "Invalid factor!" << std::endl;
			exit(EXIT_FAILURE);
		}
		term_divide->rhs = factor_rhs;
		term->var = term_divide;
	}
}

void try_consume_arithmetic(
	int tokenType,
	node::NodeTerm* term,
	node::NodeExprModulo* mod,
	Parser* parser
) {
	if (peek()->type == MODULO) {
		consume();
		auto* term_modulo = new node::NodeExprModulo();
		term_modulo->lhs = term;
		node::NodeFactor* factor_rhs = parser->parse_factor();
		if (factor_rhs == nullptr) {
			std::cerr << "Invalid factor!" << std::endl;
			exit(EXIT_FAILURE);
		}
		term_modulo->rhs = factor_rhs;
		term->var = term_modulo;
	}
}

