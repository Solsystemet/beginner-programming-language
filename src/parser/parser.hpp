#pragma once
#include <vector>
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include "../mpark/variant.hpp"
#include "../nodes.hpp"



class Parser
{
public:
	// Constructor
	Parser(std::vector<Token> tokens)
		: m_tokens(std::move(tokens)) {
	}

	node::NodeProg parse_prog();

private:


    Token* peek(int offset = 0){
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
	
	node::NodeStmt* parse_stmt();
	node::NodeDecl* parse_decleration();
	node::NodeSimpleDecl* parse_simple_decleration();
	node::NodeArrayDecl* parse_array_decleration();
	node::NodeObjectDecl* parse_object_decleration();

	node::NodeNestedStmt* parse_nested_stmt();
	node::NodeFunctionStmt* parse_function_stmt();


	node::NodeArithmeticExpr* parse_arithmetic_expr();
	node::NodeTerm* parse_term();
	node::NodeFactor* parse_factor();

	node::NodeStringExpr* parse_string_expr();

	node::NodeBooleanExpr* parse_boolean_expr();
	node::NodeBooleanOr* parse_or();
	node::NodeBooleanOrOperation* parse_or_op(node::NodeBooleanOr* lhs);


	node::NodeBooleanAnd* parse_and();
	node::NodeBooleanAndOperation* parse_and_op(node::NodeBooleanAnd* lhs);


	node::NodeBooleanEqual* parse_equal();
	node::NodeBooleanEqualIsNot* parse_equal_is_not(node::NodeBooleanEqual* lhs);
	node::NodeBooleanEqualIs* parse_equal_is(node::NodeBooleanEqual* lhs);


	node::NodeBooleanRealExpression* parse_real_expr();
	node::NodeBooleanGreaterEqual* parse_greater_equal(node::NodeBooleanRealExpression* lhs);
	node::NodeBooleanLessEqual* parse_less_equal(node::NodeBooleanRealExpression* lhs);
	node::NodeBooleanGreater* parse_greater(node::NodeBooleanRealExpression* lhs);
	node::NodeBooleanLess* parse_less(node::NodeBooleanRealExpression* lhs);


	node::NodeBooleanNot* parse_not();
	node::NodeBooleanNotOperation* parse_not_op();
	node::NodeBooleanFactor* parse_boolean_factor();

	node::NodeFunctionCall* parse_function_Call();
	node::NodeValue* parse_value();

	node::NodeAssignment* parse_assignment();

	node::NodeGlobalControlFlow* parse_global_control_flow();
	node::NodeGlobalIf* parse_global_if();
	node::NodeGlobalElseIf* parse_global_else_if();
	node::NodeGlobalElse* parse_global_else();

	node::NodeGlobalLoop* parse_global_loop();
	node::NodeGlobalWhile* parse_global_while();
	node::NodeGlobalFor* parse_global_for();

	node::NodeFunctionControlFlow* parse_function_control_flow();
	node::NodeFunctionIf* parse_function_if();
	node::NodeFunctionElseIf* parse_function_else_if();
	node::NodeFunctionElse* parse_function_else();

	node::NodeFunctionLoop* parse_function_loop();
	node::NodeFunctionWhile* parse_function_while();
	node::NodeFunctionFor* parse_function_for();

	node::NodeDefinition* parse_definition();
	node::NodeFunctionDefinition* parse_function_definition();
	node::NodeObjectDefinition* parse_object_definition();

	// will only ever accept number, boolean, string and identifier
	Token* parse_type();

	bool verify_arithmetic_expr(size_t* index);
	bool verify_term(size_t* index);
	bool verify_factor(size_t* index);
	bool verify_function_call(size_t* index);
	bool verify_value(size_t* index);

	bool verify_string_expr(size_t* index);

	bool verify_boolean_expr(size_t* index);
	bool verify_or(size_t* index);
	bool verify_or_op(size_t* index);


	bool verify_and(size_t* index);
	bool verify_and_op(size_t* index);


	bool verify_equal(size_t* index);
	bool verify_equal_is_not(size_t* index);
	bool verify_equal_is(size_t* index);


	bool verify_real_expr(size_t* index);
	bool verify_greater_equal(size_t* index);
	bool verify_less_equal(size_t* index);
	bool verify_greater(size_t* index);
	bool verify_less(size_t* index);


	bool verify_not(size_t* index);
	bool verify_not_op(size_t* index);
	bool verify_boolean_factor(size_t* index);
	bool operator_check(size_t* index);

	// check for boolean operators after verify arithmetic expression
	bool arithmetic_operator_check(size_t* index);
	bool string_operator_check(size_t* index);
	bool function_operator_check(size_t* index);
    std::vector<Token> m_tokens;
    size_t m_currentIndex = 0;


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

			if (peek()->type == OPEN_SQUAREBRACKET) {
				consume();
				factor_identifier->index = parse_arithmetic_expr();
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression!");
			}

			while (peek() && peek()->type == DOT) {
				consume();
				if (peek() && peek()->type == IDENTIFIER) {
					factor_identifier->props.push_back(consume());
				}
				else {
					std::cerr << "Expected identifier property in expression";
					exit(EXIT_FAILURE);
				}
			}

			auto* factor = new node::NodeFactor();
			factor->var = factor_identifier;
			return factor;
		}
		return nullptr;
	}

	// Overload for Parentheses (OPEN_PARANTHESIS)
	node::NodeFactor* try_consume_symbol(Token*& t, node::NodeArithmeticExpr* arithmetic_expr) {
		t = try_consume(OPEN_PARANTHESIS);
		if (t != nullptr) {
			arithmetic_expr = parse_arithmetic_expr();
			try_consume(CLOSED_PARANTHESIS, "Expected ')'");
			auto factor = new node::NodeFactor();
			factor->var = arithmetic_expr;
			return factor;
		}
		return nullptr;
	}




	void try_consume_arithmetic(
		int tokenType,
		node::NodeArithmeticExpr* expr,
		node::NodeExprAdd* add
	) {
		if (peek()->type == tokenType) {
			consume();
			auto expr_lhs = new node::NodeArithmeticExpr();
			expr_lhs->var = expr->var;
			add->lhs = expr_lhs;

			auto* term_rhs = parse_term();
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
		node::NodeExprSubtract* sub
	) {
		if (peek()->type == tokenType) {
			consume();
			auto expr_lhs = new node::NodeArithmeticExpr();
			expr_lhs->var = expr->var;
			sub->lhs = expr_lhs;

			auto* term_rhs = parse_term();
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
		node::NodeExprMult* mult
	) {
		if (peek()->type == tokenType) {
			consume();

			auto* term_mult = new node::NodeExprMult();
			term_mult->lhs = new node::NodeTerm();
			term_mult->lhs->var = term->var;
			node::NodeFactor* factor_rhs = parse_factor();
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
		node::NodeExprDivide* div
	) {
		if (peek()->type == DIVIDE) {
			consume();

			auto* term_divide = new node::NodeExprDivide();
			term_divide->lhs = new node::NodeTerm();
			term_divide->lhs->var = term->var;
			node::NodeFactor* factor_rhs = parse_factor();
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
		node::NodeExprModulo* mod
	) {
		if (peek()->type == MODULO) {
			consume();

			auto* term_modulo = new node::NodeExprModulo();
			term_modulo->lhs = new node::NodeTerm();
			term_modulo->lhs->var = term->var;
			node::NodeFactor* factor_rhs = parse_factor();
			if (factor_rhs == nullptr) {
				std::cerr << "Invalid factor!" << std::endl;
				exit(EXIT_FAILURE);
			}
			term_modulo->rhs = factor_rhs;
			term->var = term_modulo;
		}
	}



	
};
