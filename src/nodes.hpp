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
	
	struct NodeFunctionCall;
	// for function call arguments
	struct NodeArgs;
	
	// node value rules
	struct NodeValue;
	struct NodeValueIdentifier;
	struct NodeValueFunctionCall;
	struct NodeValueArithmeticExpression;
	struct NodeValueStringExpression;
	struct NodeValueBooleanExpression;
	struct NodeValueIdentifierProperty;
	struct NodeValueFunctionCallProperty;


	struct NodeStmt
	{
		mpark::variant<NodeDecl*,NodeFunctionCall*, NodeStmtPrint*> var;
	};

	struct NodeStmtPrint {
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*> var;
	};

	struct NodeFunctionCall {
		Token functionName;
		std::vector<NodeArgs*> args;
	};

	struct NodeArgs
	{
		NodeValue* value;
	};

	struct NodeValue
	{
		mpark::variant<
			NodeValueIdentifier*,
			NodeValueFunctionCall*,
			NodeValueArithmeticExpression*,
			NodeValueStringExpression*,
			NodeValueBooleanExpression*,
			NodeValueIdentifierProperty*,
			NodeValueFunctionCallProperty*> var;
	};

	struct NodeValueIdentifier {
		Token identifier;
	};
	struct NodeValueFunctionCall
	{
		NodeFunctionCall* functionCall;
	};
	struct NodeValueArithmeticExpression
	{
		NodeArithmeticExpr* expr;
	};
	struct NodeValueStringExpression
	{
		NodeStringExpr* expr;
	};
	struct NodeValueBooleanExpression
	{
		NodeBooleanExpr* expr;
	};
	struct NodeValueIdentifierProperty
	{
		Token identfierHead;
		std::vector<Token> identifierproperties;
	};
	struct NodeValueFunctionCallProperty
	{
		NodeFunctionCall* functionCall;
		NodeValueIdentifierProperty* identifierproperties;
	};
	// Decleration rules
	struct NodeSimpleDecl;
	struct NodeArrayDecl;
	struct NodeObjectDecl;

	//array declerations
	struct NodeNumberArrayDecl;
	struct NodeStringArrayDecl;
	struct NodeBooleanArrayDecl;
	struct NodeObjectArrayDecl;

	struct NodeDecl {
		mpark::variant<NodeSimpleDecl*, NodeObjectDecl*, NodeArrayDecl*> var;
	};

	struct NodeSimpleDecl {
		Token identifier;
		mpark::variant<NodeArithmeticExpr*, NodeStringExpr*, NodeBooleanExpr*> expr;
	};

	struct NodeArrayDecl {
		Token identifier;
		mpark::variant<NodeNumberArrayDecl*, NodeStringArrayDecl*, NodeBooleanArrayDecl*, NodeObjectArrayDecl*> var;
	};

	struct NodeNumberArrayDecl
	{
		std::vector<NodeArithmeticExpr*> elements;
	};

	struct NodeStringArrayDecl
	{
		std::vector<NodeStringExpr*> elements;
	};
	struct NodeBooleanArrayDecl
	{
		std::vector<NodeBooleanExpr*> elements;
	};
	// sus
	struct NodeObjectArrayDecl
	{
		std::vector<NodeObjectDecl*> elements;
	};

	struct NodeObjectDecl {
		Token objectType;
		Token identifier;
		std::vector<NodeDecl*> properties;
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
		mpark::variant<NodeFactorDecimal*, NodeFactorIdentifier*, NodeArithmeticExpr*, NodeFunctionCall*> var;
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
		mpark::variant<Token, NodeStringExprConcat*, NodeFunctionCall*> var;
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
		mpark::variant<NodeBooleanFactorValue*, NodeBooleanFactorIdentifier*, NodeBooleanExpr*, NodeFunctionCall*> var;
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