#pragma once
#include "./lexer/tokens.h"
#include "./mpark/variant.hpp"
#include <iostream>
#include <vector>

namespace node {

	// Prog node rules 
	struct NodeProg;
	struct NodeStmt;
	struct NodeNestedStmt;
	struct NodeFunctionStmt;

	struct NodeFunctionReturn;

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

	// node assigment rules
	struct NodeAssignment;
	struct NodeAssignmentFunctionCall;
	struct NodeAssignmentIdentifier;
	struct NodeAssignmentArithmeticExpression;
	struct NodeAssignmentBooleanExpression;
	struct NodeAssignmentStringExpression;
	struct NodeAssignmentIdentifierProperties;

	// node global control flow rules
	struct NodeGlobalControlFlow;
	struct NodeGlobalIf;
	struct NodeGlobalElseIf;
	struct NodeGlobalElse;
	struct NodeGlobalLoop;
	struct NodeGlobalWhile;
	struct NodeGlobalFor;

	// node function control flow rules
	struct NodeFunctionControlFlow;
	struct NodeFunctionIf;
	struct NodeFunctionElseIf;
	struct NodeFunctionElse;
	struct NodeFunctionLoop;
	struct NodeFunctionWhile;
	struct NodeFunctionFor;

	// node definition rules
	struct NodeDefinition;
	struct NodeFunctionDefinition;
	struct NodeObjectDefinition;

	struct NodeFunctionArg;


	struct NodeStmt
	{
		mpark::variant<NodeDecl*,NodeFunctionCall*,NodeAssignment*,
			NodeGlobalControlFlow*, NodeStmtPrint*, NodeDefinition*> var;
	};

	struct NodeNestedStmt
	{
		mpark::variant<NodeDecl*, NodeFunctionCall*, NodeAssignment*,
			NodeGlobalControlFlow*, NodeStmtPrint*> var;
	};

	struct NodeFunctionStmt
	{
		mpark::variant<NodeDecl*, NodeFunctionCall*, NodeAssignment*,
			NodeFunctionControlFlow*, NodeFunctionReturn*, NodeStmtPrint*> var;
	};

	struct NodeFunctionReturn {
		NodeValue* val = nullptr;
	};

	struct NodeStmtPrint {
		//TODO: make it handle conantations like "I am " + 20 + " years old\n"
		mpark::variant<NodeValue*> value;
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

		//if node value is from an array
		NodeArithmeticExpr* index = nullptr;
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

	struct NodeAssignment {
		Token identifierHead;

		// if variable is an array index will be overwritten
		NodeArithmeticExpr* index = nullptr;

		NodeValue* rhs;

		// represent <identifier>.<Identifier Property>
		std::vector<Token> props;
	};

	struct NodeAssignmentIdentifier
	{
		Token identifier;
	};

	struct NodeAssignmentFunctionCall
	{
		NodeFunctionCall* functioncall;
	};

	struct NodeAssignmentIdentifierProperties
	{
		Token identifierHead;
		std::vector<Token> props;
	};

	struct NodeAssignmentArithmeticExpression
	{
		NodeArithmeticExpr* expr;
	};

	struct NodeAssignmentBooleanExpression
	{
		NodeArithmeticExpr* expr;
	};

	struct NodeAssignmentStringExpression
	{
		NodeArithmeticExpr* expr;
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
		Token type;
		Token identifier;
		mpark::variant<NodeNumberArrayDecl*, NodeStringArrayDecl*, NodeBooleanArrayDecl*, NodeObjectArrayDecl*> var;
	};

	struct NodeNumberArrayDecl
	{
		mpark::variant<NodeArithmeticExpr*, size_t> size;
		std::vector<NodeArithmeticExpr*> elements;
	};

	struct NodeStringArrayDecl
	{
		mpark::variant<NodeArithmeticExpr*, size_t> size;
		std::vector<NodeStringExpr*> elements;
	};
	struct NodeBooleanArrayDecl
	{
		mpark::variant<NodeArithmeticExpr*, size_t> size;
		std::vector<NodeBooleanExpr*> elements;
	};
	// sus
	struct NodeObjectArrayDecl
	{
		mpark::variant<NodeArithmeticExpr*, size_t> size;
		std::vector<NodeObjectDecl*> elements;
	};

	struct NodeObjectDecl {
		Token objectType;
		Token identifier;
		std::vector<NodeAssignment*> properties;
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

	struct NodeStringValue {
		Token value;
	};

	struct NodeStringIdentifier {
		Token ident;
	};

	struct NodeStringExpr {
		mpark::variant<NodeStringValue*, NodeStringIdentifier*, NodeFunctionCall* , NodeStringExprConcat*> var;
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
		mpark::variant<NodeBooleanOr*> expr;
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

	struct NodeGlobalControlFlow {
		mpark::variant<NodeGlobalIf*, NodeGlobalLoop*> var;
	};

	struct NodeGlobalIf {
		NodeBooleanExpr* condition;
		std::vector<NodeNestedStmt*> stmts;

		std::vector<NodeGlobalElseIf*> elseifs;

		// assumes else if not there
		NodeGlobalElse* _else = nullptr;
	};

	struct NodeGlobalElseIf
	{
		NodeBooleanExpr* condition;
		std::vector<NodeNestedStmt*> stmts;
	};

	struct NodeGlobalElse {
		std::vector<NodeNestedStmt*> stmts;
	};

	struct NodeGlobalLoop {
		mpark::variant<NodeGlobalWhile*, NodeGlobalFor*> var;
	};

	struct NodeGlobalWhile {
		NodeBooleanExpr* condition;
		std::vector<NodeNestedStmt*> stmts;
	};

	struct NodeGlobalFor
	{
		Token indexValIdentifier;
		NodeArithmeticExpr* indexValExpr;
		NodeBooleanExpr* condition;
		NodeArithmeticExpr* increment;

		std::vector<NodeNestedStmt*> stmts;
	};



	struct NodeFunctionControlFlow {
		mpark::variant<NodeFunctionIf*, NodeFunctionLoop*> var;
	};

	struct NodeFunctionIf {
		NodeBooleanExpr* condition;
		std::vector<NodeFunctionStmt*> stmts;

		std::vector<NodeFunctionElseIf*> elseifs;

		// assumes else if not there
		NodeFunctionElse* _else = nullptr;
	};

	struct NodeFunctionElseIf
	{
		NodeBooleanExpr* condition;
		std::vector<NodeFunctionStmt*> stmts;
	};

	struct NodeFunctionElse {
		std::vector<NodeFunctionStmt*> stmts;
	};

	struct NodeFunctionLoop {
		mpark::variant<NodeFunctionWhile*, NodeFunctionFor*> var;
	};

	struct NodeFunctionWhile {
		NodeBooleanExpr* condition;
		std::vector<NodeFunctionStmt*> stmts;
	};

	struct NodeFunctionFor
	{
		Token indexValIdentifier;
		NodeArithmeticExpr* indexValExpr;
		NodeBooleanExpr* condition;
		NodeArithmeticExpr* increment;

		std::vector<NodeFunctionStmt*> stmts;
	};

	struct NodeDefinition {
		mpark::variant<NodeFunctionDefinition*, NodeObjectDefinition*> var;
	};

	struct NodeFunctionDefinition
	{
		// can either be number, string, boolean or identifier
		Token* type = nullptr;
		bool isAnArray = false;
		Token functionName;

		std::vector<NodeFunctionArg*> args;
		std::vector<NodeFunctionStmt*> stmts;
	};

	struct NodeFunctionArg
	{
		Token type;
		bool isTypeAnArray = false;
		Token identifier;
	};

	struct NodeObjectDefinition
	{
		Token identifier;
		std::vector<NodeDecl*> props;
	};




}