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
			std::cout << "function call" << std::endl;
		}
		// assignment
		void operator()(const node::NodeAssignment* assignment) const {
			evaluator->evaluate_assignment(assignment);
		}
		// global control flow
		void operator()(const node::NodeGlobalControlFlow* global_control_flow) const {
			std::cout << "global control flow" << std::endl;
		}

		// print stmt
		void operator()(const node::NodeStmtPrint* stmt_print) const {
			evaluator->evaluate_print(stmt_print);
		}

		// definition
		void operator()(const node::NodeDefinition* definition) const {
			evaluator->evaluate_definition(definition);
		}

	};
	mpark::visit(StmtVisitor{this}, stmt->var);
}

void Evaluator::evaluate_print(const node::NodeStmtPrint* print_stmt)
{
	struct PrintVisitor
	{
		Evaluator* evaluator;

		// node value
		// TODO: actually evalua not fucking hard code it
		void operator()(const node::NodeValue* val) const {
			if (mpark::holds_alternative<node::NodeValueIdentifier*>(val->var)) {
				node::NodeValueIdentifier* ident = mpark::get<node::NodeValueIdentifier*>(val->var);
				if (evaluator->m_symbolTable.contains(ident->identifier.value)) {
					Symbol* symbol = evaluator->m_symbolTable.lookup(ident->identifier.value);
					if (symbol->type == "number") {
						std::cout << mpark::get<double>(symbol->value);
					}
					else if (symbol->type == "boolean") {
						std::cout << mpark::get<bool>(symbol->value);
					}
					else if (symbol->type == "string") {
						std::cout << mpark::get<std::string>(symbol->value);
					}
				}
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
			std::cout << "object declare" << std::endl;
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
				std::vector<mpark::variant<double, std::string, bool>> arr;
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
				std::vector<mpark::variant<double, std::string, bool>> arr;
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
				std::vector<mpark::variant<double, std::string, bool>> arr;
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
				for (mpark::variant<double, std::string, bool> val : arr) {
					std::cout << mpark::get<bool>(val);
				}
				symbol.value = arr;
				evaluator->m_symbolTable.insert(symbol);
			}
			// Assume object array decleration
			// TODO: implement object array decleration
			else {

			}
		}
	};
	mpark::visit(DeclVisitor{ this }, decl->var);
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
			evaluator->m_symbolTable.insert(string); // string decleration inserted into symbol table
		}

	};
	mpark::visit(SimpDeclVisitor{ this }, simp_decl->expr);
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
			if(evaluator->m_symbolTable.contains(identifier->identifier.value)){
				evaluator->m_stack.push(evaluator->m_symbolTable.lookup(identifier->identifier.value)->value);
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
		// TODO: function call
		void operator()(const node::NodeFunctionCall* function_call) const {
			std::cerr << "Function calls not implemented yet" << std::endl;
			exit(EXIT_FAILURE);
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
			evaluator->m_stack.push(lhs == rhs);
		}

		// equal is not
		void operator()(const node::NodeBooleanEqualIsNot* equal_is_not) const {
			evaluator->evaluate_boolean_equal(equal_is_not->lhs);
			evaluator->evaluate_real_expr(equal_is_not->rhs);

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
			evaluator->m_stack.push(lhs != rhs);
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
			evaluator->evaluate_arithmetic_expression(less->lhs);
			evaluator->evaluate_arithmetic_expression(less->rhs);

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
			evaluator->evaluate_arithmetic_expression(less_equal->lhs);
			evaluator->evaluate_arithmetic_expression(less_equal->rhs);

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
			evaluator->evaluate_arithmetic_expression(greater->lhs);
			evaluator->evaluate_arithmetic_expression(greater->rhs);

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
			evaluator->evaluate_arithmetic_expression(greater_equal->lhs);
			evaluator->evaluate_arithmetic_expression(greater_equal->rhs);

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
				evaluator->m_stack.push(evaluator->m_symbolTable.lookup(identifier->identifier.value)->value);
			}
			else {
				std::cerr << "Undeclared identifier: " << identifier->identifier.value << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		// boolean expr 
		void operator()(const node::NodeBooleanExpr* expr) const {
			evaluator->evaluate_boolean_expression(expr);
		}

		// TODO: function call
		void operator()(const node::NodeFunctionCall* function_call) const {
			std::cerr << "Function calls not implemented yet" << std::endl;
			exit(EXIT_FAILURE);
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
				evaluator->m_stack.push(evaluator->m_symbolTable.lookup(ident->ident.value)->value);
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
		// TODO: function call
		void operator()(const node::NodeFunctionCall* function_call) const {
			std::cerr << "Function calls not implemented yet" << std::endl;
			exit(EXIT_FAILURE);
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

// TODO: implement assignments with function calls, identifier property and function call property.
void Evaluator::evaluate_assignment(const node::NodeAssignment* assignment)
{
	if (Symbol* symbol_lhs = m_symbolTable.lookup(assignment->identifierHead.value)) {

		// handle if symbol is an array
		if (assignment->index != nullptr) {
			size_t lhs_index = get_array_index(assignment->index);

			std::vector<mpark::variant<double, std::string, bool>> arr =
				mpark::get<std::vector<mpark::variant<double, std::string, bool>>>(symbol_lhs->value);

			if (arr.size() <= lhs_index) {
				std::cerr << "Array bound of bounds" << std::endl;
				exit(EXIT_FAILURE);
			}

			// Handle right hand side
			if (mpark::holds_alternative<node::NodeValueIdentifier*>(assignment->rhs->var)) {
				node::NodeValueIdentifier* ident = mpark::get<node::NodeValueIdentifier*>(assignment->rhs->var);

				if (Symbol* symbol_rhs = m_symbolTable.lookup(ident->identifier.value)) {

					if (assignment->rhs->index != nullptr) {
						size_t rhs_index = get_array_index(assignment->rhs->index);
						std::vector<mpark::variant<double, std::string, bool>> arr_rhs =
							mpark::get<std::vector<mpark::variant<double, std::string, bool>>>(symbol_rhs->value);
						if (arr_rhs.size() <= rhs_index) {
							std::cerr << "Array bound of bounds" << std::endl;
							exit(EXIT_FAILURE);
						}

						//Check if type matches
						if (symbol_lhs->type == symbol_rhs->type) {
							if (symbol_lhs->type == "number") {
								arr[lhs_index] = mpark::get<double>(arr_rhs[rhs_index]);
								symbol_lhs->value = arr;
							}
							else if (symbol_lhs->type == "string") {
								arr[lhs_index] = mpark::get<std::string>(arr_rhs[rhs_index]);
								symbol_lhs->value = arr;
							}
							else if (symbol_lhs->type == "boolean") {
								arr[lhs_index] = mpark::get<bool>(arr_rhs[rhs_index]);
								symbol_lhs->value = arr;
							}
						}
						return;

					}

					//Check if type matches
					if (symbol_lhs->type == symbol_rhs->type) {
						if (symbol_lhs->type == "number") {
							arr[lhs_index] = mpark::get<double>(symbol_rhs->value);
							symbol_lhs->value = arr;
						}
						else if (symbol_lhs->type == "string") {
							arr[lhs_index] = mpark::get<std::string>(symbol_rhs->value);
							symbol_lhs->value = arr;
						}
						else if (symbol_lhs->type == "boolean") {
							arr[lhs_index] = mpark::get<bool>(symbol_rhs->value);
							symbol_lhs->value = arr;
						}
					}

				}
				else {
					std::cerr << "Assignment on right hand side is an undeclared variable!" << std::endl;
				}
			}
			else if (mpark::holds_alternative<node::NodeValueArithmeticExpression*>(assignment->rhs->var)) {
				//Check if type matches
				if (symbol_lhs->type == "number") {
					evaluate_arithmetic_expression(mpark::get<node::NodeValueArithmeticExpression*>(assignment->rhs->var)->expr);
					arr[lhs_index] = mpark::get<double>(m_stack.top());
					m_stack.pop();
					symbol_lhs->value = arr;
				}
				else {
					std::cerr << "lhs is not of type number" << std::endl;
					exit(EXIT_FAILURE);
				}

			}
			else if (mpark::holds_alternative<node::NodeValueStringExpression*>(assignment->rhs->var)) {
				//Check if type matches
				if (symbol_lhs->type == "string") {
					evaluate_string_expression(mpark::get<node::NodeValueStringExpression*>(assignment->rhs->var)->expr);
					arr[lhs_index] = mpark::get<std::string>(m_stack.top());
					m_stack.pop();
					symbol_lhs->value = arr;
				}
				else {
					std::cerr << "lhs is not of type number" << std::endl;
					exit(EXIT_FAILURE);
				}

			}
			else if (mpark::holds_alternative<node::NodeValueBooleanExpression*>(assignment->rhs->var)) {
				//Check if type matches
				if (symbol_lhs->type == "boolean") {
					evaluate_boolean_expression(mpark::get<node::NodeValueBooleanExpression*>(assignment->rhs->var)->expr);
					arr[lhs_index] = mpark::get<bool>(m_stack.top());
					m_stack.pop();
					symbol_lhs->value = arr;
				}
				else {
					std::cerr << "lhs is not of type number" << std::endl;
					exit(EXIT_FAILURE);
				}

			}

		}
		// TODO: handle identifier properties when object evaluation is implemented
		if (assignment->props.size() > 0) {

		}
		
		// Handle right hand side
		if (mpark::holds_alternative<node::NodeValueIdentifier*>(assignment->rhs->var)) {
			node::NodeValueIdentifier* ident = mpark::get<node::NodeValueIdentifier*>(assignment->rhs->var);
			
			if (Symbol* symbol_rhs = m_symbolTable.lookup(ident->identifier.value)) {

				if (assignment->rhs->index != nullptr) {
					size_t rhs_index = get_array_index(assignment->rhs->index);
					std::vector<mpark::variant<double, std::string, bool>> arr_rhs =
						mpark::get<std::vector<mpark::variant<double, std::string, bool>>>(symbol_rhs->value);
					if (arr_rhs.size() <= rhs_index) {
						std::cerr << "Array bound of bounds" << std::endl;
						exit(EXIT_FAILURE);
					}

					//Check if type matches
					if (symbol_lhs->type == symbol_rhs->type) {
						if (symbol_lhs->type == "number") {
							symbol_lhs->value = mpark::get<double>(arr_rhs[rhs_index]);
						}
						else if (symbol_lhs->type == "string") {
							symbol_lhs->value = mpark::get<std::string>(arr_rhs[rhs_index]);
						}
						else if (symbol_lhs->type == "boolean") {
							symbol_lhs->value = mpark::get<bool>(arr_rhs[rhs_index]);
						}
					}
					return;

				}

				//Check if type matches
				if (symbol_lhs->type == symbol_rhs->type) {
					if (symbol_lhs->type == "number") {
						symbol_lhs->value = mpark::get<double>(symbol_rhs->value);
					}
					else if (symbol_lhs->type == "string") {
						symbol_lhs->value = mpark::get<std::string>(symbol_rhs->value);
					}
					else if (symbol_lhs->type == "boolean") {
						symbol_lhs->value = mpark::get<bool>(symbol_rhs->value);
					}
				}

			}
			else {
				std::cerr << "Assignment on right hand side is an undeclared variable!" << std::endl;
			}
		}
		else if (mpark::holds_alternative<node::NodeValueArithmeticExpression*>(assignment->rhs->var)) {
			//Check if type matches
			if (symbol_lhs->type == "number") {
				evaluate_arithmetic_expression(mpark::get<node::NodeValueArithmeticExpression*>(assignment->rhs->var)->expr);
				symbol_lhs->value = m_stack.top();
				m_stack.pop();
			}
			else {
				std::cerr << "lhs is not of type number" << std::endl;
				exit(EXIT_FAILURE);
			}

		}
		else if (mpark::holds_alternative<node::NodeValueStringExpression*>(assignment->rhs->var)) {
			//Check if type matches
			if (symbol_lhs->type == "string") {
				evaluate_string_expression(mpark::get<node::NodeValueStringExpression*>(assignment->rhs->var)->expr);
				symbol_lhs->value = m_stack.top();
				m_stack.pop();
			}
			else {
				std::cerr << "lhs is not of type number" << std::endl;
				exit(EXIT_FAILURE);
			}

		}
		else if (mpark::holds_alternative<node::NodeValueBooleanExpression*>(assignment->rhs->var)) {
			//Check if type matches
			if (symbol_lhs->type == "boolean") {
				evaluate_boolean_expression(mpark::get<node::NodeValueBooleanExpression*>(assignment->rhs->var)->expr);
				symbol_lhs->value = m_stack.top();
				m_stack.pop();
			}
			else {
				std::cerr << "lhs is not of type number" << std::endl;
				exit(EXIT_FAILURE);
			}

		}

	}
	else
	{
		std::cerr << "Assignment on left hand side is an undeclared variable!" << std::endl;
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

		// TODO: Implement object definitions 
		void operator()(const node::NodeObjectDefinition* obj_def) const {
			std::cerr << "object definition" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	mpark::visit(DefinitionVisitor{ this }, definition->var);
}

void Evaluator::evaluate_function_definition(const node::NodeFunctionDefinition* func_def)
{

}

size_t Evaluator::get_array_index(const node::NodeArithmeticExpr* expr)
{
	this->evaluate_arithmetic_expression(expr);
	size_t size = mpark::get<double>(this->m_stack.top());
	m_stack.pop();
	return size;
}
