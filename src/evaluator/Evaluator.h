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
#include "../errorHandling/errorHandling.h"



class Evaluator
{
	
public:
	Evaluator(node::NodeProg prog):
		m_prog(std::move(prog)) {}

	void evaluate_prog();
	
	inline void evaluate_stmt(const node::NodeStmt* stmt);
	inline void evaluate_nested_stmt(const node::NodeNestedStmt* stmt, bool* _break);
	inline void evaluate_loop_stmt(const node::NodeLoopStmt* stmt, bool* _break, bool* _continue);
	
	inline void evaluate_print(const node::NodeStmtPrint* print_stmt);
	
	inline void evaluate_declecration(const node::NodeDecl* decl);
	inline void evaluate_object_declare(const node::NodeObjectDecl* decl);
	inline void evaluate_object_declare_to_stack(const node::NodeObjectDecl* decl);
	inline void evaluate_simple_decleration(const node::NodeSimpleDecl* simp_decl);
	 
	inline void evaluate_object_declecration(const node::NodeDecl* decl, SymbolTable* table);
	inline void evaluate_simple_object_decleration(const node::NodeSimpleDecl* simp_decl, SymbolTable* table);
	
	inline void evaluate_arithmetic_expression(const node::NodeArithmeticExpr* expr);
	inline void evaluate_term(const node::NodeTerm* term);
	inline void evaluate_factor(const node::NodeFactor* factor);
	
	inline void evaluate_boolean_expression(const node::NodeBooleanExpr* expr);
	inline void evaluate_boolean_or(const node::NodeBooleanOr* _or);
	inline void evaluate_boolean_and(const node::NodeBooleanAnd* _and);
	inline void evaluate_boolean_equal(const node::NodeBooleanEqual* equal);
	inline void evaluate_real_expr(const node::NodeBooleanRealExpression* expr);
	inline void evaluate_not(const node::NodeBooleanNot* _not);
	inline void evaluate_bool_factor(const node::NodeBooleanFactor* factor);
	
	inline void evaluate_string_expression(const node::NodeStringExpr* expr);
	
	// Have a return type which is the size of the array
	inline double evaluate_number_array(const node::NodeNumberArrayDecl* arr);
	inline double evaluate_string_array(const node::NodeStringArrayDecl* arr);
	inline double evaluate_boolean_array(const node::NodeBooleanArrayDecl* arr);
	inline double evaluate_object_array(const node::NodeObjectArrayDecl* arr);
	
	inline void evaluate_assignment(const node::NodeAssignment* assignment);
	inline void evaluate_assignment_object(const node::NodeAssignment* assignment, SymbolTable* table);
	
	inline void evaluate_definition(const node::NodeDefinition* definition);
	inline void evaluate_function_definition(const node::NodeFunctionDefinition* func_def);
	inline void evaluate_object_definition(const node::NodeObjectDefinition* obj_def);
	
	inline void evaluate_function_call(const node::NodeFunctionCall* func_call);
	
	inline void evaluate_value(const node::NodeValue* val);
	
	inline void evaluate_identifier_property(const node::NodeValueIdentifierProperty* props);
	inline void evaluate_function_call_property(const node::NodeValueFunctionCallProperty* props);
	
	inline void evaluate_value_object(const node::NodeValue* val, SymbolTable* table);
	
	inline void evaluate_function_stmt(const node::NodeFunctionStmt* stmt, Function* func, bool* _break);
	inline void evaluate_function_loop_stmt(const node::NodeFunctionLoopStmt* stmt, Function* func, bool* _break, bool* _continue);
	
	inline void evaluate_global_control_flow(const node::NodeGlobalControlFlow* flow);
	inline void evaluate_global_loop_control_flow(const node::NodeGlobalLoopControlFlow* flow, bool* _break, bool* _continue);
	
	inline void evaluate_global_loop(const node::NodeGlobalLoop* loop);
	inline void evaluate_global_while(const node::NodeGlobalWhile* _while);
	inline void evaluate_global_for(const node::NodeGlobalFor* _for);
	
	inline void evaluate_function_control_flow(const node::NodeFunctionControlFlow* flow, Function* func, bool* _break);
	inline void evaluate_function_loop_control_flow(const node::NodeFunctionLoopControlFlow* flow, Function* func, bool* _break, bool* _continue);
	
	inline void evaluate_function_loop(const node::NodeFunctionLoop* loop, Function* func, bool* _break);
	inline void evaluate_function_while(const node::NodeFunctionWhile* _while, Function* func, bool* _break);
	inline void evaluate_function_for(const node::NodeFunctionFor* _for, Function* func, bool* _break);
	
	inline size_t get_array_index(const node::NodeArithmeticExpr* expr);
	
	
	inline void assign_function_args(Function* func, const node::NodeFunctionCall* func_call);
	inline std::string get_type(const Token type);

//private:
	template<typename T>
	inline bool type_check(Symbol lhs, std::string lhs_type, mpark::variant<double,
		std::string,
		bool,
		Struct,
		std::vector<mpark::variant<double, std::string, bool, Struct>>> rhs) {
		if (lhs.type == lhs_type && mpark::holds_alternative<T>(rhs))
			return true;
		else
			return false;
	}

	template<typename T>
	inline bool type_check_arr(Symbol lhs, std::string lhs_type, 
		mpark::variant<double, std::string, bool, Struct> rhs) {
		if (lhs.type == lhs_type && mpark::holds_alternative<T>(rhs))
			return true;
		else
			return false;
	}

	inline std::string variant_to_type(mpark::variant<double,
		std::string,
		bool,
		Struct,
		std::vector<mpark::variant<double, std::string, bool, Struct>>
	> var) 
	{
		std::string* result = new std::string();
		struct typeVisitor
		{
			std::string* output;
			void operator()(const double) {
				*output = "number";
			}
			void operator()(const std::string) {
				*output = "string";
			}
			void operator()(const bool) {
				*output = "boolean";
			}

			void operator()(const Struct) {
				*output = "object";
			}
			void operator()(const std::vector<mpark::variant<double, std::string, bool, Struct>> arr) {
				struct typeArrVisitor
				{
					std::string* arr_output;
					void operator()(const double) {
						*arr_output = "number";
					}
					void operator()(const std::string) {
						*arr_output = "string";
					}
					void operator()(const bool) {
						*arr_output = "boolean";
					}
					void operator()(const Struct) {
						*arr_output = "object";
					}
				};
				mpark::visit(typeVisitor{ output }, arr[0]);
			}
		};
		mpark::visit(typeVisitor{ result }, var);
		return *result;
	}

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

