#pragma once
#include "src/lexer/tokens.h"
#include "src/mpark/variant.hpp"
#include <iostream>
#include <vector>

namespace node {
	struct NodeTerm;
	struct NodeArithmeticExpr;

	struct NodeFactorDecimal
	{
		Token decimal;
	};

	struct NodeFactorIdentifier
	{
		Token identifier;
	};

	struct NodeFactor
	{
		//TODO: Add function call production later
		mpark::variant<NodeFactorDecimal*, NodeFactorIdentifier*, NodeArithmeticExpr*> var;
	};

	struct  NodeExprAdd {
		NodeArithmeticExpr* lhs;
		NodeTerm* rhs;
	};
	struct  NodeExprSubtract {
		NodeArithmeticExpr* lhs;
		NodeTerm* rhs;
	};
	struct  NodeExprMult {
		NodeTerm* lhs;
		NodeFactor* rhs;
	};
	struct  NodeExprDivide {
		NodeTerm* lhs;
		NodeFactor* rhs;
	};
	struct NodeExprModulo {
		NodeTerm* lhs;
		NodeFactor* rhs;
	};

	struct NodeStringExpr;
	struct NodeStringExprConcat {
		NodeStringExpr* lhs;
		NodeStringExpr* rhs;
	};

	struct NodeStringExpr {
		mpark::variant<Token, NodeStringExprConcat*> var;
	};

	struct NodeArithmeticExpr
	{
		mpark::variant<NodeTerm*, NodeExprAdd*, NodeExprSubtract*> var;
	};

	//Identifier or Decimal can be passed to this struct
	struct NodeTerm {
		mpark::variant<NodeFactor*, NodeExprMult*, NodeExprDivide*, NodeExprModulo*> var;
	};

	struct NodeSimpleDecl {
		Token identifier;
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*> expr;
	};

	struct NodeObjectDecl {
		Token identifier;
		std::vector<NodeSimpleDecl*> properties;
	};

	struct NodeDecl {
		mpark::variant<NodeSimpleDecl*, NodeObjectDecl*> var;
	};

	struct NodeStmtPrint {
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*> var;
	};

	struct NodeStmt
	{
		mpark::variant<NodeDecl*, NodeStmtPrint*> var;
	};

	struct NodeProg
	{
		std::vector<NodeStmt*> stmts;
	};
}