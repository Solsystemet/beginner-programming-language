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

		// arithmetic expression
		void operator()(const node::NodeArithmeticExpr* a_expr) const {
			evaluator->evaluate_arithmetic_expression(a_expr);
			std::cout << mpark::get<double>(evaluator->m_stack.top()) << std::endl;
			evaluator->m_stack.pop();
		}

		// boolean expression
		void operator()(const node::NodeBooleanExpr* b_expr) const {
			std::cout << "boolean expression" << std::endl;
		}

		// string expression
		void operator()(const node::NodeStringExpr* s_expr) const {
			std::cout << "string expression" << std::endl;
		}

	}; mpark::visit(PrintVisitor{ this }, print_stmt->var);
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
			std::cout << "boolean expression" << std::endl;
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

		// function call
		void operator()(const node::NodeFunctionCall* function_call) const {
			std::cerr << "Function calls not implemented yet" << std::endl;
			exit(EXIT_FAILURE);
		}
	};
	mpark::visit(FactorVisitor{ this }, factor->var);
}
