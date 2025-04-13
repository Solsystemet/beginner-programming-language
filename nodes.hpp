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
	struct NodeBooleanOr;
	struct NodeBooleanAnd;
	struct NodeBooleanEqual;
	struct NodeRealExpression;
	struct NodeBooleanNot;
	struct NodeBooleanFactor;
	struct NodeBooleanLess;
	struct NodeBooleanGreater;
	struct NodeBooleanLessEqual;
	struct NodeBooleanGreaterEqual;
	struct NodeFunctionCall;
	struct NodeBooleanEqualIs;
	struct NodeBooleanEqualIsNot;
	struct NodeBooleanOrOperation;
	struct NodeBooleanAndOperation;

	struct NodeBooleanExpr {
		NodeBooleanOr* expr;
	};

	struct NodeBooleanOrOperation {
		NodeBooleanOr* lhs;
		Token op;
		NodeBooleanAnd* rhs;
	};

	struct NodeBooleanOr {
		mpark::variant<NodeBooleanAnd*, NodeBooleanOrOperation*> var;
	};

	struct NodeBooleanAndOperation {
		NodeBooleanAnd* lhs;
		Token op;
		NodeBooleanEqual* rhs;
	};

	struct NodeBooleanAnd {
		mpark::variant<NodeBooleanEqual*, NodeBooleanAndOperation*> var;
	};

	struct NodeBooleanEqualIs {
		NodeBooleanEqual* lhs;
		Token is_token;
		NodeRealExpression* rhs;
	};

	struct NodenBooleanEqualIsNot {
		NodeBooleanEqual* lhs;
		Token is_token;
		Token not_token;
		NodeRealExpression* rhs;
	};

	struct NodeBooleanEqual {
		mpark::variant<NodeRealExpression*, NodeBooleanEqualIs*, NodeBooleanEqualIsNot*> var;
	};

	struct NodeRealExpression {
		mpark::variant<
			NodeBooleanNot*,
			NodeBooleanLess*,
			NodeBooleanGreater*,
			NodeBooleanLessEqual*,
			NodeBooleanGreaterEqual*
		> var;
	};
	
	struct NodeBooleanNotOperation {
		Token not_token;
		NodeBooleanNot* expr;
	};

	struct NodeBooleanNot {
		mpark::variant<NodeBooleanFactor*, NodeBooleanNotOperation*> var;
	};

	struct NodeBoolValue {
		Token boolval;
	};

	struct NodeIdentifier {
		Token identifier;
	};

	struct NodeBooleanFactor {
		mpark::variant<NodeBoolValue, NodeIdentifier, NodeBooleanExpr*> var;
	};

	struct NodeBooleanLess {
		NodeArithmeticExpr* lhs;
		Token less_op;          
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanGreater {
		NodeArithmeticExpr* lhs;
		Token greater_op;       
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanLessEqual {
		NodeArithmeticExpr* lhs;
		Token less_equal_op;     
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanGreaterEqual {
		NodeArithmeticExpr* lhs;
		Token greater_equal_op;  
		NodeArithmeticExpr* rhs;
	};

	struct NodeProg
	{
		std::vector<NodeStmt*> stmts;
	};
}