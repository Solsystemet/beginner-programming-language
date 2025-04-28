#pragma once
#include <unordered_map>
#include <stack>
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include "../mpark/variant.hpp"
#include "../nodes.hpp"
#include "./SymbolTable.h"



class Evaluator
{

public:
	Evaluator(node::NodeProg prog):
		m_prog(std::move(prog)) {}

	void evaluate_prog();

	void evaluate_stmt(const node::NodeStmt* stmt);

	void evaluate_print(const node::NodeStmtPrint* print_stmt);

	void evaluate_declecration(const node::NodeDecl* decl);
	void evaluate_simple_decleration(const node::NodeSimpleDecl* simp_decl);
	void evaluate_arithmetic_expression(const node::NodeArithmeticExpr* expr);
	void evaluate_term(const node::NodeTerm* term);
	void evaluate_factor(const node::NodeFactor* factor);

	void evaluate_boolean_expression(const node::NodeBooleanExpr* expr);
	void evaluate_boolean_or(const node::NodeBooleanOr* _or);
	void evaluate_boolean_and(const node::NodeBooleanAnd* _and);
	void evaluate_boolean_equal(const node::NodeBooleanEqual* equal);
	void evaluate_real_expr(const node::NodeBooleanRealExpression* expr);
	void evaluate_not(const node::NodeBooleanNot* _not);
	void evaluate_bool_factor(const node::NodeBooleanFactor* factor);

	void evaluate_string_expression(const node::NodeStringExpr* expr);

	// Have a return type which is the size of the array
	double evaluate_number_array(const node::NodeNumberArrayDecl* arr);
	double evaluate_string_array(const node::NodeStringArrayDecl* arr);
	double evaluate_boolean_array(const node::NodeBooleanArrayDecl* arr);

	void evaluate_assignment(const node::NodeAssignment* assignment);

	size_t get_array_index(const node::NodeArithmeticExpr* expr);

private:
	node::NodeProg m_prog;

	SymbolTable m_symbolTable; // this is for the global scope EXCLUSIVELY
	
	std::stack<mpark::variant<double,
		std::string,
		bool,
		std::vector<mpark::variant<double, std::string, bool>>
		>> m_stack;
};

