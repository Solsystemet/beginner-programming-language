#include "parser.hpp"


node::NodeProg Parser::parse_prog() {
	node::NodeProg prog;
	// Rule 2
	// Stmts -> <Stmt><Stmts>
	while (peek())
	{
		if (node::NodeDecl* decl = parse_declaration()) {
			node::NodeStmt* stmt = new node::NodeStmt();
			stmt->var = decl;


			prog.stmts.push_back(stmt);
			std::cout << "Parsed declaration\n";
			continue;
		}
		//Parse a statement
		else if (node::NodeStmt* stmt = parse_stmt()) {
			//Push statement to program
			prog.stmts.push_back(stmt);
		}

		else {
			std::cerr << "Invalid statement" << std::endl;
		}
	}
	return prog;
}

node::NodeDecl* Parser::parse_declaration() {
	// Simple Declaration: <type> <identifier> = <expr>
	if (
		peek()->type == NUMBER &&
		peek(1) && peek(1)->type == IDENTIFIER &&
		peek(2) && peek(2)->type == EQUAL
		)

	{
		auto* simple_decl = new node::NodeSimpleDecl();
		consume();      // <type>
		simple_decl->identifier = consume();// <identifier>
		consume();

		auto* arithmetic_expr = parse_arithmetic_expr();
		if (arithmetic_expr == nullptr) {
			std::cerr << "Invalid expression in simple declaration after '=' at token index "
				<< m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}
		simple_decl->expr = arithmetic_expr;


		try_consume(NEW_LINE, "Expected newline after declaration");

		auto* decl = new node::NodeDecl();
		decl->var = simple_decl;
		return decl;
	}

	if (
		peek() && peek()->type == STRING &&
		peek(1) && peek(1)->type == IDENTIFIER &&
		peek(2) && peek(2)->type == EQUAL
		)

	{
		auto* simple_decl = new node::NodeSimpleDecl();
		consume();      // <type>
		simple_decl->identifier = consume();// <identifier>
		consume();

		auto* expr = parse_string_expr();
		if (expr == nullptr) {
			std::cerr << "Invalid expression in simple declaration after '=' at token index "
				<< m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}
		simple_decl->expr = expr;


		try_consume(NEW_LINE, "Expected newline after declaration");

		auto* decl = new node::NodeDecl();
		decl->var = simple_decl;
		return decl;
	}

	if (
		peek() && peek()->type == BOOLEAN &&
		peek(1) && peek(1)->type == IDENTIFIER &&
		peek(2) && peek(2)->type == EQUAL
		)

	{
		auto* simple_decl = new node::NodeSimpleDecl();
		consume();      // <type>
		simple_decl->identifier = consume();// <identifier>
		consume();

		auto* expr = parse_boolean_expr();
		if (expr == nullptr) {
			std::cerr << "Invalid expression in simple declaration after '=' at token index "
				<< m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}
		simple_decl->expr = expr;


		try_consume(NEW_LINE, "Expected newline after declaration");

		auto* decl = new node::NodeDecl();
		decl->var = simple_decl;
		return decl;
	}

	// Object Declaration: <identifier> : <newline> <indent> <simple_decl> <dedent>
	if (peek(0) && peek(0)->type == IDENTIFIER &&
		peek(1) && peek(1)->type == COLON &&
		peek(2) && peek(2)->type == NEW_LINE &&
		peek(3) && peek(3)->type == TAB_INDENT) {

		auto* object_decl = new node::NodeObjectDecl();
		object_decl->identifier = consume(); // identifier
		consume(); // COLON
		consume(); // NEW_LINE
		consume(); // INDENT

		// one or more simple declarations inside
		while (peek() &&
			(peek()->type == NUMBER || peek()->type == STRING || peek()->type == BOOLEAN)) {
			auto* properties_decl = parse_declaration();
			if (auto simple = mpark::get_if<node::NodeSimpleDecl*>(&properties_decl->var)) {
				object_decl->properties.push_back(*simple);
			}
			else {
				std::cerr << "Only simple declarations allowed inside object" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		try_consume(TAB_DEDENT, "Expected dedent after object declaration");

		auto* decl = new node::NodeDecl();
		decl->var = object_decl;
		return decl;
	}

	return nullptr;
}

node::NodeStmt* Parser::parse_stmt() {
	//Rule 3.2
	// Stmt -> print(<Expr>)
	if (peek() && peek()->type == PRINT &&
		peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
		// consume terminal symbols
		consume();
		consume();
		auto* node_stmt_print = new node::NodeStmtPrint();
		if (const auto string_expr = parse_string_expr()) {
			node_stmt_print->var = string_expr;
		}
		// Parse expression rule
		else if (const auto node_epxr = parse_arithmetic_expr()) {
			node_stmt_print->var = node_epxr;
		}

		else {
			std::cerr << "Invalid expression" << std::endl;
			exit(EXIT_FAILURE);
		}

		// consume terminal symbols
		try_consume(CLOSED_PARANTHESIS, "Exprected ')'");
		try_consume(EOF, "Expected newline after print statement");


		auto* node_stmt = new node::NodeStmt();
		node_stmt->var = node_stmt_print;
		return node_stmt;
	}

	return nullptr;

}

node::NodeArithmeticExpr* Parser::parse_arithmetic_expr() {
	node::NodeTerm* term = parse_term();

	if (term == nullptr) {
		return nullptr;
	}

	auto expr = new node::NodeArithmeticExpr();
	expr->var = term;


	try_consume_arithmetic(PLUS, expr, new node::NodeExprAdd());

	try_consume_arithmetic(MINUS, expr, new node::NodeExprSubtract());

	return expr;
}

node::NodeTerm* Parser::parse_term() {
	// <term> ::= <DECIMAL> | <IDENTIFIER
	auto* term = new node::NodeTerm();
	node::NodeFactor* factor = parse_factor();

	if (factor == nullptr) {
		return nullptr;
	}
	term->var = factor;


	try_consume_arithmetic(MULTIPLY, term, new node::NodeExprMult());

	try_consume_arithmetic(DIVIDE, term, new node::NodeExprDivide());

	try_consume_arithmetic(MODULO, term, new node::NodeExprModulo());


	return term;
}

node::NodeFactor* Parser::parse_factor() {
	Token* t;
	node::NodeFactor* nodefactor = nullptr;
	// Decimal
	if (node::NodeFactor* factor = try_consume_symbol(t, new node::NodeFactorDecimal()))
		nodefactor = factor;


	// Identifier
	if (node::NodeFactor* factor = try_consume_symbol(t, new node::NodeFactorIdentifier()))
		nodefactor = factor;

	// (<AExpr>)
	if (node::NodeFactor* factor = try_consume_symbol(t, new node::NodeArithmeticExpr()))
		nodefactor = factor;

	return nodefactor;
}

node::NodeStringExpr* Parser::parse_string_expr() {
	Token* t = try_consume(STRING_VAL);
	if (t == nullptr) {
		return nullptr;
	}


	auto* string_expr = new node::NodeStringExpr();
	string_expr->var = *t;

	if (peek()->type == PLUS) {
		consume();
		auto* concat = new node::NodeStringExprConcat();
		concat->lhs = string_expr;

		node::NodeStringExpr* rhs = parse_string_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected string expression after '+'" << std::endl;
			exit(EXIT_FAILURE);
		}
		concat->rhs = rhs;
		string_expr->var = concat;
	}
	return string_expr;
}

node::NodeBooleanExpr* Parser::parse_boolean_expr()
{
	node::NodeBooleanExpr* result = new node::NodeBooleanExpr();
	node::NodeBooleanOr* _or = parse_or();

	if (_or != nullptr) {
		result->expr = _or;
		return result;
	}

	return nullptr;
}

node::NodeBooleanOr* Parser::parse_or()
{
	node::NodeBooleanOr* result = new node::NodeBooleanOr();
	// Assume and
	node::NodeBooleanAnd* _and = parse_and();

	if (_and != nullptr) {
		// check for operator
		if (peek()->type == OR) {
			node::NodeBooleanOr* lhs = new node::NodeBooleanOr();
			lhs->var = _and;
			node::NodeBooleanOrOperation* and_op = parse_or_op(lhs);
			if (and_op != nullptr) {
				result->var = and_op;
				return result;
			}
		}

		result->var = _and;
		return result;
	}
	return nullptr;
}

node::NodeBooleanOrOperation* Parser::parse_or_op(node::NodeBooleanOr* lhs)
{
	Token* _or = try_consume(OR);

	if (_or != nullptr) {
		node::NodeBooleanAnd* rhs = parse_and();
		if (rhs == nullptr) {
			std::cerr << "Expected real expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanOrOperation* result = new node::NodeBooleanOrOperation();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanAnd* Parser::parse_and()
{
	node::NodeBooleanAnd* result = new node::NodeBooleanAnd();

	// Assume equal
	node::NodeBooleanEqual* equal = parse_equal();

	if (equal != nullptr) {
		// Check for operator
		if (peek()->type == AND) {
			node::NodeBooleanAnd* lhs = new node::NodeBooleanAnd();
			lhs->var = equal;
			node::NodeBooleanAndOperation* and_op = parse_and_op(lhs);
			if (and_op != nullptr) {
				result->var = and_op;
				return result;
			}
		}

		result->var = equal;
		return result;
	}

	return nullptr;
}

node::NodeBooleanAndOperation* Parser::parse_and_op(node::NodeBooleanAnd* lhs)
{
	Token* _and = try_consume(AND);

	if (_and != nullptr) {
		node::NodeBooleanEqual* rhs = parse_equal();
		if (rhs == nullptr) {
			std::cerr << "Expected real expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanAndOperation* result = new node::NodeBooleanAndOperation();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanEqual* Parser::parse_equal()
{
	node::NodeBooleanEqual* result = new node::NodeBooleanEqual();
	// Assume Real expression
	node::NodeBooleanRealExpression* real_expr_lhs = parse_real_expr();

	if (real_expr_lhs != nullptr) {
		// check for operators
		
		// is not
		if (peek()->type == IS && peek(1)->type == NOT) {
			node::NodeBooleanEqual* lhs = new node::NodeBooleanEqual();
			lhs->var = real_expr_lhs;
			node::NodeBooleanEqualIsNot* isnot = parse_equal_is_not(lhs);
			if (isnot != nullptr) {
				result->var = isnot;
				return result;
			}
		}

		// is not
		if (peek()->type == IS) {
			node::NodeBooleanEqual* lhs = new node::NodeBooleanEqual();
			lhs->var = real_expr_lhs;
			node::NodeBooleanEqualIs* is = parse_equal_is(lhs);
			if (is != nullptr) {
				result->var = is;
				return result;
			}
		}
		
		result->var = real_expr_lhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanEqualIsNot* Parser::parse_equal_is_not(node::NodeBooleanEqual* lhs)
{
	Token* is = try_consume(IS);
	Token* _not = try_consume(NOT);

	if (is != nullptr && _not != nullptr) {
		node::NodeBooleanRealExpression* rhs = parse_real_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected real expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanEqualIsNot* result = new node::NodeBooleanEqualIsNot();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanEqualIs* Parser::parse_equal_is(node::NodeBooleanEqual* lhs)
{
	Token* is = try_consume(IS);

	if (is != nullptr) {
		node::NodeBooleanRealExpression* rhs = parse_real_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected real expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanEqualIs* result = new node::NodeBooleanEqualIs();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanRealExpression* Parser::parse_real_expr()
{
	node::NodeBooleanRealExpression* result = new node::NodeBooleanRealExpression();

	// Assume arithmetic expression
	node::NodeArithmeticExpr* expr_lhs = parse_arithmetic_expr();
	if (expr_lhs != nullptr) {
		// greater equal
		if (peek()->type == GREATER && peek(1)->type == EQUAL) {
			node::NodeBooleanGreaterEqual* ge = parse_greater_equal(expr_lhs);
			if (ge != nullptr) {
				result->var = ge;
				return result;
			}
		}

		// less equal
		if (peek()->type == LESS && peek(1)->type == EQUAL) {
			node::NodeBooleanLessEqual* le = parse_less_equal(expr_lhs);
			if (le != nullptr) {
				result->var = le;
				return result;
			}
		}

		// greater
		if (peek()->type == GREATER) {
			node::NodeBooleanGreater* g = parse_greater(expr_lhs);
			if (g != nullptr) {
				result->var = g;
				return result;
			}
		}

		// less
		if (peek()->type == LESS) {
			node::NodeBooleanLess* l = parse_less(expr_lhs);
			if (l != nullptr) {
				result->var = l;
				return result;
			}
		}
	}

	// if no arithmetic expression
	node::NodeBooleanNot* not_result = parse_not();

	if (not_result != nullptr) {
		result->var = not_result;
		return result;
	}

	return nullptr;
}

node::NodeBooleanGreaterEqual* Parser::parse_greater_equal(node::NodeArithmeticExpr* lhs)
{
	Token* greater = try_consume(GREATER);
	Token* equal = try_consume(EQUAL);

	if (greater != nullptr && equal != nullptr) {
		node::NodeArithmeticExpr* rhs = parse_arithmetic_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected arithmetic expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanGreaterEqual* result = new node::NodeBooleanGreaterEqual();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanLessEqual* Parser::parse_less_equal(node::NodeArithmeticExpr* lhs)
{
	Token* less = try_consume(LESS);
	Token* equal = try_consume(EQUAL);

	if (less != nullptr && equal != nullptr) {
		node::NodeArithmeticExpr* rhs = parse_arithmetic_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected arithmetic expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanLessEqual* result = new node::NodeBooleanLessEqual();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanGreater* Parser::parse_greater(node::NodeArithmeticExpr* lhs)
{
	Token* greater = try_consume(GREATER);

	if (greater != nullptr) {
		node::NodeArithmeticExpr* rhs = parse_arithmetic_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected arithmetic expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanGreater* result = new node::NodeBooleanGreater();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanLess* Parser::parse_less(node::NodeArithmeticExpr* lhs)
{
	Token* less = try_consume(LESS);

	if (less != nullptr) {
		node::NodeArithmeticExpr* rhs = parse_arithmetic_expr();
		if (rhs == nullptr) {
			std::cerr << "Expected arithmetic expression at: " << m_currentIndex << std::endl;
			exit(EXIT_FAILURE);
		}

		node::NodeBooleanLess* result = new node::NodeBooleanLess();
		result->lhs = lhs;
		result->rhs = rhs;
		return result;

	}

	return nullptr;
}

node::NodeBooleanNot* Parser::parse_not()
{
	// Assume not since they are recursive with itself
	
	node::NodeBooleanNot* result = new node::NodeBooleanNot();
	if (peek()->type == NOT) {
		node::NodeBooleanNotOperation* notop = parse_not_op();

		if (notop != nullptr) {
		result->var = notop;
		return result;
		}
	}

	// no not tokens now expects factor
	node::NodeBooleanFactor* factor = parse_boolean_factor();

	if (factor != nullptr) {
		result->var = factor;
		return result;
	}

	return nullptr;
}

node::NodeBooleanNotOperation* Parser::parse_not_op()
{
	Token* t = try_consume(NOT);
	if (t != nullptr) {
		node::NodeBooleanNotOperation* op = new node::NodeBooleanNotOperation();
		node::NodeBooleanNot* expr = parse_not();
		if (expr != nullptr) {
			op->expr = expr;
			return op;
		}
	}
	//lowkey would never happen because "not" token is already verified.
	return nullptr;
}

node::NodeBooleanFactor* Parser::parse_boolean_factor()
{
	
	node::NodeBooleanFactor* nodefactor = new node::NodeBooleanFactor();
	Token* t;
	// bool value
	t = try_consume(BOOLVAL);
	if (t != nullptr) {
		node::NodeBooleanFactorValue* value = new node::NodeBooleanFactorValue();
		value->boolval = *t;
		nodefactor->var = value;
		return nodefactor;
	}

	// TODO: Make statement for function calls


	// Identifier
	t = try_consume(IDENTIFIER);
	if (t != nullptr) {
		node::NodeBooleanFactorIdentifier* value = new node::NodeBooleanFactorIdentifier();
		value->identifier = *t;
		nodefactor->var = value;
		return nodefactor;
	}

	// (<BExpr>)
	t = try_consume(OPEN_PARANTHESIS);
	if (t != nullptr) {
		node::NodeBooleanExpr* expr = parse_boolean_expr();
		try_consume(CLOSED_PARANTHESIS, "Expected ')'");
		nodefactor->var = expr;
		return nodefactor;
	}

	return nullptr;
}
