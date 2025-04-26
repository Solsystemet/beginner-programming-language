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
			std::cout << "assignment" << std::endl;
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
			std::cout << "definition" << std::endl;
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
			std::cout << "array declare" << std::endl;
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
			evaluator->m_symbolTable.insert(boolean); // number decleration inserted into symbol table
		}

		// string expression
		void operator()(const node::NodeStringExpr* s_expr) const {
			std::cout << "string expression" << std::endl;
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
