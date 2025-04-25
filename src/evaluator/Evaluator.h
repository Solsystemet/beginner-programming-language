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

private:
	node::NodeProg m_prog;

	SymbolTable m_symbolTable; // this is for the global scope EXCLUSIVELY
	
	std::stack<mpark::variant<double, std::string, bool>> m_stack;
};

