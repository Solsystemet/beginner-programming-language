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


    inline Token* peek(int offset = 0){
        if (m_currentIndex + offset >= m_tokens.size())
            return nullptr;
        return &m_tokens.at(m_currentIndex + offset);
    }

    inline Token& consume() {
        return m_tokens.at(m_currentIndex++);
    }

    inline Token* try_consume(int type, const std::string& err_msg = "") {
        if (peek() != nullptr && peek()->type == type) {
            return &consume();
        }
        if (!err_msg.empty()) {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
        return nullptr;
    }
	
	inline node::NodeStmt* parse_stmt();
	inline node::NodeDecl* parse_decleration();
	inline node::NodeSimpleDecl* parse_simple_decleration();
	inline node::NodeArrayDecl* parse_array_decleration();
	inline node::NodeObjectDecl* parse_object_decleration();
	
	inline node::NodeNestedStmt* parse_nested_stmt();
	inline node::NodeFunctionStmt* parse_function_stmt();
	 
	inline node::NodeArithmeticExpr* parse_arithmetic_expr();
	inline node::NodeTerm* parse_term();
	inline node::NodeFactor* parse_factor();
	
	inline node::NodeStringExpr* parse_string_expr();
	 
	inline node::NodeBooleanExpr* parse_boolean_expr();
	inline node::NodeBooleanOr* parse_or();
	inline node::NodeBooleanOrOperation* parse_or_op(node::NodeBooleanOr* lhs);
	
	inline node::NodeBooleanAnd* parse_and();
	inline node::NodeBooleanAndOperation* parse_and_op(node::NodeBooleanAnd* lhs);
	
	inline node::NodeBooleanEqual* parse_equal();
	inline node::NodeBooleanEqualIsNot* parse_equal_is_not(node::NodeBooleanEqual* lhs);
	inline node::NodeBooleanEqualIs* parse_equal_is(node::NodeBooleanEqual* lhs);
	
	inline node::NodeBooleanRealExpression* parse_real_expr();
	inline node::NodeBooleanGreaterEqual* parse_greater_equal(node::NodeBooleanRealExpression* lhs);
	inline node::NodeBooleanLessEqual* parse_less_equal(node::NodeBooleanRealExpression* lhs);
	inline node::NodeBooleanGreater* parse_greater(node::NodeBooleanRealExpression* lhs);
	inline node::NodeBooleanLess* parse_less(node::NodeBooleanRealExpression* lhs);


	inline node::NodeBooleanNot* parse_not();
	inline node::NodeBooleanNotOperation* parse_not_op();
	inline node::NodeBooleanFactor* parse_boolean_factor();
	
	inline node::NodeFunctionCall* parse_function_Call();
	inline node::NodeValue* parse_value();
	
	inline node::NodeAssignment* parse_assignment();
	
	inline node::NodeGlobalControlFlow* parse_global_control_flow();
	inline node::NodeGlobalIf* parse_global_if();
	inline node::NodeGlobalElseIf* parse_global_else_if();
	inline node::NodeGlobalElse* parse_global_else();
	
	inline node::NodeGlobalLoop* parse_global_loop();
	inline node::NodeGlobalWhile* parse_global_while();
	inline node::NodeGlobalFor* parse_global_for();
	
	inline node::NodeFunctionControlFlow* parse_function_control_flow();
	inline node::NodeFunctionIf* parse_function_if();
	inline node::NodeFunctionElseIf* parse_function_else_if();
	inline node::NodeFunctionElse* parse_function_else();
	
	inline node::NodeFunctionLoop* parse_function_loop();
	inline node::NodeFunctionWhile* parse_function_while();
	inline node::NodeFunctionFor* parse_function_for();
	
	inline node::NodeDefinition* parse_definition();
	inline node::NodeFunctionDefinition* parse_function_definition();
	inline node::NodeObjectDefinition* parse_object_definition();

	// will only ever accept number, boolean, string and identifier
	inline Token* parse_type();

	inline bool verify_arithmetic_expr(size_t* index);
	inline bool verify_term(size_t* index);
	inline bool verify_factor(size_t* index);
	inline bool verify_function_call(size_t* index);
	inline bool verify_value(size_t* index);
	
	inline bool verify_string_expr(size_t* index);
	
	inline bool verify_boolean_expr(size_t* index);
	inline bool verify_or(size_t* index);
	inline bool verify_or_op(size_t* index);
	
	inline bool verify_and(size_t* index);
	inline bool verify_and_op(size_t* index);
	 
	inline bool verify_equal(size_t* index);
	inline bool verify_equal_is_not(size_t* index);
	inline bool verify_equal_is(size_t* index);
	 
	inline bool verify_real_expr(size_t* index);
	inline bool verify_greater_equal(size_t* index);
	inline bool verify_less_equal(size_t* index);
	inline bool verify_greater(size_t* index);
	inline bool verify_less(size_t* index);
	 
	inline bool verify_not(size_t* index);
	inline bool verify_not_op(size_t* index);
	inline bool verify_boolean_factor(size_t* index);
	inline bool operator_check(size_t* index);
	 
	// check for boolean operators after verify arithmetic expression
	inline bool arithmetic_operator_check(size_t* index);
    std::vector<Token> m_tokens;
    size_t m_currentIndex = 0;


	// Overload for DECIMAL
	inline node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorDecimal* factor_decimal) {
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
	inline node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorIdentifier* factor_identifier) {
		t = try_consume(IDENTIFIER);
		if (t != nullptr) {
			factor_identifier->identifier = *t;

			if (peek()->type == OPEN_SQUAREBRACKET) {
				consume();
				factor_identifier->index = parse_arithmetic_expr();
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression!");
			}

			auto* factor = new node::NodeFactor();
			factor->var = factor_identifier;
			return factor;
		}
		return nullptr;
	}

	// Overload for Parentheses (OPEN_PARANTHESIS)
	inline node::NodeFactor* try_consume_symbol(Token*& t, node::NodeArithmeticExpr* arithmetic_expr) {
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

	
};
