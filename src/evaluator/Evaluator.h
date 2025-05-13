#pragma once
#include <unordered_map>
#include <stack>
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include "../mpark/variant.hpp"
#include "../nodes.hpp"
#include "./SymbolTable.h"
#include "./FunctionTable.h"
#include "StructTable.h"



class Evaluator
{
	
public:
	Evaluator(node::NodeProg prog):
		m_prog(std::move(prog)) {}

	void evaluate_prog();

	void evaluate_stmt(const node::NodeStmt* stmt);
	void evaluate_nested_stmt(const node::NodeNestedStmt* stmt, bool* _break);

	void evaluate_print(const node::NodeStmtPrint* print_stmt);

	void evaluate_declecration(const node::NodeDecl* decl);
	void evaluate_object_declare(const node::NodeObjectDecl* decl);
	void evaluate_object_declare_to_stack(const node::NodeObjectDecl* decl);
	void evaluate_simple_decleration(const node::NodeSimpleDecl* simp_decl);

	void evaluate_object_declecration(const node::NodeDecl* decl, SymbolTable* table);
	void evaluate_simple_object_decleration(const node::NodeSimpleDecl* simp_decl, SymbolTable* table);

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
	double evaluate_object_array(const node::NodeObjectArrayDecl* arr);

	void evaluate_assignment(const node::NodeAssignment* assignment);
	void evaluate_assignment_object(const node::NodeAssignment* assignment, SymbolTable* table);

	void evaluate_definition(const node::NodeDefinition* definition);
	void evaluate_function_definition(const node::NodeFunctionDefinition* func_def);
	void evaluate_object_definition(const node::NodeObjectDefinition* obj_def);

	void evaluate_function_call(const node::NodeFunctionCall* func_call);

	void evaluate_value(const node::NodeValue* val);

	void evaluate_identifier_property(const node::NodeValueIdentifierProperty* props);
	void evaluate_function_call_property(const node::NodeValueFunctionCallProperty* props);

	void evaluate_value_object(const node::NodeValue* val, SymbolTable* table);

	void evaluate_function_stmt(const node::NodeFunctionStmt* stmt, Function* func, bool* _break);

	void evaluate_global_control_flow(const node::NodeGlobalControlFlow* flow);

	void evaluate_global_loop(const node::NodeGlobalLoop* loop);
	void evaluate_global_while(const node::NodeGlobalWhile* _while);
	void evaluate_global_for(const node::NodeGlobalFor* _for);

	void evaluate_function_control_flow(const node::NodeFunctionControlFlow* flow, Function* func, bool* _break);

	void evaluate_function_loop(const node::NodeFunctionLoop* loop, Function* func, bool* _break);
	void evaluate_function_while(const node::NodeFunctionWhile* _while, Function* func, bool* _break);
	void evaluate_function_for(const node::NodeFunctionFor* _for, Function* func, bool* _break);

	size_t get_array_index(const node::NodeArithmeticExpr* expr);


	void assign_function_args(Function* func, const node::NodeFunctionCall* func_call);
	std::string get_type(const Token type);

//private:
	node::NodeProg m_prog;

	SymbolTable m_symbolTable; // this is for the global scope EXCLUSIVELY
	FunctionTable m_functionTable;
	StructTable m_structDefinitionTable; // includes objects definitions
	
	std::vector<SymbolTable> m_scopedTables;

	std::stack<mpark::variant<double,
		std::string,
		bool,
		Struct,
		std::vector<mpark::variant<double, std::string, bool, Struct>>
		>> m_stack;
};

