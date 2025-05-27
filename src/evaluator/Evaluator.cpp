#include "Evaluator.h"

void Evaluator::evaluate_prog()
{
	for (const node::NodeStmt* stmt : m_prog.stmts) {
		
		evaluate_stmt(stmt);
	}
}

void Evaluator::evaluate_stmt(const node::NodeStmt* stmt)
{
	struct StmtVisitor
	{
		Evaluator* evaluator;
		
		// decleration
		void operator()(const node::NodeDecl* decl) const {
			
			evaluator->evaluate_declecration(decl);
		}

		// function call
		void operator()(const node::NodeFunctionCall* stmt_function_call) const {
			evaluator->evaluate_function_call(stmt_function_call);
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// global control flow
		void operator()(const node::NodeGlobalControlFlow* global_control_flow) const {
			evaluator->evaluate_global_control_flow(global_control_flow);
		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// definition
		void operator()(const node::NodeDefinition* definition) const {
			evaluator->evaluate_definition(definition);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}

	};
	mpark::visit(StmtVisitor{this}, stmt->var);
}

void Evaluator::evaluate_nested_stmt(const node::NodeNestedStmt* stmt, bool* _break)
{
	struct StmtVisitor
	{
		Evaluator* evaluator;
		bool* _break;

		// decleration
		void operator()(const node::NodeDecl* decl) const {

			evaluator->evaluate_declecration(decl);
		}

		// function call
		void operator()(const node::NodeFunctionCall* stmt_function_call) const {
			evaluator->evaluate_function_call(stmt_function_call);
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// global control flow
		void operator()(const node::NodeGlobalControlFlow* global_control_flow) const {
			evaluator->evaluate_global_control_flow(global_control_flow);
		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}

	};
	mpark::visit(StmtVisitor{ this, _break }, stmt->var);
}

inline void Evaluator::evaluate_loop_stmt(const node::NodeLoopStmt* stmt, bool* _break, bool* _continue)
{
	struct StmtVisitor
	{
		Evaluator* evaluator;
		bool* _break;
		bool* _continue;

		// decleration
		void operator()(const node::NodeDecl* decl) const {

			evaluator->evaluate_declecration(decl);
		}

		// function call
		void operator()(const node::NodeFunctionCall* stmt_function_call) const {
			evaluator->evaluate_function_call(stmt_function_call);
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// global control flow
		void operator()(const node::NodeGlobalLoopControlFlow* global_control_flow) const {
			evaluator->evaluate_global_loop_control_flow(global_control_flow, _break, _continue);
		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}

		// break
		void operator()(const node::NodeBreak* _) const {
			*_break = true;
		}

		// continue
		void operator()(const node::NodeContinue* _) const {
			*_continue = true;
		}

	};
	mpark::visit(StmtVisitor{ this, _break, _continue }, stmt->var);
}

void Evaluator::evaluate_print(const node::NodeStmtPrint* print_stmt)
{
	struct PrintVisitor
	{
		Evaluator* evaluator;

		// node value
		void operator()(const node::NodeValue* val) const {
			evaluator->evaluate_value(val);

			auto result = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			if (mpark::holds_alternative<double>(result)) {
				std::cout << mpark::get<double>(result);
			}
			else if (mpark::holds_alternative<bool>(result)) {
				std::cout << mpark::get<bool>(result);
			}
			else if (mpark::holds_alternative<std::string>(result)) {
				std::cout << mpark::get<std::string>(result);
			}
		}

	}; mpark::visit(PrintVisitor{ this }, print_stmt->value);
}

void Evaluator::evaluate_declecration(const node::NodeDecl* decl)
{
	struct DeclVisitor {
		Evaluator* evaluator;
		// simple declare
		void operator()(const node::NodeSimpleDecl* simp_decl) const {
			evaluator->m_stack.push(simp_decl->identifier.value);
			evaluator->evaluate_simple_decleration(simp_decl);
		}

		// object declare
		void operator()(const node::NodeObjectDecl* obj_decl) const {
			evaluator->evaluate_object_declare(obj_decl);
		}

		// array declare
		void operator()(const node::NodeArrayDecl* arr_decl) const {
			if (arr_decl->type.type == NUMBER && 
				mpark::holds_alternative<node::NodeNumberArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_number_array(mpark::get<node::NodeNumberArrayDecl*>(arr_decl->var));
				

				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "number";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back(0.0);
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize ;i++)
					{
						arr.push_back(mpark::get<double>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				symbol.value = arr;
				if (evaluator->m_scopedTables.size() > 0) {
					evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(symbol);
				}
				else
					evaluator->m_symbolTable.insert(symbol);
			}
			else if (arr_decl->type.type == STRING &&
				mpark::holds_alternative<node::NodeStringArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_string_array(mpark::get<node::NodeStringArrayDecl*>(arr_decl->var));


				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "string";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back("");
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize; i++)
					{
						arr.push_back(mpark::get<std::string>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				symbol.value = arr;
				if (evaluator->m_scopedTables.size() > 0) {
					evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(symbol);
				}
				else
					evaluator->m_symbolTable.insert(symbol);
			}
			else if (arr_decl->type.type == BOOLEAN &&
				mpark::holds_alternative<node::NodeBooleanArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_boolean_array(mpark::get<node::NodeBooleanArrayDecl*>(arr_decl->var));


				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "boolean";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back(false);
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize; i++)
					{
						arr.push_back(mpark::get<bool>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				symbol.value = arr;
				if (evaluator->m_scopedTables.size() > 0) {
					evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(symbol);
				}
				else
					evaluator->m_symbolTable.insert(symbol);
			}
			else if (arr_decl->type.type == IDENTIFIER &&
				mpark::holds_alternative<node::NodeObjectArrayDecl*>(arr_decl->var)){
				
					auto decl = mpark::get<node::NodeObjectArrayDecl*>(arr_decl->var);
					if (evaluator->m_structDefinitionTable.contains(decl->objectType.value)) {

						double arraySize = evaluator->evaluate_object_array(decl);


						Symbol symbol;
						symbol.name = arr_decl->identifier.value;
						symbol.type = "object";
						symbol.isAnArray = true;

						// Make array into correct size if it is assigned fixed size
						std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
						while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
							Struct obj = *evaluator->m_structDefinitionTable.lookup(decl->objectType.value);
							arr.push_back(obj);
						}
						// populate array if it was assigned elements
						if (evaluator->m_stack.size() > 0) {
							//populate backwards
							for (size_t i = 0; i < arraySize; i++)
							{
								arr.push_back(mpark::get<Struct>(evaluator->m_stack.top()));
								evaluator->m_stack.pop();
							}
						}
						std::reverse(arr.begin(), arr.end());
						symbol.value = arr;
						if (evaluator->m_scopedTables.size() > 0) {
							evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(symbol);
						}
						else
							evaluator->m_symbolTable.insert(symbol);
					}
					else {
						std::cerr << "Object of type: " << decl->objectType.value << " has no definition!!" << std::endl;
						exit(EXIT_FAILURE);
					}

			}
			else {
				std::cerr << "HOW TF";
				exit(EXIT_FAILURE);
			}
		}
	};
	mpark::visit(DeclVisitor{ this }, decl->var);
}
void Evaluator::evaluate_object_declare(const node::NodeObjectDecl* decl)
{
	if (m_structDefinitionTable.contains(decl->objectType.value)) {
		Struct obj_def = *m_structDefinitionTable.lookup(decl->objectType.value);
		Struct obj;
		obj.name = obj_def.name;
		obj.table = new SymbolTable();
		*obj.table = *obj_def.table;
		for (node::NodeAssignment* ass : decl->properties) {
			evaluate_assignment_object(ass, obj.table);
		}

		Symbol obj_symbol;
		obj_symbol.name = decl->identifier.value;
		obj_symbol.type = "object";
		obj_symbol.value = obj;
		if (m_scopedTables.size() > 0) {
			m_scopedTables.at(m_scopedTables.size() - 1).insert(obj_symbol);
		}
		else
			m_symbolTable.insert(obj_symbol);
	}
	else {
		std::cerr << "Object of type: " << decl->objectType.value << " has no definition!!" << std::endl;
		exit(EXIT_FAILURE);
	}
}
void Evaluator::evaluate_object_declare_to_stack(const node::NodeObjectDecl* decl)
{
	if (m_structDefinitionTable.contains(decl->objectType.value)) {
		Struct obj = *m_structDefinitionTable.lookup(decl->objectType.value);

		for (node::NodeAssignment* ass : decl->properties) {
			evaluate_assignment_object(ass, obj.table);
		}

		m_stack.push(obj);
	}
	else {
		std::cerr << "Object of type: " << decl->objectType.value << " has no definition!!" << std::endl;
		exit(EXIT_FAILURE);
	}
}
void Evaluator::evaluate_simple_decleration(const node::NodeSimpleDecl* simp_decl)
{
	struct SimpDeclVisitor {
		Evaluator* evaluator;

		// arithmetic expression
		void operator()(const node::NodeArithmeticExpr* a_expr) const {
			Symbol number;
			number.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			number.type = "number";
			evaluator->evaluate_arithmetic_expression(a_expr);
			number.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			if (evaluator->m_scopedTables.size() > 0) {
				evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(number);
			}
			else
				evaluator->m_symbolTable.insert(number); // number decleration inserted into symbol table
		}

		// boolean expression
		void operator()(const node::NodeBooleanExpr* b_expr) const {
			Symbol boolean;
			boolean.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			boolean.type = "boolean";
			evaluator->evaluate_boolean_expression(b_expr);
			boolean.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			if (evaluator->m_scopedTables.size() > 0) {
				evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(boolean);
			}
			else
				evaluator->m_symbolTable.insert(boolean); // boolean decleration inserted into symbol table
		}

		// string expression
		void operator()(const node::NodeStringExpr* s_expr) const {
			Symbol string;
			string.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			string.type = "string";
			evaluator->evaluate_string_expression(s_expr);
			string.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			if (evaluator->m_scopedTables.size() > 0) {
				evaluator->m_scopedTables.at(evaluator->m_scopedTables.size() - 1).insert(string);
			}
			else
				evaluator->m_symbolTable.insert(string); // string decleration inserted into symbol table
		}

	};
	mpark::visit(SimpDeclVisitor{ this }, simp_decl->expr);
}

void Evaluator::evaluate_object_declecration(const node::NodeDecl* decl, SymbolTable* table)
{
	struct DeclVisitor {
		Evaluator* evaluator;
		SymbolTable* table;
		// simple declare
		void operator()(const node::NodeSimpleDecl* simp_decl) const {
			evaluator->m_stack.push(simp_decl->identifier.value);
			evaluator->evaluate_simple_object_decleration(simp_decl, table);
		}

		// object declare
		void operator()(const node::NodeObjectDecl* obj_decl) const {
			if (evaluator->m_structDefinitionTable.contains(obj_decl->objectType.value)) {
				Struct copy = *evaluator->m_structDefinitionTable.lookup(obj_decl->objectType.value);

				for (node::NodeAssignment* ass : obj_decl->properties) {
					evaluator->evaluate_assignment_object(ass, copy.table);
				}

				Symbol symbol;
				symbol.name = obj_decl->identifier.value;
				symbol.type = "object";
				symbol.value = copy;
				table->insert(symbol);
			}
		}

		// array declare
		void operator()(const node::NodeArrayDecl* arr_decl) const {
			if (arr_decl->type.type == NUMBER &&
				mpark::holds_alternative<node::NodeNumberArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_number_array(mpark::get<node::NodeNumberArrayDecl*>(arr_decl->var));


				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "number";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back(0.0);
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize; i++)
					{
						arr.push_back(mpark::get<double>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				symbol.value = arr;
				table->insert(symbol);
			}
			else if (arr_decl->type.type == STRING &&
				mpark::holds_alternative<node::NodeStringArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_string_array(mpark::get<node::NodeStringArrayDecl*>(arr_decl->var));


				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "string";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back(0.0);
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize; i++)
					{
						arr.push_back(mpark::get<std::string>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				symbol.value = arr;
				table->insert(symbol);
			}
			else if (arr_decl->type.type == BOOLEAN &&
				mpark::holds_alternative<node::NodeBooleanArrayDecl*>(arr_decl->var)) {
				double arraySize = evaluator->evaluate_boolean_array(mpark::get<node::NodeBooleanArrayDecl*>(arr_decl->var));


				Symbol symbol;
				symbol.name = arr_decl->identifier.value;
				symbol.type = "boolean";
				symbol.isAnArray = true;

				// Make array into correct size if it is assigned fixed size
				std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
				while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
					arr.push_back(0.0);
				}
				// populate array if it was assigned elements
				if (evaluator->m_stack.size() > 0) {
					//populate backwards
					for (size_t i = 0; i < arraySize; i++)
					{
						arr.push_back(mpark::get<bool>(evaluator->m_stack.top()));
						evaluator->m_stack.pop();
					}
				}
				std::reverse(arr.begin(), arr.end());
				for (mpark::variant<double, std::string, bool, Struct> val : arr) {
					std::cout << mpark::get<bool>(val);
				}
				symbol.value = arr;
				table->insert(symbol);
			}
			else if (arr_decl->type.type == IDENTIFIER &&
				mpark::holds_alternative<node::NodeObjectArrayDecl*>(arr_decl->var)) {

				auto decl = mpark::get<node::NodeObjectArrayDecl*>(arr_decl->var);
				if (evaluator->m_structDefinitionTable.contains(decl->objectType.value)) {

					double arraySize = evaluator->evaluate_object_array(decl);


					Symbol symbol;
					symbol.name = arr_decl->identifier.value;
					symbol.type = "object";
					symbol.isAnArray = true;

					// Make array into correct size if it is assigned fixed size
					std::vector<mpark::variant<double, std::string, bool, Struct>> arr;
					while (arr.size() < arraySize && evaluator->m_stack.size() == 0) {
						Struct obj = *evaluator->m_structDefinitionTable.lookup(decl->objectType.value);
						arr.push_back(obj);
					}
					// populate array if it was assigned elements
					if (evaluator->m_stack.size() > 0) {
						//populate backwards
						for (size_t i = 0; i < arraySize; i++)
						{
							arr.push_back(mpark::get<Struct>(evaluator->m_stack.top()));
							evaluator->m_stack.pop();
						}
					}
					std::reverse(arr.begin(), arr.end());
					symbol.value = arr;
					table->insert(symbol);
				}
				else {
					std::cerr << "Object of type: " << decl->objectType.value << " has no definition!!" << std::endl;
					exit(EXIT_FAILURE);
				}

			}
			else {
				std::cerr << "HOW TF";
				exit(EXIT_FAILURE);
			}
		}
	};
	mpark::visit(DeclVisitor{ this, table }, decl->var);
}

void Evaluator::evaluate_simple_object_decleration(const node::NodeSimpleDecl* simp_decl, SymbolTable* table)
{
	struct SimpDeclVisitor {
		Evaluator* evaluator;
		SymbolTable* table;

		// arithmetic expression
		void operator()(const node::NodeArithmeticExpr* a_expr) const {
			Symbol number;
			number.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			number.type = "number";
			evaluator->evaluate_arithmetic_expression(a_expr);
			number.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			table->insert(number); // number decleration inserted into symbol table
		}

		// boolean expression
		void operator()(const node::NodeBooleanExpr* b_expr) const {
			Symbol boolean;
			boolean.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			boolean.type = "boolean";
			evaluator->evaluate_boolean_expression(b_expr);
			boolean.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			table->insert(boolean); // boolean decleration inserted into symbol table
		}

		// string expression
		void operator()(const node::NodeStringExpr* s_expr) const {
			Symbol string;
			string.name = mpark::get<std::string>(evaluator->m_stack.top()); // gets identifier name
			evaluator->m_stack.pop();
			string.type = "string";
			evaluator->evaluate_string_expression(s_expr);
			string.value = evaluator->m_stack.top(); // get evaluated value from expression
			evaluator->m_stack.pop();
			table->insert(string); // string decleration inserted into symbol table
		}

	};
	mpark::visit(SimpDeclVisitor{ this, table }, simp_decl->expr);
}

void Evaluator::evaluate_arithmetic_expression(const node::NodeArithmeticExpr* expr)
{
	struct ArithmeticExprVisitor
	{
		Evaluator* evaluator;
		// term
		void operator()(const node::NodeTerm* term) const {
			evaluator->evaluate_term(term);
		}

		// add expr
		void operator()(const node::NodeExprAdd* add) const {
			evaluator->evaluate_arithmetic_expression(add->lhs);
			evaluator->evaluate_term(add->rhs);
			
			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs + rhs);

		}

		// subtract expr
		void operator()(const node::NodeExprSubtract* subtract) const {
			evaluator->evaluate_arithmetic_expression(subtract->lhs);
			evaluator->evaluate_term(subtract->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs - rhs);
		}
	};
	mpark::visit(ArithmeticExprVisitor{ this }, expr->var);
}

void Evaluator::evaluate_term(const node::NodeTerm* term)
{
	struct TermVisitor {

		Evaluator* evaluator;
		// factor
		void operator()(const node::NodeFactor* factor) const {
			evaluator->evaluate_factor(factor);
		}

		// mult expr
		void operator()(const node::NodeExprMult* mult) const {
			evaluator->evaluate_term(mult->lhs);
			evaluator->evaluate_factor(mult->rhs);
			
			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs * rhs);
		}

		// divide expr
		void operator()(const node::NodeExprDivide* divide) const {
			evaluator->evaluate_term(divide->lhs);
			evaluator->evaluate_factor(divide->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs / rhs);
		}

		// mod expr
		void operator()(const node::NodeExprModulo* mod) const {
			evaluator->evaluate_term(mod->lhs);
			evaluator->evaluate_factor(mod->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(double((int)lhs % (int)rhs));
		}
	};
	mpark::visit(TermVisitor{ this }, term->var);
}

void Evaluator::evaluate_factor(const node::NodeFactor* factor)
{
	struct FactorVisitor {
		Evaluator* evaluator;

		// decimal
		void operator()(const node::NodeFactorDecimal* decimal) const {
			evaluator->m_stack.push(std::stod(decimal->decimal.value));
		}
		// identifier
		void operator()(const node::NodeFactorIdentifier* identifier) const {
			if (evaluator->m_symbolTable.contains(identifier->identifier.value)) {

				Symbol* symbol = evaluator->m_symbolTable.lookup(identifier->identifier.value);

				if (symbol->isAnArray) {
					auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
					evaluator->evaluate_arithmetic_expression(identifier->index);
					double index = mpark::get<double>(evaluator->m_stack.top());
					evaluator->m_stack.pop();
					if (mpark::holds_alternative<bool>(arr[index])) {
						evaluator->m_stack.push(mpark::get<bool>(arr[index]));
					}
					else if (mpark::holds_alternative<double>(arr[index])) {
						evaluator->m_stack.push(mpark::get<double>(arr[index]));
					}
					else if (mpark::holds_alternative<std::string>(arr[index])) {
						evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
					}
					else if (mpark::holds_alternative<Struct>(arr[index])) {
						evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
					}
				}
				else {
					if (mpark::holds_alternative<Struct>(symbol->value) && identifier->props.size() > 0) {
						for (size_t i = 0; i < identifier->props.size(); i++)
						{
							symbol = mpark::get<Struct>(symbol->value).table->lookup(identifier->props[i].value);
						}
						evaluator->m_stack.push(symbol->value);
						return;
					}

					evaluator->m_stack.push(evaluator->m_symbolTable.lookup(identifier->identifier.value)->value);
				}

			}
			else if (evaluator->m_scopedTables.empty() == false) {
				for (size_t i = evaluator->m_scopedTables.size() - 1; i >= 0; i--)
				{
					if (evaluator->m_scopedTables[i].contains(identifier->identifier.value)) {

						Symbol* symbol = evaluator->m_scopedTables[i].lookup(identifier->identifier.value);

						if (symbol->isAnArray) {
							auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
							evaluator->evaluate_arithmetic_expression(identifier->index);
							double index = mpark::get<double>(evaluator->m_stack.top());
							evaluator->m_stack.pop();
							if (mpark::holds_alternative<bool>(arr[index])) {
								evaluator->m_stack.push(mpark::get<bool>(arr[index]));
							}
							else if (mpark::holds_alternative<double>(arr[index])) {
								evaluator->m_stack.push(mpark::get<double>(arr[index]));
							}
							else if (mpark::holds_alternative<std::string>(arr[index])) {
								evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
							}
							else if (mpark::holds_alternative<Struct>(arr[index])) {
								evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
							}
						}
						else {
							if (mpark::holds_alternative<Struct>(symbol->value) && identifier->props.size() > 0) {
								for (size_t i = 0; i < identifier->props.size(); i++)
								{
									symbol = mpark::get<Struct>(symbol->value).table->lookup(identifier->props[i].value);
								}
								evaluator->m_stack.push(symbol->value);
								return;
							}
							evaluator->m_stack.push(evaluator->m_scopedTables[i].lookup(identifier->identifier.value)->value);
						}

						return;
					}
				}
			}
			else {
				std::cerr << "Undeclared identifier: " << identifier->identifier.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		// arithmetic expression
		void operator()(const node::NodeArithmeticExpr* expr) const {
			evaluator->evaluate_arithmetic_expression(expr);
		}
		void operator()(const node::NodeFunctionCall* function_call) const {
			evaluator->evaluate_function_call(function_call);
		}
	};
	mpark::visit(FactorVisitor{ this }, factor->var);
}

void Evaluator::evaluate_boolean_expression(const node::NodeBooleanExpr* expr)
{
	struct BoolExprVisitor
	{
		Evaluator* evaluator;

		// boolean or
		void operator()(const node::NodeBooleanOr* _or) const {
			evaluator->evaluate_boolean_or(_or);
		}
	};
	mpark::visit(BoolExprVisitor{ this }, expr->expr);
}

void Evaluator::evaluate_boolean_or(const node::NodeBooleanOr* _or)
{
	struct BoolOrVisitor
	{
		Evaluator* evaluator;

		// boolean and
		void operator()(const node::NodeBooleanAnd* _and) const {
			evaluator->evaluate_boolean_and(_and);
		}

		// boolean or operation
		void operator()(const node::NodeBooleanOrOperation* or_op) const {
			evaluator->evaluate_boolean_or(or_op->lhs);
			evaluator->evaluate_boolean_and(or_op->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			bool rhs = false;
			bool lhs = false;
			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				rhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type bool" << std::endl;
			}

			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				lhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type bool" << std::endl;
			}
			evaluator->m_stack.push(lhs || rhs);
		}

	};
	mpark::visit(BoolOrVisitor{ this }, _or->var);
}

void Evaluator::evaluate_boolean_and(const node::NodeBooleanAnd* _and)
{
	struct BoolAndVisitor {
		Evaluator* evaluator;
		// boolean equal
		void operator()(const node::NodeBooleanEqual* equal) const {
			evaluator->evaluate_boolean_equal(equal);
		}

		// and operation
		void operator()(const node::NodeBooleanAndOperation* and_op) const {
			evaluator->evaluate_boolean_and(and_op->lhs);
			evaluator->evaluate_boolean_equal(and_op->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			bool rhs = false;
			bool lhs = false;
			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				rhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type bool" << std::endl;
			}

			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				lhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type bool" << std::endl;
			}
			evaluator->m_stack.push(lhs && rhs);
		}
	};
	mpark::visit(BoolAndVisitor{ this }, _and->var);
}

void Evaluator::evaluate_boolean_equal(const node::NodeBooleanEqual* equal)
{
	struct BoolEqualVisitor {
		Evaluator* evaluator;

		// real Expression
		void operator()(const node::NodeBooleanRealExpression* real_expr) const {

			evaluator->evaluate_real_expr(real_expr);
		}

		// equal is
		void operator()(const node::NodeBooleanEqualIs* equal_is) const {
			evaluator->evaluate_boolean_equal(equal_is->lhs);
			evaluator->evaluate_real_expr(equal_is->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			mpark::variant<double, bool, std::string> rhs;
			mpark::variant<double, bool, std::string> lhs;
			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				rhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				rhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "value is of invalid type" << std::endl;
				exit(EXIT_FAILURE);
			}

			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				lhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				lhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "value is of invalid type" << std::endl;
				exit(EXIT_FAILURE);
			}

			if (mpark::holds_alternative<bool>(lhs) && mpark::holds_alternative<bool>(rhs)) {
				evaluator->m_stack.push(mpark::get<bool>(lhs) == (mpark::get<bool>(rhs)));
			}
			else if (mpark::holds_alternative<double>(lhs) && mpark::holds_alternative<double>(rhs)) {
				evaluator->m_stack.push(mpark::get<double>(lhs) == (mpark::get<double>(rhs)));
			}
			else if (mpark::holds_alternative<std::string>(lhs) && mpark::holds_alternative<std::string>(rhs)) {
				evaluator->m_stack.push(mpark::get<std::string>(lhs) == (mpark::get<std::string>(rhs)));
			}
			else {
				std::cerr << "lhs and rhs are not of the same type" << std::endl;
				exit(EXIT_FAILURE);
			}

			
		}

		// equal is not
		void operator()(const node::NodeBooleanEqualIsNot* equal_is_not) const {
			evaluator->evaluate_boolean_equal(equal_is_not->lhs);
			evaluator->evaluate_real_expr(equal_is_not->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			mpark::variant<double, bool, std::string> rhs;
			mpark::variant<double, bool, std::string> lhs;
			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				rhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				rhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "value is of invalid type" << std::endl;
				exit(EXIT_FAILURE);
			}

			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				lhs = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				lhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "value is of invalid type" << std::endl;
				exit(EXIT_FAILURE);
			}

			if (mpark::holds_alternative<bool>(lhs) && mpark::holds_alternative<bool>(rhs)) {
				evaluator->m_stack.push(mpark::get<bool>(lhs) != (mpark::get<bool>(rhs)));
			}
			else if (mpark::holds_alternative<double>(lhs) && mpark::holds_alternative<double>(rhs)) {
				evaluator->m_stack.push(mpark::get<double>(lhs) != (mpark::get<double>(rhs)));
			}
			else if (mpark::holds_alternative<std::string>(lhs) && mpark::holds_alternative<std::string>(rhs)) {
				evaluator->m_stack.push(mpark::get<std::string>(lhs) != (mpark::get<std::string>(rhs)));
			}
			else {
				std::cerr << "lhs and rhs are not of the same type" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

	};
	mpark::visit(BoolEqualVisitor{ this }, equal->var);
}

void Evaluator::evaluate_real_expr(const node::NodeBooleanRealExpression* expr)
{
	struct RealExprVisitor {
		Evaluator* evaluator;

		// not 
		void operator()(const node::NodeBooleanNot* _not) const {
			evaluator->evaluate_not(_not);
		}

		// less 
		void operator()(const node::NodeBooleanLess* less) const {
			evaluator->evaluate_real_expr(less->lhs);
			evaluator->evaluate_not(less->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs < rhs);
		}
		// less equal
		void operator()(const node::NodeBooleanLessEqual* less_equal) const {
			evaluator->evaluate_real_expr(less_equal->lhs);
			evaluator->evaluate_not(less_equal->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs <= rhs);
		}

		// greater 
		void operator()(const node::NodeBooleanGreater* greater) const {
			evaluator->evaluate_real_expr(greater->lhs);
			evaluator->evaluate_not(greater->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs > rhs);
		}

		// greater equal
		void operator()(const node::NodeBooleanGreaterEqual* greater_equal) const {
			evaluator->evaluate_real_expr(greater_equal->lhs);
			evaluator->evaluate_not(greater_equal->rhs);

			// assign the right hand side
			//check if top of stack is a variable
			double rhs = 0;
			double lhs = 0;
			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				rhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}

			if (mpark::holds_alternative<double>(evaluator->m_stack.top())) {
				lhs = mpark::get<double>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type double" << std::endl;
			}
			evaluator->m_stack.push(lhs >= rhs);
		}

	};
	mpark::visit(RealExprVisitor{ this }, expr->var);
}

void Evaluator::evaluate_not(const node::NodeBooleanNot* _not)
{
	struct BoolNotVisitor {
		Evaluator* evaluator;

		// boolean factor 
		void operator()(const node::NodeBooleanFactor* factor) const {
			evaluator->evaluate_bool_factor(factor);
		}

		// boolean factor 
		void operator()(const node::NodeBooleanNotOperation* not_op) const {
			evaluator->evaluate_not(not_op->expr);

			bool value;
			if (mpark::holds_alternative<bool>(evaluator->m_stack.top())) {
				value = mpark::get<bool>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type bool" << std::endl;
			}
			evaluator->m_stack.push(!value);
		}

	};
	mpark::visit(BoolNotVisitor{ this }, _not->var);
}

void Evaluator::evaluate_bool_factor(const node::NodeBooleanFactor* factor)
{
	struct BooleanFactorVisitor {
		Evaluator* evaluator;

		// boolean value 
		void operator()(const node::NodeBooleanFactorValue* value) const {
			if(value->boolval.value == "true")
				evaluator->m_stack.push(true);
			else
				evaluator->m_stack.push(false);
		}

		// identifier 
		void operator()(const node::NodeBooleanFactorIdentifier* identifier) const {
			if (evaluator->m_symbolTable.contains(identifier->identifier.value)) {
				Symbol* symbol = evaluator->m_symbolTable.lookup(identifier->identifier.value);

				if (symbol->isAnArray) {
					auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
					evaluator->evaluate_arithmetic_expression(identifier->index);
					double index = mpark::get<double>(evaluator->m_stack.top());
					evaluator->m_stack.pop();

					if (mpark::holds_alternative<bool>(arr[index])) {
						evaluator->m_stack.push(mpark::get<bool>(arr[index]));
					}
					else if (mpark::holds_alternative<double>(arr[index])) {
						evaluator->m_stack.push(mpark::get<double>(arr[index]));
					}
					else if (mpark::holds_alternative<std::string>(arr[index])) {
						evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
					}
					else if (mpark::holds_alternative<Struct>(arr[index])) {
						evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
					}
				}
				else {
					if (mpark::holds_alternative<Struct>(symbol->value) && identifier->props.size() > 0) {
						for (size_t i = 0; i < identifier->props.size(); i++)
						{
							symbol = mpark::get<Struct>(symbol->value).table->lookup(identifier->props[i].value);
						}
						evaluator->m_stack.push(symbol->value);
						return;
					}
					evaluator->m_stack.push(evaluator->m_symbolTable.lookup(identifier->identifier.value)->value);
				}
			}
			else if (evaluator->m_scopedTables.empty() == false) {
				for (size_t i = evaluator->m_scopedTables.size() - 1; i >= 0; i--)
				{
					if (evaluator->m_scopedTables[i].contains(identifier->identifier.value)) {
						Symbol* symbol = evaluator->m_scopedTables[i].lookup(identifier->identifier.value);

						if (symbol->isAnArray) {
							auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
							evaluator->evaluate_arithmetic_expression(identifier->index);
							double index = mpark::get<double>(evaluator->m_stack.top());
							evaluator->m_stack.pop();
							if (mpark::holds_alternative<bool>(arr[index])) {
								evaluator->m_stack.push(mpark::get<bool>(arr[index]));
							}
							else if (mpark::holds_alternative<double>(arr[index])) {
								evaluator->m_stack.push(mpark::get<double>(arr[index]));
							}
							else if (mpark::holds_alternative<std::string>(arr[index])) {
								evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
							}
							else if (mpark::holds_alternative<Struct>(arr[index])) {
								evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
							}
						}
						else {
							if (mpark::holds_alternative<Struct>(symbol->value) && identifier->props.size() > 0) {
								for (size_t i = 0; i < identifier->props.size(); i++)
								{
									symbol = mpark::get<Struct>(symbol->value).table->lookup(identifier->props[i].value);
								}
								evaluator->m_stack.push(symbol->value);
								return;
							}

							evaluator->m_stack.push(evaluator->m_scopedTables[i].lookup(identifier->identifier.value)->value);
						}
						return;
					}
				}
			}
			else {
				std::cerr << "Undeclared identifier: " << identifier->identifier.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		// arithmetic expression
		void operator()(const node::NodeArithmeticExpr* expr) const {
			evaluator->evaluate_arithmetic_expression(expr);
		}

		// string expression
		void operator()(const node::NodeStringExpr* expr) const {
			evaluator->evaluate_string_expression(expr);
		}

		// boolean expr 
		void operator()(const node::NodeBooleanExpr* expr) const {
			evaluator->evaluate_boolean_expression(expr);
		}
		void operator()(const node::NodeFunctionCall* function_call) const {
			evaluator->evaluate_function_call(function_call);

			if (evaluator->m_stack.empty() == false) {
				auto val = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				if (mpark::holds_alternative<bool>(val)) {
					evaluator->m_stack.push(mpark::get<bool>(val));
				}
				else {
					std::cerr << "Function does not return type of boolean!!" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else {
				std::cerr << "Function does not return type of boolean!!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

	};
	mpark::visit(BooleanFactorVisitor{ this }, factor->var);
}

void Evaluator::evaluate_string_expression(const node::NodeStringExpr* expr)
{
	struct StringExpressionVisitor {
		Evaluator* evaluator;

		// string value
		void operator()(const node::NodeStringValue* val) const {
			evaluator->m_stack.push(val->value.value);
		}
		// identifier
		void operator()(const node::NodeStringIdentifier* ident) const {
			if (evaluator->m_symbolTable.contains(ident->ident.value)) {
				Symbol* symbol = evaluator->m_symbolTable.lookup(ident->ident.value);

				if (symbol->isAnArray) {
					auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
					evaluator->evaluate_arithmetic_expression(ident->index);
					double index = mpark::get<double>(evaluator->m_stack.top());
					evaluator->m_stack.pop();
					if (mpark::holds_alternative<bool>(arr[index])) {
						evaluator->m_stack.push(mpark::get<bool>(arr[index]));
					}
					else if (mpark::holds_alternative<double>(arr[index])) {
						evaluator->m_stack.push(mpark::get<double>(arr[index]));
					}
					else if (mpark::holds_alternative<std::string>(arr[index])) {
						evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
					}
					else if (mpark::holds_alternative<Struct>(arr[index])) {
						evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
					}
				}
				else {
					if (mpark::holds_alternative<Struct>(symbol->value) && ident->props.size() > 0) {
						for (size_t i = 0; i < ident->props.size(); i++)
						{
							symbol = mpark::get<Struct>(symbol->value).table->lookup(ident->props[i].value);
						}
						evaluator->m_stack.push(symbol->value);
						return;
					}

					evaluator->m_stack.push(evaluator->m_symbolTable.lookup(ident->ident.value)->value);
				}
			}
			else if (evaluator->m_scopedTables.empty() == false) {
				for (size_t i = evaluator->m_scopedTables.size() - 1; i >= 0; i--)
				{
					if (evaluator->m_scopedTables[i].contains(ident->ident.value)) {
						Symbol* symbol = evaluator->m_scopedTables[i].lookup(ident->ident.value);

						if (symbol->isAnArray) {
							auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
							evaluator->evaluate_arithmetic_expression(ident->index);
							double index = mpark::get<double>(evaluator->m_stack.top());
							evaluator->m_stack.pop();
							if (mpark::holds_alternative<bool>(arr[index])) {
								evaluator->m_stack.push(mpark::get<bool>(arr[index]));
							}
							else if (mpark::holds_alternative<double>(arr[index])) {
								evaluator->m_stack.push(mpark::get<double>(arr[index]));
							}
							else if (mpark::holds_alternative<std::string>(arr[index])) {
								evaluator->m_stack.push(mpark::get<std::string>(arr[index]));
							}
							else if (mpark::holds_alternative<Struct>(arr[index])) {
								evaluator->m_stack.push(mpark::get<Struct>(arr[index]));
							}
						}
						else {

							if (mpark::holds_alternative<Struct>(symbol->value) && ident->props.size() > 0) {
								for (size_t i = 0; i < ident->props.size(); i++)
								{
									symbol = mpark::get<Struct>(symbol->value).table->lookup(ident->props[i].value);
								}
								evaluator->m_stack.push(symbol->value);
								return;
							}

							evaluator->m_stack.push(evaluator->m_scopedTables[i].lookup(ident->ident.value)->value);
						}
						return;
					}
				}
			}
			else {
				std::cerr << "Undeclared identifier: " << ident->ident.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		// string value
		void operator()(const node::NodeStringExprConcat* concat) const {
			evaluator->evaluate_string_expression(concat->lhs);
			evaluator->evaluate_string_expression(concat->rhs);

			//assign the right hand side
			//check if top of stack is a variable
			std::string rhs;
			std::string lhs;
			if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				rhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type string" << std::endl;
			}

			if (mpark::holds_alternative<std::string>(evaluator->m_stack.top())) {
				lhs = mpark::get<std::string>(evaluator->m_stack.top());
				evaluator->m_stack.pop();
			}
			else {
				std::cerr << "expected value of type string" << std::endl;
			}
			evaluator->m_stack.push(lhs + rhs);
		}
		// Function call
		void operator()(const node::NodeFunctionCall* function_call) const {
			evaluator->evaluate_function_call(function_call);

			if (evaluator->m_stack.empty() == false) {
				auto val = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				evaluator->m_stack.push(val);
			}
			else {
				std::cerr << "Function does not return a value!!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}
	};
	mpark::visit(StringExpressionVisitor{ this }, expr->var);
}

double Evaluator::evaluate_number_array(const node::NodeNumberArrayDecl* arr)
{
	if (arr->elements.size() > 0) {
		for (node::NodeArithmeticExpr* expr : arr->elements) {
			this->evaluate_arithmetic_expression(expr);
		}
		return mpark::get<size_t>(arr->size);
	}
	else if (mpark::holds_alternative<node::NodeArithmeticExpr*>(arr->size)) {
		this->evaluate_arithmetic_expression(mpark::get<node::NodeArithmeticExpr*>(arr->size));
		double size = mpark::get<double>(this->m_stack.top());
		m_stack.pop();
		return size;
	}
}

double Evaluator::evaluate_string_array(const node::NodeStringArrayDecl* arr)
{
	if(arr->elements.size() > 0) {
		for (node::NodeStringExpr* expr : arr->elements) {
			this->evaluate_string_expression(expr);
		}
		return mpark::get<size_t>(arr->size);
	}
	else if (mpark::holds_alternative<node::NodeArithmeticExpr*>(arr->size)) {
		this->evaluate_arithmetic_expression(mpark::get<node::NodeArithmeticExpr*>(arr->size));
		double size = mpark::get<double>(this->m_stack.top());
		m_stack.pop();
		return size;
	}
}

double Evaluator::evaluate_boolean_array(const node::NodeBooleanArrayDecl* arr)
{
	if (arr->elements.size() > 0) {
		for (node::NodeBooleanExpr* expr : arr->elements) {
			this->evaluate_boolean_expression(expr);
		}
		return mpark::get<size_t>(arr->size);
	}
	else if (mpark::holds_alternative<node::NodeArithmeticExpr*>(arr->size)) {
		this->evaluate_arithmetic_expression(mpark::get<node::NodeArithmeticExpr*>(arr->size));
		double size = mpark::get<double>(this->m_stack.top());
		m_stack.pop();
		return size;
	}
}

double Evaluator::evaluate_object_array(const node::NodeObjectArrayDecl* arr)
{
	if (arr->elements.size() > 0) {
		for (node::NodeObjectDecl* decl : arr->elements) {
			this->evaluate_object_declare_to_stack(decl);
		}
		return mpark::get<size_t>(arr->size);
	}
	else if (mpark::holds_alternative<node::NodeArithmeticExpr*>(arr->size)) {
		this->evaluate_arithmetic_expression(mpark::get<node::NodeArithmeticExpr*>(arr->size));
		double size = mpark::get<double>(this->m_stack.top());
		m_stack.pop();
		return size;
	}
}

void Evaluator::evaluate_assignment(const node::NodeAssignment* assignment)
{
	Symbol symbol_lhs = m_symbolTable.lookup(assignment->identifierHead.value) != nullptr ? *m_symbolTable.lookup(assignment->identifierHead.value) : Symbol();
	Symbol* result = m_symbolTable.lookup(assignment->identifierHead.value);
	size_t index = -1;
	// Search scoped symbol tables if not found in global
	if (!m_symbolTable.contains(assignment->identifierHead.value) && !m_scopedTables.empty()) {
		for (int i = static_cast<int>(m_scopedTables.size()) - 1; i >= 0; --i) {
			if (m_scopedTables[i].contains(assignment->identifierHead.value)) {
				symbol_lhs = *m_scopedTables[i].lookup(assignment->identifierHead.value);
				result = m_scopedTables[i].lookup(assignment->identifierHead.value);
				index = i;
				break;
			}
		}
	}

	if (result == nullptr) {
		std::cerr << "Assignment to undeclared variable: " << assignment->identifierHead.value << std::endl;
		exit(EXIT_FAILURE);
	}

	// Assign to array element
	if (symbol_lhs.isAnArray && assignment->index != nullptr) {
		size_t lhs_index = get_array_index(assignment->index);
		auto& arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol_lhs.value);

		if (lhs_index >= arr.size()) {
			std::cerr << "Array index out of bounds" << std::endl;
			exit(EXIT_FAILURE);
		}

		evaluate_value(assignment->rhs);
		auto rhs = m_stack.top(); m_stack.pop();

		if (type_check<double>(symbol_lhs, "number", rhs))
			arr[lhs_index] = mpark::get<double>(rhs);
		else if (type_check<std::string>(symbol_lhs, "string", rhs))
			arr[lhs_index] = mpark::get<std::string>(rhs);
		else if (type_check<bool>(symbol_lhs, "boolean", rhs))
			arr[lhs_index] = mpark::get<bool>(rhs);
		else if (type_check<Struct>(symbol_lhs, "object", rhs))
			arr[lhs_index] = mpark::get<Struct>(rhs);
		else {
			errorHandling::semantic_error(symbol_lhs, variant_to_type(rhs));
		}

		symbol_lhs.value = arr;
		if(index != -1)
			result = m_scopedTables[index].lookup(assignment->identifierHead.value);
		else
			result = m_symbolTable.lookup(assignment->identifierHead.value);
		result->value = symbol_lhs.value;
		return;
	}

	// Assign whole array
	if (symbol_lhs.isAnArray && assignment->index == nullptr) {
		evaluate_value(assignment->rhs);
		auto rhs = m_stack.top(); m_stack.pop();

		if (!mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(rhs)) {
			std::cerr << "RHS is not an array!" << std::endl;
			exit(EXIT_FAILURE);
		}

		auto rhs_arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(rhs);
		if (rhs_arr.empty()) {
			std::cerr << "Cannot assign empty array without knowing element type!" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (type_check_arr<double>(symbol_lhs, "number", rhs_arr[0]) ||
			type_check_arr<std::string>(symbol_lhs, "string", rhs_arr[0]) ||
			type_check_arr<bool>(symbol_lhs, "boolean", rhs_arr[0]) ||
			type_check_arr<Struct>(symbol_lhs, "object", rhs_arr[0])) {
			symbol_lhs.value = rhs_arr;
		}
		else {
			errorHandling::semantic_error(symbol_lhs, variant_to_type(rhs));
		}
		if (index != -1)
			result = m_scopedTables[index].lookup(assignment->identifierHead.value);
		else
			result = m_symbolTable.lookup(assignment->identifierHead.value);
		result->value = symbol_lhs.value;
		return;
	}

	// Standard variable assignment
	evaluate_value(assignment->rhs);
	auto rhs = m_stack.top(); m_stack.pop();

	if (type_check<double>(symbol_lhs, "number", rhs))
		symbol_lhs.value = rhs;
	else if (type_check<std::string>(symbol_lhs, "string", rhs))
		symbol_lhs.value = rhs;
	else if (type_check<bool>(symbol_lhs, "boolean", rhs))
		symbol_lhs.value = rhs;
	else if (type_check<Struct>(symbol_lhs, "object", rhs))
		symbol_lhs.value = rhs;
	else {
		errorHandling::semantic_error(symbol_lhs, variant_to_type(rhs));
	}

	if (index != -1)
		result = m_scopedTables[index].lookup(assignment->identifierHead.value);
	else
		result = m_symbolTable.lookup(assignment->identifierHead.value);
	result->value = symbol_lhs.value;
}



void Evaluator::evaluate_assignment_object(const node::NodeAssignment* assignment, SymbolTable* table)
{
	if (Symbol* symbol = table->lookup(assignment->identifierHead.value)) {
		evaluate_value(assignment->rhs);
		auto rhs = m_stack.top();
		m_stack.pop();

		mpark::variant<double, std::string, bool, Struct, std::vector<mpark::variant<double, std::string, bool, Struct>>>* value_ptr = &symbol->value;

		// Handle root array index: e.g., arr[2] = 5
		if (assignment->index != nullptr) {
			if (!mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(*value_ptr)) {
				std::cerr << "Trying to index non-array variable" << std::endl;
				exit(EXIT_FAILURE);
			}
			size_t idx = get_array_index(assignment->index);
			auto& vec = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(*value_ptr);

			if (idx >= vec.size()) {
				std::cerr << "Array index out of bounds" << std::endl;
				exit(EXIT_FAILURE);
			}

			auto& elem = vec[idx];

			// Type check and assign
			if (type_check<double>(*symbol, "number",rhs)) {
				elem = mpark::get<double>(rhs);
			}
			else if (type_check<std::string>(*symbol, "string", rhs)) {
				elem = mpark::get<std::string>(rhs);
			}
			else if (type_check<bool>(*symbol, "boolean", rhs)) {
				elem = mpark::get<bool>(rhs);
			}
			else if (type_check<Struct>(*symbol, "object", rhs)) {
				elem = mpark::get<Struct>(rhs);
			}
			else {
				errorHandling::semantic_error(*symbol, variant_to_type(rhs));
			}
			return;
		}

		// Handle property access (possibly nested)
		if (assignment->props.empty()) {
			// No props: assign directly
			symbol->value = rhs;
			return;
		}

		// Traverse props
		Struct* current_struct = nullptr;
		SymbolTable* current_table = nullptr;
		mpark::variant<double, std::string, bool, Struct, std::vector<mpark::variant<double, std::string, bool, Struct>>>* current_value = value_ptr;

		for (size_t i = 0; i < assignment->props.size(); ++i) {
			const std::string& prop_name = assignment->props[i].value;
			bool is_last = (i == assignment->props.size() - 1);

			if (mpark::holds_alternative<Struct>(*current_value)) {
				current_struct = &mpark::get<Struct>(*current_value);
				current_table = current_struct->table;

				if (!current_table) {
					std::cerr << "Null symbol table in object '" << current_struct->name << "'" << std::endl;
					exit(EXIT_FAILURE);
				}

				Symbol* next_sym = current_table->lookup(prop_name);
				if (!next_sym) {
					std::cerr << "Property '" << prop_name << "' not found in object" << std::endl;
					exit(EXIT_FAILURE);
				}

				if (is_last) {
					// Assign to final property
					next_sym->value = rhs;
					return;
				} else {
					current_value = &next_sym->value;
				}
			}
			else if (mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(*current_value)) {
				std::cerr << "Unexpected array while traversing property path '" << prop_name << "'" << std::endl;
				exit(EXIT_FAILURE);
			}
			else {
				std::cerr << "Cannot access property '" << prop_name << "' on non-object" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		std::cerr << "Assignment on left hand side is an undeclared variable!" << std::endl;
		exit(EXIT_FAILURE);
	}
}



void Evaluator::evaluate_definition(const node::NodeDefinition* definition)
{
	struct DefinitionVisitor
	{
		Evaluator* evaluator;
		// Function definition 
		void operator()(const node::NodeFunctionDefinition* func_def) const {
			evaluator->evaluate_function_definition(func_def);
		}

		// Object definition
		void operator()(const node::NodeObjectDefinition* obj_def) const {
			evaluator->evaluate_object_definition(obj_def);
		}
	};
	mpark::visit(DefinitionVisitor{ this }, definition->var);
}

void Evaluator::evaluate_function_definition(const node::NodeFunctionDefinition* func_def)
{
	if (m_functionTable.contains(func_def->functionName.value)) {
		std::cerr << "Function " << func_def->functionName.value << " is already defined!" << std::endl;
		exit(EXIT_FAILURE);
	}
	Function func;
	func.name = func_def->functionName.value;
	if (func_def->type != nullptr) {
		func.type = func_def->type->value;
		func.type = get_type(*func_def->type); 
		func.isAnArray = func_def->isAnArray;
	}

	for (node::NodeFunctionArg* arg : func_def->args) {
		Symbol symbol;
		symbol.name = arg->identifier.value;
		symbol.type = arg->type.value;
		symbol.type = get_type(arg->type);
		symbol.isAnArray = arg->isTypeAnArray;
		func.args.push_back(symbol);
	}

	func.stmts = func_def->stmts;
	m_functionTable.insert(func);
}

void Evaluator::evaluate_object_definition(const node::NodeObjectDefinition* obj_def)
{
	if (!m_structDefinitionTable.contains(obj_def->identifier.value)) {
		Struct obj;
		obj.name = obj_def->identifier.value;
		obj.table = new SymbolTable();

		for (node::NodeDecl* decl : obj_def->props)
		{
			evaluate_object_declecration(decl, obj.table);
		}
		m_structDefinitionTable.insert(obj);
	}
	else
	{
		std::cerr << "Object of type: " << obj_def->identifier.value << " already has a definition!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Evaluator::evaluate_function_call(const node::NodeFunctionCall* func_call)
{
	
	if (m_functionTable.contains(func_call->functionName.value)) {
		Function* func = m_functionTable.lookup(func_call->functionName.value);

		if (func->args.size() == func_call->args.size()) {

			assign_function_args(func, func_call);

			bool _break = false;
			SymbolTable table;

			for (Symbol s : func->args)
				table.insert(s);

			m_scopedTables.push_back(table);
			
			for (node::NodeFunctionStmt* stmt : func->stmts) {
				if (_break == true) {
					break;
				}
				evaluate_function_stmt(stmt, func, &_break);
			}

			m_scopedTables.pop_back();

		}
		else {
			std::cerr << "function arguments do not match!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else {
		std::cerr << "Function: " << func_call->functionName.value << " not defined!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Evaluator::evaluate_value(const node::NodeValue* val)
{
	struct ValueVisittor
	{
		Evaluator* evaluator;
		node::NodeArithmeticExpr* expr;

		// boolean value 
		void operator()(const node::NodeValueIdentifier* ident) const {
			
			if (evaluator->m_symbolTable.contains(ident->identifier.value)) {
				Symbol* symbol = evaluator->m_symbolTable.lookup(ident->identifier.value);

				//Handle if symbol is an array
				if (symbol->isAnArray == true && expr != nullptr) {
					std::vector<mpark::variant<double, std::string, bool, Struct>> val =
						mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
					size_t index = evaluator->get_array_index(expr);

					if (mpark::holds_alternative<double>(val[index])) {
						evaluator->m_stack.push(mpark::get<double>(val[index]));
					}
					else if (mpark::holds_alternative<bool>(val[index])) {
						evaluator->m_stack.push(mpark::get<bool>(val[index]));
					}
					else if (mpark::holds_alternative<std::string>(val[index])) {
						evaluator->m_stack.push(mpark::get<std::string>(val[index]));
					}

				}
				// type is just an array of a primitive type
				else if (symbol->isAnArray == true && expr == nullptr) {
					evaluator->m_stack.push(symbol->value);
				}
				// symbol is just primitive
				else if (symbol->isAnArray == false && expr == nullptr) {
					evaluator->m_stack.push(symbol->value);
				}
				else {
					std::cerr << "Identifier is not an array" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else if (evaluator->m_scopedTables.size() > 0) {
				for (size_t i = evaluator->m_scopedTables.size() - 1; i >= 0; i--)
				{
					if (evaluator->m_scopedTables[i].contains(ident->identifier.value)) {
						Symbol* symbol = evaluator->m_scopedTables[i].lookup(ident->identifier.value);

						//Handle if symbol is an array
						if (symbol->isAnArray == true && expr != nullptr) {
							std::vector<mpark::variant<double, std::string, bool, Struct>> val =
								mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
							size_t index = evaluator->get_array_index(expr);

							if (mpark::holds_alternative<double>(val[index])) {
								evaluator->m_stack.push(mpark::get<double>(val[index]));
							}
							else if (mpark::holds_alternative<bool>(val[index])) {
								evaluator->m_stack.push(mpark::get<bool>(val[index]));
							}
							else if (mpark::holds_alternative<std::string>(val[index])) {
								evaluator->m_stack.push(mpark::get<std::string>(val[index]));
							}

						}
						// type is just an array of a primitive type
						else if (symbol->isAnArray == true && expr == nullptr) {
							evaluator->m_stack.push(symbol->value);
						}
						// symbol is just primitive
						else if (symbol->isAnArray == false && expr == nullptr) {
							evaluator->m_stack.push(symbol->value);
						}
						else {
							std::cerr << "Identifier is not an array" << std::endl;
							exit(EXIT_FAILURE);
						}
						return;
					}
				}
			}
			else
			{
				std::cerr << "Undeclared identifier! " << ident->identifier.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		void operator()(const node::NodeValueFunctionCall* func_call) const {
			
			evaluator->evaluate_function_call(func_call->functionCall);
		}
		void operator()(const node::NodeValueArithmeticExpression* expr) const {
			evaluator->evaluate_arithmetic_expression(expr->expr);
		}
		void operator()(const node::NodeValueStringExpression* expr) const {
			evaluator->evaluate_string_expression(expr->expr);
		}
		void operator()(const node::NodeValueBooleanExpression* expr) const {
			evaluator->evaluate_boolean_expression(expr->expr);
		}
		void operator()(const node::NodeValueIdentifierProperty* props) const {
			evaluator->evaluate_identifier_property(props);
		}
		void operator()(const node::NodeValueFunctionCallProperty* props) const {
			evaluator->evaluate_function_call_property(props);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}
	};
	mpark::visit(ValueVisittor{ this, val->index }, val->var);
}

// TODO: Support arrays
void Evaluator::evaluate_identifier_property(const node::NodeValueIdentifierProperty* props)
{
	if (m_symbolTable.contains(props->identfierHead.value)) {
		Symbol* symbol = m_symbolTable.lookup(props->identfierHead.value);

		if (symbol->type == "object") {
			Symbol* prop = new Symbol();
			// the assumption is that up till the last element its all structs
			for (size_t i = 0; i < props->identifierproperties.size()-1; i++)
			{
				prop = mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties[i].value);
				if (prop->type == "object") {
					// go into the object and store in symbol
					symbol = prop;
				}
				else {
					errorHandling::semantic_error(*symbol, "object");
				}
			}

			// We at the end of identifier properties
			m_stack.push(mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties[props->identifierproperties.size() - 1].value)->value);
		}
	}
	else if (m_scopedTables.empty() == false) {
		for (size_t i = m_scopedTables.size() - 1; i >= 0; i--)
		{
			if (m_scopedTables[i].contains(props->identfierHead.value)) {
				Symbol* symbol = m_scopedTables[i].lookup(props->identfierHead.value);

				if (symbol->type == "object") {
					Symbol* prop = new Symbol();
					// the assumption is that up till the last element its all structs
					for (size_t i = 0; i < props->identifierproperties.size() - 1; i++)
					{
						prop = mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties[i].value);
						if (prop->type == "object") {
							// go into the object and store in symbol
							symbol = prop;
						}
						else {
							errorHandling::semantic_error(*symbol, "object");
						}
					}

					// We at the end of identifier properties
					m_stack.push(mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties[props->identifierproperties.size() - 1].value)->value);
				}
				return;
			}
		}
	}
	else {
		std::cerr << "Undeclared identifier: " << props->identfierHead.value << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Evaluator::evaluate_function_call_property(const node::NodeValueFunctionCallProperty* props)
{
	evaluate_function_call(props->functionCall);

	if (m_stack.empty() == false) {
		auto value = m_stack.top();
		m_stack.pop();

		if (mpark::holds_alternative<Struct>(value)) {
			Symbol* symbol = mpark::get<Struct>(value).table->lookup(props->identifierproperties->identifierproperties[0].value);

			if (symbol != nullptr) {
				if (symbol->type == "object") {
					Symbol* prop = new Symbol();
					// the assumption is that up till the last element its all structs
					for (size_t i = 1; i < props->identifierproperties->identifierproperties.size() - 1; i++)
					{
						prop = mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties->identifierproperties[i].value);
						if (prop->type == "object") {
							// go into the object and store in symbol
							symbol = prop;
						}
						else {
							std::cerr << "identifier property is not of type object!!" << std::endl;
							exit(EXIT_FAILURE);
						}
					}

					// We at the end of identifier properties
					m_stack.push(mpark::get<Struct>(symbol->value).table->lookup(props->identifierproperties->identifierproperties[props->identifierproperties->identifierproperties.size() - 1].value)->value);
				}
			}
			else {
				std::cerr << "Object does not have symbol: " << props->identifierproperties->identifierproperties[0].value << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else {
			std::cerr << "Function call did not return a value of type object" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else {
		std::cerr << "Function call did not return a value" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Evaluator::evaluate_value_object(const node::NodeValue* val, SymbolTable* table)
{
	struct ValueVisittor
	{
		Evaluator* evaluator;
		node::NodeArithmeticExpr* expr;
		SymbolTable* table;

		// boolean value 
		void operator()(const node::NodeValueIdentifier* ident) const {

			if (table->contains(ident->identifier.value)) {
				Symbol* symbol = table->lookup(ident->identifier.value);

				//Handle if symbol is an array
				if (symbol->isAnArray == true && expr != nullptr) {
					std::vector<mpark::variant<double, std::string, bool, Struct>> val =
						mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(symbol->value);
					size_t index = evaluator->get_array_index(expr);

					if (mpark::holds_alternative<double>(val[index])) {
						evaluator->m_stack.push(mpark::get<double>(val[index]));
					}
					else if (mpark::holds_alternative<bool>(val[index])) {
						evaluator->m_stack.push(mpark::get<bool>(val[index]));
					}
					else if (mpark::holds_alternative<std::string>(val[index])) {
						evaluator->m_stack.push(mpark::get<std::string>(val[index]));
					}
					else if (mpark::holds_alternative<Struct>(val[index])) {
						evaluator->m_stack.push(mpark::get<Struct>(val[index]));
					}

				}
				// type is just an array of a primitive type
				else if (symbol->isAnArray == true && expr == nullptr) {
					evaluator->m_stack.push(symbol->value);
				}
				// symbol is just primitive
				else if (symbol->isAnArray == false && expr == nullptr) {
					evaluator->m_stack.push(symbol->value);
				}
				else {
					std::cerr << "Identifier is not an array" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				std::cerr << "Undeclared identifier! " << ident->identifier.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		void operator()(const node::NodeValueFunctionCall* func_call) const {

			evaluator->evaluate_function_call(func_call->functionCall);

			//TODO: make it handle if function call return an array
		}
		void operator()(const node::NodeValueArithmeticExpression* expr) const {
			evaluator->evaluate_arithmetic_expression(expr->expr);
		}
		void operator()(const node::NodeValueStringExpression* expr) const {
			evaluator->evaluate_string_expression(expr->expr);
		}
		void operator()(const node::NodeValueBooleanExpression* expr) const {
			evaluator->evaluate_boolean_expression(expr->expr);
		}
		void operator()(const node::NodeValueIdentifierProperty* props) const {
			evaluator->evaluate_identifier_property(props);
		}
		void operator()(const node::NodeValueFunctionCallProperty* props) const {
			evaluator->evaluate_function_call_property(props);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}
	};
	mpark::visit(ValueVisittor{ this, val->index, table }, val->var);
}

void Evaluator::evaluate_function_stmt(const node::NodeFunctionStmt* stmt, Function* func, bool* _break)
{
	struct StmtVisitor
	{
		Evaluator* evaluator;
		Function* func;
		bool* _break;

		// decleration
		void operator()(const node::NodeDecl* decl) const {
			evaluator->evaluate_declecration(decl);
		}

		// function call
		void operator()(const node::NodeFunctionCall* stmt_function_call) const {
			evaluator->evaluate_function_call(stmt_function_call);
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// function control flow
		void operator()(const node::NodeFunctionControlFlow* function_control_flow) const {
			evaluator->evaluate_function_control_flow(function_control_flow, func, _break);
		}

		// return
		void operator()(const node::NodeFunctionReturn* _return) const {
			if (func->type == "") {
				*_break = true;
				return;
			}
			evaluator->evaluate_value(_return->val);
			auto val = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			if (func->isAnArray == true && mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val)) {

				auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val);

				if (func->type == "number" && mpark::holds_alternative<double>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else if (func->type == "boolean" && mpark::holds_alternative<bool>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else if (func->type == "string" && mpark::holds_alternative<std::string>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else {
					std::cerr << "return value in '"<< func->name << "'" << " does not match function type: " << "'"  << func->type << "'" << std::endl;
					exit(EXIT_FAILURE);
				}

			}
			else if (func->type == "number" && mpark::holds_alternative<double>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else if (func->type == "boolean" && mpark::holds_alternative<bool>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else if (func->type == "string" && mpark::holds_alternative<std::string>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else {
				std::cerr << "return value in '" << func->name << "'" << " does not match function type: " << "'" << func->type << "'" << std::endl;
				exit(EXIT_FAILURE);
			}

		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}

	};
	mpark::visit(StmtVisitor{ this, func, _break }, stmt->var);
}

inline void Evaluator::evaluate_function_loop_stmt(const node::NodeFunctionLoopStmt* stmt, Function* func, bool* _break, bool* _continue)
{
	struct StmtVisitor
	{
		Evaluator* evaluator;
		Function* func;
		bool* _break;
		bool* _continue;

		// decleration
		void operator()(const node::NodeDecl* decl) const {
			evaluator->evaluate_declecration(decl);
		}

		// function call
		void operator()(const node::NodeFunctionCall* stmt_function_call) const {
			evaluator->evaluate_function_call(stmt_function_call);
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// function control flow
		void operator()(const node::NodeFunctionLoopControlFlow* function_control_flow) const {
			evaluator->evaluate_function_loop_control_flow(function_control_flow, func, _break, _continue);
		}

		// return
		void operator()(const node::NodeFunctionReturn* _return) const {
			if (func->type == "") {
				*_break = true;
				return;
			}
			evaluator->evaluate_value(_return->val);
			auto val = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			if (func->isAnArray == true && mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val)) {

				auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val);

				if (func->type == "number" && mpark::holds_alternative<double>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else if (func->type == "boolean" && mpark::holds_alternative<bool>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else if (func->type == "string" && mpark::holds_alternative<std::string>(arr[0])) {
					evaluator->m_stack.push(arr);
					*_break = true;
					return;
				}
				else {
					std::cerr << "return value in '" << func->name << "'" << " does not match function type: " << "'" << func->type << "'" << std::endl;
					exit(EXIT_FAILURE);
				}

			}
			else if (func->type == "number" && mpark::holds_alternative<double>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else if (func->type == "boolean" && mpark::holds_alternative<bool>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else if (func->type == "string" && mpark::holds_alternative<std::string>(val)) {
				evaluator->m_stack.push(val);
				*_break = true;
				return;
			}
			else {
				std::cerr << "return value in '" << func->name << "'" << " does not match function type: " << "'" << func->type << "'" << std::endl;
				exit(EXIT_FAILURE);
			}

		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// input
		void operator()(const node::NodeStmtInput* input) const {
			std::string value;
			std::getline(std::cin, value);
			evaluator->m_stack.push(value);
		}

		// break
		void operator()(const node::NodeBreak* _) const {
			*_break = true;
		}

		// continue
		void operator()(const node::NodeContinue* _) const {
			*_continue = true;
		}

	};
	mpark::visit(StmtVisitor{ this, func, _break, _continue}, stmt->var);
}

void Evaluator::evaluate_global_control_flow(const node::NodeGlobalControlFlow* flow)
{
	struct ControlFlowVisitor {
		Evaluator* evaluator;

		// if statement
		void operator()(const node::NodeGlobalIf* _if) const {
			evaluator->evaluate_boolean_expression(_if->condition);
			auto cond = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			// Initial if statement 
			if (mpark::get<bool>(cond) == true) {
				
				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);
				bool _break = false;
				for (node::NodeNestedStmt* stmt : _if->stmts) {
					if (_break == true) {
						break;
					}
					evaluator->evaluate_nested_stmt(stmt, &_break);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}

			// Check else ifs
			for (node::NodeGlobalElseIf* _elseif : _if->elseifs) {
				evaluator->evaluate_boolean_expression(_elseif->condition);
				auto cond = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				// Initial if statement 
				if (mpark::get<bool>(cond) == true) {

					SymbolTable table;
					evaluator->m_scopedTables.push_back(table);
					//std::cout << evaluator->m_scopedTables.size();
					bool _break = false;

					for (node::NodeNestedStmt* stmt : _elseif->stmts) {
						if (_break == true) {
							break;
						}
						evaluator->evaluate_nested_stmt(stmt, &_break);
					}

					evaluator->m_scopedTables.pop_back();
					return;
				}
			}

			if (_if->_else != nullptr) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);
				bool _break = false;

				for (node::NodeNestedStmt* stmt : _if->_else->stmts) {
					if (_break == true) {
						break;
					}
					evaluator->evaluate_nested_stmt(stmt, &_break);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}
		}

		// loop statement
		void operator()(const node::NodeGlobalLoop* _loop) const {
			evaluator->evaluate_global_loop(_loop);
		}

	};
	mpark::visit(ControlFlowVisitor{ this}, flow->var);
}

inline void Evaluator::evaluate_global_loop_control_flow(const node::NodeGlobalLoopControlFlow* flow, bool* _break, bool* _continue)
{
	struct ControlFlowVisitor {
		Evaluator* evaluator;

		bool* _break;
		bool* _continue;
		// if statement
		void operator()(const node::NodeGlobalLoopIf* _if) const {
			evaluator->evaluate_boolean_expression(_if->condition);
			auto cond = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			// Initial if statement 
			if (mpark::get<bool>(cond) == true) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);
				for (node::NodeLoopStmt* stmt : _if->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_loop_stmt(stmt, _break, _continue);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}

			// Check else ifs
			for (node::NodeGlobalLoopElseIf* _elseif : _if->elseifs) {
				evaluator->evaluate_boolean_expression(_elseif->condition);
				auto cond = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				// Initial if statement 
				if (mpark::get<bool>(cond) == true) {

					SymbolTable table;
					evaluator->m_scopedTables.push_back(table);

					for (node::NodeLoopStmt* stmt : _elseif->stmts) {
						if (*_break == true) {
							break;
						}
						evaluator->evaluate_loop_stmt(stmt, _break, _continue);
					}

					evaluator->m_scopedTables.pop_back();
					return;
				}
			}

			if (_if->_else != nullptr) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);

				for (node::NodeLoopStmt* stmt : _if->_else->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_loop_stmt(stmt, _break, _continue);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}
		}

		// loop statement
		void operator()(const node::NodeGlobalLoop* _loop) const {
			evaluator->evaluate_global_loop(_loop);
		}

	};
	mpark::visit(ControlFlowVisitor{ this, _break, _continue }, flow->var);
}

void Evaluator::evaluate_global_loop(const node::NodeGlobalLoop* loop)
{
	struct LoopVisitor {
		Evaluator* evaluator;

		// while statement
		void operator()(const node::NodeGlobalWhile* _while) const {
			evaluator->evaluate_global_while(_while);
		}

		// for statement
		void operator()(const node::NodeGlobalFor* _for) const {
			evaluator->evaluate_global_for(_for);
		}

	};

	mpark::visit(LoopVisitor{ this }, loop->var);
}

void Evaluator::evaluate_global_while(const node::NodeGlobalWhile* _while)
{
	evaluate_boolean_expression(_while->condition);
	auto cond = m_stack.top();
	m_stack.pop();
	SymbolTable table;
	m_scopedTables.push_back(table);
	// Initial if statement 
	while (mpark::get<bool>(cond) == true) {

		
		bool _break = false;
		bool _continue = false;
		for (node::NodeLoopStmt* stmt : _while->stmts) {
			if (_break == true) {
				break;
			}
			if (_continue == true) {
				_continue = false;
				break;
			}
			evaluate_loop_stmt(stmt, &_break,&_continue);
		}
		
		// Break out of while loop if break was true
		if (_break)
			break;
		// recalculate condition for while loop
		evaluate_boolean_expression(_while->condition);
		cond = m_stack.top();
		m_stack.pop();
	}

	m_scopedTables.pop_back();

}

void Evaluator::evaluate_global_for(const node::NodeGlobalFor* _for)
{
	Symbol index;
	index.name = _for->indexValIdentifier.value;
	index.type = "number";
	evaluate_arithmetic_expression(_for->indexValExpr);
	auto indexVal = m_stack.top();
	m_stack.pop();
	index.value = indexVal;

	SymbolTable table;
	table.insert(index);
	m_scopedTables.push_back(table);

	evaluate_arithmetic_expression(_for->increment);
	auto increment = m_stack.top();
	m_stack.pop();

	evaluate_boolean_expression(_for->condition);
	auto cond = m_stack.top();
	m_stack.pop();

	Symbol* indexRef = m_scopedTables[m_scopedTables.size()-1].lookup(index.name);

	// our "for loop"
	while (mpark::get<bool>(cond) == true) {
		bool _break = false;
		bool _continue = false;
		for (node::NodeLoopStmt* stmt : _for->stmts) {
			if (_break == true) {
				break;
			}
			if (_continue == true) {
				_continue = false;
				break;
			}
			evaluate_loop_stmt(stmt, &_break, &_continue);
		}

		// Break out of while loop if break was true
		if (_break)
			break;


		// simulate the for loop by incrementing
		Symbol* indexRef = m_scopedTables[m_scopedTables.size() - 1].lookup(index.name);
		mpark::get<double>(indexRef->value) += (size_t)mpark::get<double>(increment);
		evaluate_boolean_expression(_for->condition);
		cond = m_stack.top();
		m_stack.pop();
	}
	

}

void Evaluator::evaluate_function_control_flow(const node::NodeFunctionControlFlow* flow, Function* func, bool* _break)
{
	struct FunctionControlFlowVisitor
	{
		Evaluator* evaluator;
		Function* func;
		bool* _break;

		// if statement
		void operator()(const node::NodeFunctionIf* _if) const {
			evaluator->evaluate_boolean_expression(_if->condition);
			auto cond = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			// Initial if statement 
			if (mpark::get<bool>(cond) == true) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);
				for (node::NodeFunctionStmt* stmt : _if->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_function_stmt(stmt,func, _break);
				}
				
				evaluator->m_scopedTables.pop_back();
				return;
			}

			// Check else ifs
			for (node::NodeFunctionElseIf* _elseif : _if->elseifs) {
				evaluator->evaluate_boolean_expression(_elseif->condition);
				auto cond = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				// Initial if statement 
				if (mpark::get<bool>(cond) == true) {

					SymbolTable table;
					evaluator->m_scopedTables.push_back(table);
					//std::cout << evaluator->m_scopedTables.size();

					for (node::NodeFunctionStmt* stmt : _elseif->stmts) {
						if (*_break == true) {
							break;
						}
						evaluator->evaluate_function_stmt(stmt, func, _break);
					}

					evaluator->m_scopedTables.pop_back();
					return;
				}
			}

			if (_if->_else != nullptr) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);

				for (node::NodeFunctionStmt* stmt : _if->_else->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_function_stmt(stmt, func ,_break);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}
		}

		// loop statement
		void operator()(const node::NodeFunctionLoop* _loop) const {
			evaluator->evaluate_function_loop(_loop, func, _break);
		}

	};


	mpark::visit(FunctionControlFlowVisitor{ this, func, _break }, flow->var);
}

inline void Evaluator::evaluate_function_loop_control_flow(const node::NodeFunctionLoopControlFlow* flow, Function* func, bool* _break, bool* _continue)
{
	struct FunctionControlFlowVisitor
	{
		Evaluator* evaluator;
		Function* func;
		bool* _break;
		bool* _continue;

		// if statement
		void operator()(const node::NodeFunctionLoopIf* _if) const {
			evaluator->evaluate_boolean_expression(_if->condition);
			auto cond = evaluator->m_stack.top();
			evaluator->m_stack.pop();

			// Initial if statement 
			if (mpark::get<bool>(cond) == true) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);
				for (node::NodeFunctionLoopStmt* stmt : _if->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_function_loop_stmt(stmt, func, _break, _continue);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}

			// Check else ifs
			for (node::NodeFunctionLoopElseIf* _elseif : _if->elseifs) {
				evaluator->evaluate_boolean_expression(_elseif->condition);
				auto cond = evaluator->m_stack.top();
				evaluator->m_stack.pop();

				// Initial if statement 
				if (mpark::get<bool>(cond) == true) {

					SymbolTable table;
					evaluator->m_scopedTables.push_back(table);
					//std::cout << evaluator->m_scopedTables.size();

					for (node::NodeFunctionLoopStmt * stmt : _elseif->stmts) {
						if (*_break == true) {
							break;
						}
						evaluator->evaluate_function_loop_stmt(stmt, func, _break, _continue);
					}

					evaluator->m_scopedTables.pop_back();
					return;
				}
			}

			if (_if->_else != nullptr) {

				SymbolTable table;
				evaluator->m_scopedTables.push_back(table);

				for (node::NodeFunctionLoopStmt* stmt : _if->_else->stmts) {
					if (*_break == true) {
						break;
					}
					evaluator->evaluate_function_loop_stmt(stmt, func, _break, _continue);
				}

				evaluator->m_scopedTables.pop_back();
				return;
			}
		}

		// loop statement
		void operator()(const node::NodeFunctionLoop* _loop) const {
			evaluator->evaluate_function_loop(_loop, func, _break);
		}

	};


	mpark::visit(FunctionControlFlowVisitor{ this, func, _break, _continue }, flow->var);
}

void Evaluator::evaluate_function_loop(const node::NodeFunctionLoop* loop, Function* func, bool* _break)
{
	struct LoopVisitor {
		Evaluator* evaluator;
		Function* func;
		bool* _break;

		// while statement
		void operator()(const node::NodeFunctionWhile* _while) const {
			evaluator->evaluate_function_while(_while, func, _break);
		}

		// for statement
		void operator()(const node::NodeFunctionFor* _for) const {
			evaluator->evaluate_function_for(_for, func, _break);
		}

	};

	mpark::visit(LoopVisitor{ this, func, _break }, loop->var);
}

void Evaluator::evaluate_function_while(const node::NodeFunctionWhile* _while, Function* func, bool* _break)
{
	evaluate_boolean_expression(_while->condition);
	auto cond = m_stack.top();
	m_stack.pop();
	SymbolTable table;
	m_scopedTables.push_back(table);
	// Initial if statement 
	while (mpark::get<bool>(cond) == true) {
		bool _continue = false;
		bool _break = false;
		for (node::NodeFunctionLoopStmt* stmt : _while->stmts) {
			if (_break == true) {
				break;
			}
			if (_continue == true) {
				_continue = false;
				break;
			}
			evaluate_function_loop_stmt(stmt, func, &_break, &_continue);
		}

		// Break out of while loop if break was true
		if (_break)
			break;
		// recalculate condition for while loop
		evaluate_boolean_expression(_while->condition);
		cond = m_stack.top();
		m_stack.pop();
	}

	m_scopedTables.pop_back();
}

void Evaluator::evaluate_function_for(const node::NodeFunctionFor* _for, Function* func, bool* _break)
{
	Symbol index;
	index.name = _for->indexValIdentifier.value;
	index.type = "number";
	evaluate_arithmetic_expression(_for->indexValExpr);
	auto indexVal = m_stack.top();
	m_stack.pop();
	index.value = indexVal;

	SymbolTable table;
	table.insert(index);
	m_scopedTables.push_back(table);

	evaluate_arithmetic_expression(_for->increment);
	auto increment = m_stack.top();
	m_stack.pop();

	evaluate_boolean_expression(_for->condition);
	auto cond = m_stack.top();
	m_stack.pop();


	// our "for loop"
	while (mpark::get<bool>(cond) == true) {
		bool _continue = false;
		bool _break = false;
		for (node::NodeFunctionLoopStmt* stmt : _for->stmts) {
			if (_break == true) {
				break;
			}
			if (_continue == true) {
				_continue = false;
				break;
			}
			evaluate_function_loop_stmt(stmt, func, &_break, &_continue);
		}

		// Break out of while loop if break was true
		if (_break)
			break;

		// simulate the for loop by incrementing
		Symbol* indexRef = m_scopedTables[m_scopedTables.size() - 1].lookup(index.name);
		mpark::get<double>(indexRef->value) += (size_t)mpark::get<double>(increment);
		evaluate_boolean_expression(_for->condition);
		cond = m_stack.top();
		m_stack.pop();
	}
	m_scopedTables.pop_back();
}

size_t Evaluator::get_array_index(const node::NodeArithmeticExpr* expr)
{
	this->evaluate_arithmetic_expression(expr);
	size_t size = mpark::get<double>(this->m_stack.top());
	m_stack.pop();
	return size;
}

void Evaluator::assign_function_args(Function* func, const node::NodeFunctionCall * func_call)
{
	for (size_t i = 0; i < func->args.size(); i++)
	{
		evaluate_value(func_call->args[i]->value);
		auto val = m_stack.top();
		m_stack.pop();


		if (func->args[i].isAnArray == true && mpark::holds_alternative<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val)) {

			auto arr = mpark::get<std::vector<mpark::variant<double, std::string, bool, Struct>>>(val);

			if (func->args[i].type == "number" && mpark::holds_alternative<double>(arr[0])) {
				func->args[i].value = arr;
			}
			else if (func->args[i].type == "boolean" && mpark::holds_alternative<bool>(arr[0])) {
				func->args[i].value = arr;
			}
			else if (func->args[i].type == "string" && mpark::holds_alternative<std::string>(arr[0])) {
				func->args[i].value = arr;
			}
			else if (func->args[i].type == "object" && mpark::holds_alternative<Struct>(arr[0])) {
				func->args[i].value = arr;
			}
			else {
				std::cerr << "Parameter does not match argument" << std::endl;
				exit(EXIT_FAILURE);
			}

		}
		else if (func->args[i].type == "number" && mpark::holds_alternative<double>(val)) {
			func->args[i].value = val;
		}
		else if (func->args[i].type == "boolean" && mpark::holds_alternative<bool>(val)) {
			func->args[i].value = val;
		}
		else if (func->args[i].type == "string" && mpark::holds_alternative<std::string>(val)) {
			func->args[i].value = val;
		}
		else if (func->args[i].type == "object" && mpark::holds_alternative<Struct>(val)) {
			func->args[i].value = val;
		}
		else {
			std::cerr << "Parameter does not match argument" << std::endl;
			exit(EXIT_FAILURE);
		}

	}
}

std::string Evaluator::get_type(const Token type)
{
	switch (type.type)
	{
	case NUMBER:
		return "number";
		break;
	case BOOLEAN:
		return "boolean";
		break;
	case STRING:
		return "string";
		break;
	case IDENTIFIER:
		return "object";
		break;
	default:

		std::cerr << "Argument is not a valid type: " << type.type << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}
