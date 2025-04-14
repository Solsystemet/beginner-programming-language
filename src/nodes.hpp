#pragma once
#include "./lexer/tokens.h"
#include "./mpark/variant.hpp"
#include <iostream>
#include <vector>

namespace node {

	// Prog node rules 
	struct NodeProg;
	struct NodeStmt;

	//Arithmetic expression rules
	struct NodeArithmeticExpr;
	struct NodeTerm;
	struct NodeExprAdd;
	struct NodeExprSubtract;
	struct NodeFactor;
	struct NodeExprMult;
	struct NodeExprDivide;
	struct NodeExprModulo;
	struct NodeFactorDecimal;
	struct NodeFactorIdentifier;
	// Missing function call for arithmetic expressions

	// Boolean expression rules
	struct NodeBooleanExpr;
	struct NodeBooleanOr;
	struct NodeBooleanAnd;
	struct NodeBooleanEqual;
	struct NodeBooleanRealExpression;
	struct NodeBooleanNot;
	struct NodeBooleanFactor;
	struct NodeBooleanFactorValue;
	struct NodeBooleanFactorIdentifier;
	struct NodeBooleanLess;
	struct NodeBooleanGreater;
	struct NodeBooleanLessEqual;
	struct NodeBooleanGreaterEqual;
	struct NodeFunctionCall;
	struct NodeBooleanEqualIs;
	struct NodeBooleanEqualIsNot;
	struct NodeBooleanOrOperation;
	struct NodeBooleanAndOperation;

	struct NodeStringExpr;

	struct NodeProg
	{
		std::vector<NodeStmt*> stmts;
	};

	// Statement rules
	struct NodeDecl;
	struct NodeStmtPrint; // special function call stmt

	struct NodeStmt
	{
		mpark::variant<NodeDecl*, NodeStmtPrint*> var;
	};

	struct NodeStmtPrint {
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*> var;
	};

	// Decleration rules
	struct NodeSimpleDecl;
	struct NodeObjectDecl;

	struct NodeDecl {
		mpark::variant<NodeSimpleDecl*, NodeObjectDecl*> var;
	};

	struct NodeSimpleDecl {
		Token identifier;
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*, NodeBooleanExpr*> expr;
	};

	struct NodeObjectDecl {
		Token identifier;
		std::vector<NodeSimpleDecl*> properties;
	};

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

	struct NodeBooleanExpr {
		NodeBooleanOr* expr;
	};

	struct NodeBooleanOrOperation {
		NodeBooleanOr* lhs;
		NodeBooleanAnd* rhs;
	};

	struct NodeBooleanOr {
		mpark::variant<NodeBooleanAnd*, NodeBooleanOrOperation*> var;
	};

	struct NodeBooleanAndOperation {
		NodeBooleanAnd* lhs;
		NodeBooleanEqual* rhs;
	};

	struct NodeBooleanAnd {
		mpark::variant<NodeBooleanEqual*, NodeBooleanAndOperation*> var;
	};

	struct NodeBooleanEqualIs {
		NodeBooleanEqual* lhs;
		NodeBooleanRealExpression* rhs;
	};

	struct NodeBooleanEqualIsNot {
		NodeBooleanEqual* lhs;
		NodeBooleanRealExpression* rhs;
	};

	struct NodeBooleanEqual {
		mpark::variant<NodeBooleanRealExpression*, NodeBooleanEqualIs*, NodeBooleanEqualIsNot*> var;
	};

	struct NodeBooleanRealExpression {
		mpark::variant<
			NodeBooleanNot*,
			NodeBooleanLess*,
			NodeBooleanGreater*,
			NodeBooleanLessEqual*,
			NodeBooleanGreaterEqual*
		> var;
	};

	struct NodeBooleanNotOperation {
		NodeBooleanNot* expr;
	};

	struct NodeBooleanNot {
		mpark::variant<NodeBooleanFactor*, NodeBooleanNotOperation*> var;
	};

	struct NodeBooleanFactorValue {
		Token boolval;
	};

	struct NodeBooleanFactorIdentifier {
		Token identifier;
	};

	struct NodeBooleanFactor {
		mpark::variant<NodeBooleanFactorValue*, NodeBooleanFactorIdentifier*, NodeBooleanExpr*> var;
	};

	struct NodeBooleanLess {
		NodeArithmeticExpr* lhs;
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanGreater {
		NodeArithmeticExpr* lhs;
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanLessEqual {
		NodeArithmeticExpr* lhs;
		NodeArithmeticExpr* rhs;
	};

	struct NodeBooleanGreaterEqual {
		NodeArithmeticExpr* lhs;
		NodeArithmeticExpr* rhs;
	};
}