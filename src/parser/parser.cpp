#include "parser.hpp"


node::NodeProg Parser::parse_prog() {
	node::NodeProg prog;
	// Rule 2
	// Stmts -> <Stmt><Stmts>;
	while (peek() != nullptr && peek()->type != -1)
	{
		if (node::NodeStmt* stmt = parse_stmt()) {
			//Push statement to program
			prog.stmts.push_back(stmt);
		}

		else {
			std::cerr << "Invalid statement" << std::endl;
		}
	}
	return prog;
}



node::NodeStmt* Parser::parse_stmt() {
	
	// <Stmt> -> <Decleration>
	if (node::NodeDecl* decl = parse_decleration()) {
		node::NodeStmt* stmt = new node::NodeStmt();
		stmt->var = decl;
		return stmt;
	}

	// <Stmt> -> <Function Call>
	if (node::NodeFunctionCall* func_Call = parse_function_Call()) {
		node::NodeStmt* stmt = new node::NodeStmt();
		stmt->var = func_Call;
		try_consume(NEW_LINE, "Expected new_line after function call");
		return stmt;
	}

	if (node::NodeAssignment* assignment = parse_assignment()) {
		node::NodeStmt* stmt = new node::NodeStmt();
		stmt->var = assignment;
		try_consume(NEW_LINE, "Expected new_line after assignment");
		return stmt;
	}

	if (node::NodeGlobalControlFlow* controlFlow = parse_global_control_flow()) {
		node::NodeStmt* stmt = new node::NodeStmt();
		stmt->var = controlFlow;
		return stmt;
	}

	if (node::NodeDefinition* definition = parse_definition()) {
		node::NodeStmt* stmt = new node::NodeStmt();
		stmt->var = definition;
		return stmt;
	}

	// Special print function call
	if (peek() && peek()->type == PRINT &&
		peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
		// consume terminal symbols
		consume();
		consume();
		auto* node_stmt_print = new node::NodeStmtPrint();
		node_stmt_print->value = parse_value();

		// consume terminal symbols
		try_consume(CLOSED_PARANTHESIS, "Exprected ')'");
		if (try_consume(NEW_LINE) || try_consume(EOF)) {
			auto* node_stmt = new node::NodeStmt();
			node_stmt->var = node_stmt_print;
			return node_stmt;
		}
		else {
			std::cerr << "Expected new line or end of file in print" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	return nullptr;

}

node::NodeDecl* Parser::parse_decleration() {
	
	if (node::NodeSimpleDecl* simp_decl = parse_simple_decleration()) {
		node::NodeDecl* decl = new node::NodeDecl();
		decl->var = simp_decl;
		return decl;
	}

	if (node::NodeArrayDecl* arr_decl = parse_array_decleration()) {
		node::NodeDecl* decl = new node::NodeDecl();
		decl->var = arr_decl;
		return decl;
	}

	if (node::NodeObjectDecl* obj_decl = parse_object_decleration()) {
		node::NodeDecl* decl = new node::NodeDecl();
		decl->var = obj_decl;
		return decl;
	}

	

	return nullptr;
}

node::NodeSimpleDecl* Parser::parse_simple_decleration()
{

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
		size_t verify_arithmetic = 0;
		if (verify_arithmetic_expr(&verify_arithmetic)) {
			auto* arithmetic_expr = parse_arithmetic_expr();
			if (arithmetic_expr == nullptr) {
				std::cerr << "Invalid expression in simple declaration after '=' at token index "
					<< m_currentIndex << std::endl;
				exit(EXIT_FAILURE);
			}
			simple_decl->expr = arithmetic_expr;


			try_consume(NEW_LINE, "Expected newline after declaration");
			return simple_decl;
		}
		
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
		size_t verify_str = 0;
		if (verify_string_expr(&verify_str)) {
			auto* expr = parse_string_expr();
			if (expr == nullptr) {
				std::cerr << "Invalid expression in simple declaration after '=' at token index "
					<< m_currentIndex << std::endl;
				exit(EXIT_FAILURE);
			}
			simple_decl->expr = expr;


			try_consume(NEW_LINE, "Expected newline after declaration");
			return simple_decl;
		}
		
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
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			auto* expr = parse_boolean_expr();
			if (expr == nullptr) {
				std::cerr << "Invalid expression in simple declaration after '=' at token index "
					<< m_currentIndex << std::endl;
				exit(EXIT_FAILURE);
			}
			simple_decl->expr = expr;


			try_consume(NEW_LINE, "Expected newline after declaration");
			return simple_decl;
		}
		
	}

	return nullptr;
}

node::NodeArrayDecl* Parser::parse_array_decleration()
{
	// number[] x =
	if (
		peek()->type == NUMBER &&
		peek(1) && peek(1)->type == OPEN_SQUAREBRACKET &&
		peek(2) && peek(2)->type == CLOSED_SQUAREBRACKET&&
		peek(3) && peek(3)->type == IDENTIFIER &&
		peek(4) && peek(4)->type == EQUAL
		)

	{
		auto* arr = new node::NodeArrayDecl();
		auto* numberArray = new node::NodeNumberArrayDecl();
		arr->type = consume();
		consume(); // [
		consume(); // ]
		arr->identifier = consume();// <identifier>
		consume(); // =

		//Assumes empty array
		if (
			peek() && peek()->type == NUMBER &&
			peek(1) && peek(1)->type == OPEN_SQUAREBRACKET
			) {
			consume(); // Number
			consume(); // [

			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				numberArray->size = expr;
			}
			else
			{
				std::cerr << "Expected arithmetic expression for size allocation" << std::endl;
			}

			try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expressions for size allocation");
			arr->var = numberArray;
			try_consume(NEW_LINE, "Expected newline after declaration");
			return arr;
		}
		// Now assume arithmetic expressions 
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume(); // [
			size_t size = 0;
			do
			{
				size_t verify_arithmetic = 0;
				if (verify_arithmetic_expr(&verify_arithmetic)) {
					auto* arithmetic_expr = parse_arithmetic_expr();
					numberArray->elements.push_back(arithmetic_expr);
					size++;
					if (peek() && peek()->type == CLOSED_SQUAREBRACKET) {
						consume(); // ]
						break;
					}
				}
				else {
					std::cerr << "Invalid expression in array declaration after '=' at token index "
						<< m_currentIndex << std::endl;
					exit(EXIT_FAILURE);
				}
			} while (consume().type == COMMA);
			numberArray->size = size;
			arr->var = numberArray;
		}


		try_consume(NEW_LINE, "Expected newline after declaration");
		return arr;
	}

	// string[] x =
	if (
		peek()->type == STRING &&
		peek(1) && peek(1)->type == OPEN_SQUAREBRACKET &&
		peek(2) && peek(2)->type == CLOSED_SQUAREBRACKET &&
		peek(3) && peek(3)->type == IDENTIFIER &&
		peek(4) && peek(4)->type == EQUAL
		)

	{
		auto* arr = new node::NodeArrayDecl();
		auto* stringArray = new node::NodeStringArrayDecl();
		arr->type = consume();      // <type>
		consume(); // [
		consume(); // ]
		arr->identifier = consume();// <identifier>
		consume(); // =

		//Assumes empty array
		if (
			peek() && peek()->type == STRING &&
			peek(1) && peek(1)->type == OPEN_SQUAREBRACKET
			) {
			consume(); // IDENTIFIER
			consume(); // [
			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				stringArray->size = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expressions for size allocation");
				arr->var = stringArray;
				try_consume(NEW_LINE, "Expected newline after declaration");
				return arr;
			}
			else
			{
				std::cerr << "Expected arithmetic expression for size allocation" << std::endl;
			}

			
		}
		// Now assume arithmetic expressions 
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume(); // [
			size_t size = 0;
			do
			{
				size_t verify_str = 0;
				if (verify_string_expr(&verify_str)) {
					auto* string_expr = parse_string_expr();
					stringArray->elements.push_back(string_expr);
					size++;
					if (peek() && peek()->type == CLOSED_SQUAREBRACKET) {
						consume(); // ]
						break;
					}
				}
				else {
					std::cerr << "Invalid expression in array declaration after '=' at token index "
						<< m_currentIndex << std::endl;
					exit(EXIT_FAILURE);
				}
				
			} while (consume().type == COMMA);
			stringArray->size = size;
			arr->var = stringArray;
		}


		try_consume(NEW_LINE, "Expected newline after declaration");
		return arr;
	}

	// boolean[] x =
	if (
		peek()->type == BOOLEAN &&
		peek(1) && peek(1)->type == OPEN_SQUAREBRACKET &&
		peek(2) && peek(2)->type == CLOSED_SQUAREBRACKET &&
		peek(3) && peek(3)->type == IDENTIFIER &&
		peek(4) && peek(4)->type == EQUAL
		)

	{
		auto* arr = new node::NodeArrayDecl();
		auto* booleanArray = new node::NodeBooleanArrayDecl();
		arr->type = consume();      // <type>
		consume(); // [
		consume(); // ]
		arr->identifier = consume();// <identifier>
		consume(); // =

		if (
			peek() && peek()->type == BOOLEAN &&
			peek(1) && peek(1)->type == OPEN_SQUAREBRACKET
			) {
			consume(); // IDENTIFIER
			consume(); // [
			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				booleanArray->size = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expressions for size allocation");
				arr->var = booleanArray;
				try_consume(NEW_LINE, "Expected newline after declaration");
				return arr;
			}
			else
			{
				std::cerr << "Expected arithmetic expression for size allocation" << std::endl;
			}
		}
		// Now assume boolean expressions 
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume(); // [
			size_t size = 0;
			do
			{
				size_t verify_bool = 0;
				if (verify_boolean_expr(&verify_bool)) {
					auto* boolean_expr = parse_boolean_expr();
					booleanArray->elements.push_back(boolean_expr);
					size++;
					if (peek() && peek()->type == CLOSED_SQUAREBRACKET) {
						consume(); // ]
						break;
					}
				}
				else {
					std::cerr << "Invalid expression in array declaration after '=' at token index "
						<< m_currentIndex << std::endl;
					exit(EXIT_FAILURE);
				}
				
			} while (consume().type == COMMA);
			booleanArray->size = size;
			arr->var = booleanArray;
		}

		try_consume(NEW_LINE, "Expected newline after declaration");
		return arr;
	}

	// person[] x =
	if (
		peek()->type == IDENTIFIER &&
		peek(1) && peek(1)->type == OPEN_SQUAREBRACKET &&
		peek(2) && peek(2)->type == CLOSED_SQUAREBRACKET &&
		peek(3) && peek(3)->type == IDENTIFIER &&
		peek(4) && peek(4)->type == EQUAL
		)

	{
		auto* arr = new node::NodeArrayDecl();
		auto* objArray = new node::NodeObjectArrayDecl();
		arr->type = consume();     // <type>
		consume(); // [
		consume(); // ]
		arr->identifier = consume();// <identifier>
		consume(); // =

		//Assumes size array
		if (
			peek() && peek()->type == IDENTIFIER &&
			peek() && peek()->value == arr->type.value &&
			peek(1) && peek(1)->type == OPEN_SQUAREBRACKET
			) {
			consume(); // IDENTIFIER
			consume(); // [
			
			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				objArray->size = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expressions for size allocation");
				arr->var = objArray;
				try_consume(NEW_LINE, "Expected newline after declaration");
				return arr;
			}
			else
			{
				std::cerr << "Expected arithmetic expression for size allocation" << std::endl;
			}

			
		}
		// Now assume boolean expressions 
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume(); // [
			size_t size = 0;
			do
			{
				auto* obj_decl = parse_object_decleration();
				if (obj_decl == nullptr) {
					std::cerr << "Invalid expression in array declaration after '=' at token index "
						<< m_currentIndex << std::endl;
					exit(EXIT_FAILURE);
				}
				objArray->elements.push_back(obj_decl);
				size++;
				if (peek() && peek()->type == CLOSED_SQUAREBRACKET) {
					consume(); // ]
					break;
				}
			} while (consume().type == COMMA);
				objArray->size = size;
				arr->var = objArray;
		}

		try_consume(NEW_LINE, "Expected newline after declaration");
		return arr;
	}
	
	return nullptr;
}

node::NodeObjectDecl* Parser::parse_object_decleration()
{
	// Object Declaration: <identifier><identifier> : <newline> <indent> <assignments> <dedent>
	if (peek(0) && peek(0)->type == IDENTIFIER &&
		peek(1) && peek(1)->type == IDENTIFIER &&
		peek(2) && peek(2)->type == COLON &&
		peek(3) && peek(3)->type == NEW_LINE &&
		peek(4) && peek(4)->type == TAB_INDENT) {

		auto* object_decl = new node::NodeObjectDecl();
		object_decl->objectType = consume(); // identifier for object type
		object_decl->identifier = consume(); // identifier for variable name
		consume(); // COLON
		consume(); // NEW_LINE
		consume(); // INDENT

		// assign values
		while (node::NodeAssignment* assignment = parse_assignment())
		{
			object_decl->properties.push_back(assignment);
			try_consume(NEW_LINE, "Expected 'new line' after assignment in obj decleration");
		}

		try_consume(TAB_DEDENT, "Expected dedent after object declaration");
		return object_decl;
	}

	return nullptr;
}

node::NodeNestedStmt* Parser::parse_nested_stmt()
{
	// <Stmt> -> <Decleration>
	if (node::NodeDecl* decl = parse_decleration()) {
		node::NodeNestedStmt* stmt = new node::NodeNestedStmt();
		stmt->var = decl;
		return stmt;
	}

	// <Stmt> -> <Function Call>
	if (node::NodeFunctionCall* func_Call = parse_function_Call()) {
		node::NodeNestedStmt* stmt = new node::NodeNestedStmt();
		stmt->var = func_Call;
		try_consume(NEW_LINE, "Expected new_line after function call");
		return stmt;
	}

	if (node::NodeAssignment* assignment = parse_assignment()) {
		node::NodeNestedStmt* stmt = new node::NodeNestedStmt();
		stmt->var = assignment;
		try_consume(NEW_LINE, "Expected new_line after assignment");
		return stmt;
	}

	if (node::NodeGlobalControlFlow* controlFlow = parse_global_control_flow()) {
		node::NodeNestedStmt* stmt = new node::NodeNestedStmt();
		stmt->var = controlFlow;
		try_consume(NEW_LINE, "Expected new_line after control flow");
		return stmt;
	}

	// Special print function call
	if (peek() && peek()->type == PRINT &&
		peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
		// consume terminal symbols
		consume();
		consume();
		auto* node_stmt_print = new node::NodeStmtPrint();
		node_stmt_print->value = parse_value();

		// consume terminal symbols
		try_consume(CLOSED_PARANTHESIS, "Exprected ')'");
		if (try_consume(NEW_LINE) || try_consume(EOF)) {
			node::NodeNestedStmt* stmt = new node::NodeNestedStmt();
			stmt->var = node_stmt_print;
			return stmt;
		}
		else {
			std::cerr << "Expected new line or end of file in print" << std::endl;
			exit(EXIT_FAILURE);
		}
	}


	return nullptr;
}

node::NodeFunctionStmt* Parser::parse_function_stmt()
{
	// <Stmt> -> <Decleration>
	if (node::NodeDecl* decl = parse_decleration()) {
		node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
		stmt->var = decl;
		return stmt;
	}

	// <Stmt> -> <Function Call>
	if (node::NodeFunctionCall* func_Call = parse_function_Call()) {
		node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
		stmt->var = func_Call;
		try_consume(NEW_LINE, "Expected new_line after function call");
		return stmt;
	}

	if (node::NodeAssignment* assignment = parse_assignment()) {
		node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
		stmt->var = assignment;
		try_consume(NEW_LINE, "Expected new_line after assignment");
		return stmt;
	}

	if (node::NodeFunctionControlFlow* controlFlow = parse_function_control_flow()) {
		node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
		stmt->var = controlFlow;
		try_consume(NEW_LINE, "Expected new_line after control flow");
		return stmt;
	}

	if (peek() && peek()->type == RETURN) {
		consume();
		node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
		node::NodeFunctionReturn* returnstmt = new node::NodeFunctionReturn();
		size_t verify_Val = 0;
		if (verify_value(&verify_Val)) {
			node::NodeValue* val = parse_value();
			returnstmt->val = val;
		}
		try_consume(NEW_LINE, "Expected new_line after return");
		stmt->var = returnstmt;
		return stmt;
	}

	// Special print function call
	if (peek() && peek()->type == PRINT &&
		peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
		// consume terminal symbols
		consume();
		consume();
		auto* node_stmt_print = new node::NodeStmtPrint();
		node_stmt_print->value = parse_value();

		// consume terminal symbols
		try_consume(CLOSED_PARANTHESIS, "Exprected ')'");
		if (try_consume(NEW_LINE) || try_consume(EOF)) {
			node::NodeFunctionStmt* stmt = new node::NodeFunctionStmt();
			stmt->var = node_stmt_print;
			return stmt;
		}
		else {
			std::cerr << "Expected new line or end of file in print" << std::endl;
			exit(EXIT_FAILURE);
		}
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

	if (node::NodeFunctionCall* func_Call = parse_function_Call()) {
		nodefactor->var = func_Call;
	}

	return nodefactor;
}

node::NodeStringExpr* Parser::parse_string_expr() {
	auto* string_expr = new node::NodeStringExpr();

	if (Token* t = try_consume(STRING_VAL)) {
		node::NodeStringValue* val = new node::NodeStringValue();
		val->value = *t;
		string_expr->var = val;

		if (peek()->type == PLUS) {
			consume();
			auto* concat = new node::NodeStringExprConcat();
			concat->lhs = new node::NodeStringExpr();
			concat->lhs->var = string_expr->var;

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
	else if (Token* t = try_consume(IDENTIFIER)) {
		node::NodeStringIdentifier* ident = new node::NodeStringIdentifier();
		ident->ident = *t;
		string_expr->var = ident;

		if (peek()->type == PLUS) {
			consume();
			auto* concat = new node::NodeStringExprConcat();
			concat->lhs = new node::NodeStringExpr();
			concat->lhs->var = string_expr->var;

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
	else if (node::NodeFunctionCall* func_Call = parse_function_Call()) {
		string_expr->var = func_Call;

		if (peek()->type == PLUS) {
			consume();
			auto* concat = new node::NodeStringExprConcat();
			concat->lhs = new node::NodeStringExpr();
			concat->lhs->var = string_expr->var;

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

	return nullptr;
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
		result->lhs = new node::NodeBooleanOr();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeBooleanAnd();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeBooleanEqual();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeBooleanEqual();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeArithmeticExpr();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeArithmeticExpr();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeArithmeticExpr();
		result->lhs->var = lhs->var;
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
		result->lhs = new node::NodeArithmeticExpr();
		result->lhs->var = lhs->var;
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

	if (node::NodeFunctionCall* func_call = parse_function_Call()) {
		nodefactor->var = func_call;
		return nodefactor;
	}

	return nullptr;
}

node::NodeFunctionCall* Parser::parse_function_Call()
{
	if (peek() && peek()->type == IDENTIFIER &&
		peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
		node::NodeFunctionCall* function_call = new node::NodeFunctionCall();

		function_call->functionName = consume(); // identifier
		consume();

		do
		{
			if (node::NodeValue* val = parse_value()) {
				node::NodeArgs* arg = new node::NodeArgs();
				arg->value = val;
				function_call->args.push_back(arg);
			}

		} while (try_consume(COMMA));
		try_consume(CLOSED_PARANTHESIS, "Expected closed paranthesis after function call args");
		
		return function_call;
	}
	return nullptr;
}

node::NodeValue* Parser::parse_value()
{
	node::NodeValue* val = new node::NodeValue();

	// Assume normal function call
	if (node::NodeFunctionCall* function_call = parse_function_Call()) {
		node::NodeValueFunctionCall* v_func_call = new node::NodeValueFunctionCall();
		v_func_call->functionCall = function_call;

		// Verify if properties are there after function call (also assume function call returns an object)
		if (peek() && peek()->type == DOT) {
			consume(); // .
			node::NodeValueFunctionCallProperty* fc_props = new node::NodeValueFunctionCallProperty();
			node::NodeValueIdentifierProperty* ident_prop = new node::NodeValueIdentifierProperty();

			fc_props->functionCall = function_call;
			fc_props->identifierproperties = ident_prop;

			ident_prop->identfierHead = function_call->functionName;

			if (Token* t = try_consume(IDENTIFIER,
				"Expected identifier after '.' after a function call")) {
				ident_prop->identifierproperties.push_back(*t);
			}

			while (peek() && peek()->type == DOT)
			{
				ident_prop->identifierproperties.push_back(*try_consume(IDENTIFIER,
					"Expected identifier after '.' after a function call"));
			}

			//Check if it is an array
			if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
				consume();
				if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
					val->index = expr;
					try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
				}
			}
			val->var = fc_props;
			return val;
		}

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
				val->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
		}

		val->var = v_func_call;
		return val;
	}
	size_t verify_operator = 0;
	// value returns identifier (can be of any type)
	if (peek() && peek()->type == IDENTIFIER && operator_check(&verify_operator) == false) {
		node::NodeValueIdentifier* val_ident = new node::NodeValueIdentifier();
		val_ident->identifier = consume();

		// Verify if properties are there after identifier (also assume identifier is an object)
		if (peek() && peek()->type == DOT) {
			consume(); // .
			node::NodeValueIdentifierProperty* ident_props = new node::NodeValueIdentifierProperty();
			ident_props->identfierHead = val_ident->identifier;
			ident_props->identifierproperties.push_back(*try_consume(IDENTIFIER,
				"Expected identifier after '.' after a function call"));
			while (peek() && peek()->type == DOT)
			{
				ident_props->identifierproperties.push_back(*try_consume(IDENTIFIER,
					"Expected identifier after '.' after a function call"));
			}

			//Check if it is an array
			if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
				consume();
				if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
					val->index = expr;
					try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
				}
			}
			val->var = ident_props;
			return val;
		}

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
				val->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
		}

		val->var = val_ident;
		return val;

	}


	// value returns boolean expression
	size_t verify_bool = 0;
	if (verify_boolean_expr(&verify_bool)) {
		node::NodeBooleanExpr* b_epxr = parse_boolean_expr();
		node::NodeValueBooleanExpression* val_expr = new node::NodeValueBooleanExpression();
		val_expr->expr = b_epxr;
		val->var = val_expr;

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
				val->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
		}

		return val;
	}

	// value returns arithmetic expression
	size_t verify_arithmetic = 0;
	if (verify_arithmetic_expr(&verify_arithmetic)) {
		node::NodeArithmeticExpr* a_epxr = parse_arithmetic_expr();
		node::NodeValueArithmeticExpression* val_expr = new node::NodeValueArithmeticExpression();
		val_expr->expr = a_epxr;
		val->var = val_expr;

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
				val->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
		}

		return val;
	}

	// value returns string expression
	if (node::NodeStringExpr* s_epxr = parse_string_expr()) {
		node::NodeValueStringExpression* val_expr = new node::NodeValueStringExpression();
		val_expr->expr = s_epxr;
		val->var = val_expr;

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			if (node::NodeArithmeticExpr* expr = parse_arithmetic_expr()) {
				val->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
		}

		return val;
	}

	return nullptr;
}

node::NodeAssignment* Parser::parse_assignment()
{
	node::NodeAssignment* assignment = new node::NodeAssignment();

	//assume identfier with properties first
	if (peek() && peek()->type == IDENTIFIER &&
		peek(1) && peek(1)->type == DOT) {
		assignment->identifierHead = consume(); // identifier
		consume();
		
		do {
			if (Token* t = try_consume(IDENTIFIER)) {

				assignment->props.push_back(*t);
			}
		} while (try_consume(DOT));

		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				assignment->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
			else {
				std::cerr << "Expected arithmetic expression in array assignment" << std::endl;
			}
		}

		try_consume(EQUAL, "Expects '=' after identifier properties assignment");

		if (node::NodeValue* val = parse_value()) {
			assignment->rhs = val;
			return assignment;
		}

	}
	else if (peek() && peek()->type == IDENTIFIER) {

		assignment->identifierHead = consume(); // identifier
		
		//Check if it is an array
		if (peek() && peek()->type == OPEN_SQUAREBRACKET) {
			consume();
			size_t verify_arithmetic = 0;
			if (verify_arithmetic_expr(&verify_arithmetic)) {
				node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
				assignment->index = expr;
				try_consume(CLOSED_SQUAREBRACKET, "Expected ']' after arithmetic expression assignment");
			}
			else {
				std::cerr << "Expected arithmetic expression in array assignment" << std::endl;
			}
		}

		try_consume(EQUAL, "Expects '=' after lhs");
		size_t verify_val = 0;
		if (verify_value(&verify_val)) {
			node::NodeValue* val = parse_value();
			assignment->rhs = val;
			return assignment;
		}

	}

	return nullptr;
}

node::NodeGlobalControlFlow* Parser::parse_global_control_flow()
{
	node::NodeGlobalControlFlow* controlFlow = new node::NodeGlobalControlFlow();
	if (node::NodeGlobalIf* _if = parse_global_if()) {
		controlFlow->var = _if;
		return controlFlow;
	}

	if (node::NodeGlobalLoop* _loop = parse_global_loop()) {
		controlFlow->var = _loop;
		return controlFlow;
	}

	return nullptr;
}

node::NodeGlobalIf* Parser::parse_global_if()
{

	if (peek() && peek()->type == IF) {
		consume(); // if
		node::NodeGlobalIf* _if = new node::NodeGlobalIf();
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_if->condition = expr;
			try_consume(COLON, "Expected ':' after boolean expr in global if");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

			while (node::NodeNestedStmt* stmt = parse_nested_stmt()) {
				_if->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

			while (node::NodeGlobalElseIf* _elseif = parse_global_else_if())
			{
				_if->elseifs.push_back(_elseif);
			}

			if (node::NodeGlobalElse* _else = parse_global_else()) {
				_if->_else = _else;
			}

			return _if;
		}
		else
		{
			std::cerr << "Expected boolean expression after 'if'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return nullptr;
}

node::NodeGlobalElseIf* Parser::parse_global_else_if()
{
	if (peek() && peek()->type == ELSE &&
		peek(1) && peek(1)->type == IF) {
		consume(); // else
		consume(); // if
		node::NodeGlobalElseIf* _elseif = new node::NodeGlobalElseIf();
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_elseif->condition = expr;
			try_consume(COLON, "Expected ':' after boolean expr in global if");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

			while (node::NodeNestedStmt* stmt = parse_nested_stmt()) {
				_elseif->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after else if statement");
			return _elseif;
		}
		else
		{
			std::cerr << "Expected boolean expression after 'if'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return nullptr;
}

node::NodeGlobalElse* Parser::parse_global_else()
{
	if (peek() && peek()->type == ELSE) {
		consume(); // else
		node::NodeGlobalElse* _else = new node::NodeGlobalElse();

		try_consume(COLON, "Expected ':' after boolean expr in global if");
		try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
		try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

		while (node::NodeNestedStmt* stmt = parse_nested_stmt()) {
			_else->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

		return _else;

			
	}

	return nullptr;
}

node::NodeGlobalLoop* Parser::parse_global_loop()
{
	node::NodeGlobalLoop* loop = new node::NodeGlobalLoop();
	if (node::NodeGlobalWhile* _while = parse_global_while()) {
		loop->var = _while;
		return loop;
	}

	if (node::NodeGlobalFor* _for = parse_global_for()) {
		loop->var = _for;
		return loop;
	}

	return nullptr;
}

node::NodeGlobalWhile* Parser::parse_global_while()
{
	node::NodeGlobalWhile* _while = new node::NodeGlobalWhile();
	if (peek() && peek()->type == WHILE) {
		consume(); // while

		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_while->condition = expr;

			try_consume(COLON, "Expected ':' after boolean expr in global while");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global while");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global while");

			while (node::NodeNestedStmt* stmt = parse_nested_stmt()) {
				_while->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

			return _while;
		}
		else {
			std::cerr << "Expected boolean expression for global while loop" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	return nullptr;
}

node::NodeGlobalFor* Parser::parse_global_for()
{
	node::NodeGlobalFor* _for = new node::NodeGlobalFor();
	if (peek() && peek()->type == FOR &&
		peek(1) && peek(1)->type == NUMBER &&
		peek(2) && peek(2)->type == IDENTIFIER &&
		peek(3) && peek(3)->type == EQUAL
		) {

		consume(); // for
		consume(); // number
		_for->indexValIdentifier = consume(); // identifier
		consume(); // equal
		size_t verify_arithmetic = 0;
		if (verify_arithmetic_expr(&verify_arithmetic)) {
			node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
			_for->indexValExpr = expr;
		}
		else {
			std::cerr << "Expected arithmetic expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COMMA, "Expected ',' after arithmetic expression in global for loop");
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_for->condition = expr;
		}
		else {
			std::cerr << "Expected boolean expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COMMA, "Expected ',' after arithmetic expression in global for loop");
		size_t verify_arithmetic2 = m_currentIndex;
		if (verify_arithmetic_expr(&verify_arithmetic2)) {
			node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
			_for->increment = expr;
		}
		else {
			std::cerr << "Expected boolean expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COLON, "Expected ':' after boolean expr in global while");
		try_consume(NEW_LINE, "Expected 'new_line' after ':' in global while");
		try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global while");

		while (node::NodeNestedStmt* stmt = parse_nested_stmt()) {
			_for->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

		return _for;
	}

	return nullptr;
}

node::NodeFunctionControlFlow* Parser::parse_function_control_flow()
{
	node::NodeFunctionControlFlow* controlFlow = new node::NodeFunctionControlFlow();
	if (node::NodeFunctionIf* _if = parse_function_if()) {
		controlFlow->var = _if;
		return controlFlow;
	}

	if (node::NodeFunctionLoop* _loop = parse_function_loop()) {
		controlFlow->var = _loop;
		return controlFlow;
	}

	return nullptr;
}

node::NodeFunctionIf* Parser::parse_function_if()
{
	if (peek() && peek()->type == IF) {
		consume(); // if
		node::NodeFunctionIf* _if = new node::NodeFunctionIf();
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_if->condition = expr;

			try_consume(COLON, "Expected ':' after boolean expr in global if");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

			while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
				_if->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

			while (node::NodeFunctionElseIf* _elseif = parse_function_else_if())
			{
				_if->elseifs.push_back(_elseif);
			}

			if (node::NodeFunctionElse* _else = parse_function_else()) {
				_if->_else = _else;
			}

			return _if;
		}
		else
		{
			std::cerr << "Expected boolean expression after 'if'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return nullptr;
}

node::NodeFunctionElseIf* Parser::parse_function_else_if()
{
	if (peek() && peek()->type == ELSE &&
		peek(1) && peek(1)->type == IF) {
		consume(); // else
		consume(); // if
		node::NodeFunctionElseIf* _elseif = new node::NodeFunctionElseIf();
		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_elseif->condition = expr;

			try_consume(COLON, "Expected ':' after boolean expr in global if");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

			while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
				_elseif->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after else if statement");
			return _elseif;
		}
		else
		{
			std::cerr << "Expected boolean expression after 'if'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return nullptr;
}

node::NodeFunctionElse* Parser::parse_function_else()
{
	if (peek() && peek()->type == ELSE) {
		consume(); // else
		node::NodeFunctionElse* _else = new node::NodeFunctionElse();

		try_consume(COLON, "Expected ':' after boolean expr in global if");
		try_consume(NEW_LINE, "Expected 'new_line' after ':' in global if");
		try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global if");

		while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
			_else->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

		return _else;


	}
	return nullptr;
}

node::NodeFunctionLoop* Parser::parse_function_loop()
{

	node::NodeFunctionLoop* loop = new node::NodeFunctionLoop();
	if (node::NodeFunctionWhile* _while = parse_function_while()) {
		loop->var = _while;
		return loop;
	}

	if (node::NodeFunctionFor* _for = parse_function_for()) {
		loop->var = _for;
		return loop;
	}

	return nullptr;
}

node::NodeFunctionWhile* Parser::parse_function_while()
{
	node::NodeFunctionWhile* _while = new node::NodeFunctionWhile();
	if (peek() && peek()->type == WHILE) {
		consume(); // while

		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_while->condition = expr;

			try_consume(COLON, "Expected ':' after boolean expr in global while");
			try_consume(NEW_LINE, "Expected 'new_line' after ':' in global while");
			try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global while");

			while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
				_while->stmts.push_back(stmt);
			}

			try_consume(TAB_DEDENT, "Expected tab dedent after if statement");
			return _while;

		}
		else {
			std::cerr << "Expected boolean expression for global while loop" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	return nullptr;
}

node::NodeFunctionFor* Parser::parse_function_for()
{
	node::NodeFunctionFor* _for = new node::NodeFunctionFor();
	if (peek() && peek()->type == FOR &&
		peek(1) && peek(1)->type == NUMBER &&
		peek(2) && peek(2)->type == IDENTIFIER &&
		peek(3) && peek(3)->type == EQUAL
		) {

		consume(); // for
		consume(); // number
		_for->indexValIdentifier = consume(); // identifier
		consume(); // equal
		size_t verify_arithmetic = 0;
		if (verify_arithmetic_expr(&verify_arithmetic)) {
			node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
			_for->indexValExpr = expr;
		}
		else {
			std::cerr << "Expected arithmetic expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COMMA, "Expected ',' after arithmetic expression in global for loop");

		size_t verify_bool = 0;
		if (verify_boolean_expr(&verify_bool)) {
			node::NodeBooleanExpr* expr = parse_boolean_expr();
			_for->condition = expr;
		}
		else {
			std::cerr << "Expected boolean expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COMMA, "Expected ',' after arithmetic expression in global for loop");
		size_t verify_arithmetic2 = m_currentIndex;

		if (verify_arithmetic_expr(&verify_arithmetic2)) {
			node::NodeArithmeticExpr* expr = parse_arithmetic_expr();
			_for->increment = expr;
		}
		else {
			std::cerr << "Expected boolean expression for global for loop" << std::endl;
			exit(EXIT_FAILURE);
		}

		try_consume(COLON, "Expected ':' after boolean expr in global while");
		try_consume(NEW_LINE, "Expected 'new_line' after ':' in global while");
		try_consume(TAB_INDENT, "Expected 'tab indent' after 'new line' in global while");

		while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
			_for->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after if statement");

		return _for;
	}
	return nullptr;
}

node::NodeDefinition* Parser::parse_definition()
{
	node::NodeDefinition* definition = new node::NodeDefinition();

	if (node::NodeFunctionDefinition* func_def = parse_function_definition()) {
		definition->var = func_def;
		return definition;
	}

	if (node::NodeObjectDefinition* obj_def = parse_object_definition()) {
		definition->var = obj_def;
		return definition;
	}

	return nullptr;
}

node::NodeFunctionDefinition* Parser::parse_function_definition()
{
	node::NodeFunctionDefinition* func_def = new node::NodeFunctionDefinition();

	//Assume type function first
	if (Token* type = parse_type()) {
		func_def->type = type;
		if (
			peek() && peek()->type == OPEN_SQUAREBRACKET &&
			peek(1) && peek(1)->type == CLOSED_SQUAREBRACKET
			) 
		{
			consume(); // [
			consume(); // ]
			func_def->isAnArray = true;
		}

		try_consume(FUNCTION, "Expects function keyword");
		
		if (Token* func_name = try_consume(IDENTIFIER, "Expects identifier after function")) {
			func_def->functionName = *func_name;
		}
		try_consume(OPEN_PARANTHESIS, "Expected '(' after function name");

		do
		{
			if (Token* type = parse_type()) {
				node::NodeFunctionArg* arg = new node::NodeFunctionArg();
				arg->type = *type;
				if (
					peek() && peek()->type == OPEN_SQUAREBRACKET &&
					peek(1) && peek(1)->type == CLOSED_SQUAREBRACKET
					)
				{
					consume(); // [
					consume(); // ]
					arg->isTypeAnArray = true;
				}

				arg->identifier = *try_consume(IDENTIFIER, "Expected identfier name for argument");

				func_def->args.push_back(arg);
			}

		} while (try_consume(COMMA));

		try_consume(CLOSED_PARANTHESIS, "Expected ')' after function args");

		try_consume(COLON, "Expected ':' in function definition");
		try_consume(NEW_LINE, "Expected 'new_line'in function definition");
		try_consume(TAB_INDENT, "Expected 'tab indent' in function definition");

		while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
			func_def->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after function definition");
		return func_def;
	}


	// now typeless function
	else if (peek() && peek()->type == FUNCTION) {
		try_consume(FUNCTION, "Expects function keyword");

		if (Token* func_name = try_consume(IDENTIFIER, "Expects identifier after function")) {
			func_def->functionName = *func_name;
		}
		try_consume(OPEN_PARANTHESIS, "Expected '(' after function name");

		do
		{
			if (Token* type = parse_type()) {
				node::NodeFunctionArg* arg = new node::NodeFunctionArg();
				arg->type = *type;
				if (
					peek() && peek()->type == OPEN_SQUAREBRACKET &&
					peek(1) && peek(1)->type == CLOSED_SQUAREBRACKET
					)
				{
					consume(); // [
					consume(); // ]
					arg->isTypeAnArray = true;
				}

				arg->identifier = *try_consume(IDENTIFIER, "Expected identfier name for argument");

				func_def->args.push_back(arg);
			}

		} while (try_consume(COMMA));

		try_consume(CLOSED_PARANTHESIS, "Expected ')' after function args");

		try_consume(COLON, "Expected ':' in function definition");
		try_consume(NEW_LINE, "Expected 'new_line'in function definition");
		try_consume(TAB_INDENT, "Expected 'tab indent' in function definition");

		while (node::NodeFunctionStmt* stmt = parse_function_stmt()) {
			func_def->stmts.push_back(stmt);
		}

		try_consume(TAB_DEDENT, "Expected tab dedent after function definition");

		return func_def;
	}

	return nullptr;
}

node::NodeObjectDefinition* Parser::parse_object_definition()
{
	node::NodeObjectDefinition* obj_Def = new node::NodeObjectDefinition();
	if (peek() && peek()->type == IDENTIFIER) {
		obj_Def->identifier = consume(); // identifier

		try_consume(COLON, "expected colon in object definition");
		try_consume(NEW_LINE, "expected new_line in object definition");
		try_consume(TAB_INDENT, "expected tab indent in object definition");

		while (node::NodeDecl* decl = parse_decleration())
		{
			obj_Def->props.push_back(decl);
		}
		try_consume(TAB_DEDENT, "expected tab dedent in object definition");

		return obj_Def;
	}

	return nullptr;
}

Token* Parser::parse_type()
{
	if (
		(peek() && peek()->type == NUMBER ||
		peek() && peek()->type == STRING ||
		peek() && peek()->type == BOOLEAN ||
		peek() && peek()->type == IDENTIFIER) &&
		peek(1) && peek(1)->type != COLON
		) 
	{

		return &consume();
	}
	return nullptr;
}

bool Parser::verify_arithmetic_expr(size_t* index)
{
	if (verify_term(index)) {


		if (peek(*index)->type == PLUS) {
			(*index)++;

			return verify_term(index);
		}

		if (peek(*index)->type == MINUS) {
			(*index)++;

			return verify_term(index);
		}
		return true;
	}

	return false;
}

bool Parser::verify_term(size_t* index)
{
	if (verify_factor(index)) {


		if (peek(*index)->type == MULTIPLY) {
			(*index)++;
			return verify_factor(index);
		}

		if (peek(*index)->type == DIVIDE) {
			(*index)++;
			return verify_factor(index);
		}

		if (peek(*index)->type == MODULO) {
			(*index)++;
			return verify_factor(index);
		}
		return true;
	}
	return false;
}

bool Parser::verify_factor(size_t* index)
{
	if (peek(*index)->type == DECIMAL) {
		(*index)++;
		return true;
	}

	if (peek(*index)->type == IDENTIFIER) {
		(*index)++;
		return true;
	}

	// (<AExpr>)
	if (peek(*index)->type == OPEN_PARANTHESIS) {
		(*index)++;
		if (verify_arithmetic_expr(index)) {
			if (peek(*index)->type == CLOSED_PARANTHESIS) {
				return true;
			}
		}
	}

	return verify_function_call(index);


	return false;
}

bool Parser::verify_function_call(size_t* index)
{
	if (peek(*index) && peek(*index)->type == IDENTIFIER &&
		peek(*index + 1) && peek(*index + 1)->type == OPEN_PARANTHESIS) {

		index += 2;

		do
		{
			if (peek(*index) && peek(*index)->type == COMMA)
				(*index)++;

			if (verify_value(index) == false) {
				return false;
			}

		} while (peek(*index) && peek(*index)->type == COMMA);

		if (peek(*index) && peek(*index)->type == CLOSED_PARANTHESIS) {
			(*index)++;
			return true;
		}
	}

	return false;
}

bool Parser::verify_value(size_t* index)
{
	size_t start_val = *index;
	// Assume normal function call
	if (verify_function_call(index)) {

		// Verify if properties are there after function call (also assume function call returns an object)
		if (peek(*index) && peek(*index)->type == DOT) {
			(*index)++; // .

			if (peek(*index) && peek(*index)->type == IDENTIFIER) {
				(*index)++;

				while (peek(*index) && peek(*index)->type == DOT)
				{
					(*index)++;
					if (peek(*index) && peek(*index)->type == IDENTIFIER)
						(*index)++;
				}

				//Check if it is an array
				if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
					(*index)++;
					if (verify_arithmetic_expr(index)) {
						if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
							(*index)++;
							return true;
						}
					}
					else
						return false;
				}
				return true;

			}


		}

		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
					return true;
				}
			}
			else
				return false;
		}
		return true;
	}
	else
		*index = start_val;

	// value returns boolean expression
	if (verify_boolean_expr(index)) {

		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
					return true;
				}
			}
			else return false;
		}
		else
			return true;
	}
	else
		*index = start_val;

	// value returns arithmetic expression
	if (verify_arithmetic_expr(index)) {

		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
					return true;
				}
			}
			return false;
		}
		else
			return true;
	}
	else
		*index = start_val;

	// value returns string expression
	if (verify_string_expr(index)) {
		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
					return true;
				}
			}
			else
				return false;
		}
		else
			return true;
	}
	else
		*index = start_val;

	

	// value returns identifier (can be of any type)
	if (peek(*index) && peek(*index)->type == IDENTIFIER) {
		(*index)++;

		// Verify if properties are there after identifier (also assume identifier is an object)
		if (peek(*index) && peek(*index)->type == DOT) {
			(*index)++; // .

			if (peek(*index) && peek(*index)->type == IDENTIFIER) {
				(*index)++;

				while (peek(*index) && peek(*index)->type == DOT)
				{
					(*index)++;
					if (peek(*index) && peek(*index)->type == IDENTIFIER)
						(*index)++;
				}
			}
			

			//Check if it is an array
			if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
				(*index)++;
				if (verify_arithmetic_expr(index)) {
					if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
						(*index)++;
					}
				}
				else
					return false;
			}
			else
				return true;
		}

		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
					return true;
				}
			}
			else
				return false;
		}
		else
			return true;

	}


	return false;
}

bool Parser::verify_string_expr(size_t* index)
{

	if (peek(*index) && peek(*index)->type == STRING_VAL) {
		(*index)++;

		if (peek(*index)->type == PLUS) {
			(*index)++;

			return verify_string_expr(index);
		}

		return true;
	}
	else if (peek(*index) && peek(*index)->type == IDENTIFIER) {
		(*index)++;

		if (peek(*index)->type == PLUS) {
			(*index)++;

			return verify_string_expr(index);
		}

		return true;
	}
	else if (verify_function_call(index)) {

		if (peek(*index)->type == PLUS) {
			(*index)++;

			return verify_string_expr(index);
		}

		return true;
	}

	return false;
}

bool Parser::verify_boolean_expr(size_t* index)
{
	return verify_or(index);
}

bool Parser::verify_or(size_t* index)
{

	if (verify_and(index)) {
		// check for operator
		if (peek(*index)->type == OR) {
			return verify_or_op(index);
		}
		return true;
	}
	return false;
}

bool Parser::verify_or_op(size_t* index)
{
	if (peek(*index) && peek(*index)->type == OR) {
		(*index)++;
		return verify_and(index);
	}
	return false;
}

bool Parser::verify_and(size_t* index)
{

	if (verify_equal(index)) {
		// Check for operator
		if (peek(*index) && peek(*index)->type == AND) {
			return verify_and_op(index);
		}

		return true;
	}
	return false;
}

bool Parser::verify_and_op(size_t* index)
{
	Token* _and = try_consume(AND);

	if (peek(*index) && peek(*index)->type == AND) {
		(*index)++;
		return verify_equal(index);

	}
	return false;
}

bool Parser::verify_equal(size_t* index)
{

	if (verify_real_expr(index)) {
		// check for operators

		// is not
		if (peek(*index)->type == IS && peek(*index + 1)->type == NOT) {
			return verify_equal_is_not(index);
		}

		// is not
		if (peek()->type == IS) {
			return verify_equal_is(index);
		}

		return true;

	}

	return false;
}

bool Parser::verify_equal_is_not(size_t* index)
{

	if (peek(*index) && peek(*index)->type == IS &&
		peek(*index + 1) && peek(*index + 1)->type == NOT) {
		(*index) += 2;
		return verify_real_expr(index);
	}
	return false;
}

bool Parser::verify_equal_is(size_t* index)
{
	if (peek(*index) && peek(*index)->type == IS) {
		(*index)++;
		return verify_real_expr(index);
	}
	return false;
}

bool Parser::verify_real_expr(size_t* index)
{

	// Assume arithmetic expression
	if (verify_arithmetic_expr(index)) {
		// greater equal
		if (peek(*index)->type == GREATER && peek(*index + 1)->type == EQUAL) {
			return verify_greater_equal(index);
		}

		// less equal
		if (peek(*index)->type == LESS && peek(*index + 1)->type == EQUAL) {
			return verify_less_equal(index);
		}

		// greater
		if (peek(*index)->type == GREATER) {
			return verify_greater(index);
		}

		// less
		if (peek(*index)->type == LESS) {
			return verify_less(index);
		}
		return false;
	}

	return verify_not(index);
}

bool Parser::verify_greater_equal(size_t* index)
{

	if (peek(*index) && peek(*index)->type == GREATER &&
		peek(*index + 1) && peek(*index + 1)->type == EQUAL) {
		(*index) += 2;
		return verify_arithmetic_expr(index);

	}
	return false;
}

bool Parser::verify_less_equal(size_t* index)
{
	if (peek(*index) && peek(*index)->type == LESS &&
		peek(*index + 1) && peek(*index + 1)->type == EQUAL) {
		(*index) += 2;
		return verify_arithmetic_expr(index);

	}
	return false;
}

bool Parser::verify_greater(size_t* index)
{
	if (peek(*index) && peek(*index)->type == GREATER) {
		(*index)++;
		return verify_arithmetic_expr(index);

	}
	return false;
}

bool Parser::verify_less(size_t* index)
{
	if (peek(*index) && peek(*index)->type == LESS) {
		(*index)++;
		return verify_arithmetic_expr(index);

	}
	return false;
}

bool Parser::verify_not(size_t* index)
{
	if (peek(*index)->type == NOT) {
		return verify_not_op(index);
	}

	// no not tokens now expects factor
	return verify_boolean_factor(index);
}

bool Parser::verify_not_op(size_t* index)
{
	if (peek(*index) && peek(*index)->type == NOT) {
		(*index)++;
		return verify_not(index);
	}
	return false;
}

bool Parser::verify_boolean_factor(size_t* index)
{
	// bool value
	if (peek(*index) && peek(*index)->type == BOOLVAL) {
		(*index)++;
		return true;
	}


	// Identifier
	if (peek(*index) && peek(*index)->type == IDENTIFIER) {
		(*index)++;
		return true;
	}

	// (<BExpr>)
	if (peek(*index) && peek(*index)->type == OPEN_PARANTHESIS) {
		(*index)++;
		if (verify_boolean_expr(index)) {
			if (peek(*index) && peek(*index)->type == CLOSED_PARANTHESIS)
				return true;
		}
	}

	if (verify_function_call(index)) {
		return true;
	}

	return false;
}

bool Parser::operator_check(size_t* index)
{
	node::NodeValueIdentifier* val_ident = new node::NodeValueIdentifier();
	if (peek(*index) && peek(*index)->type == IDENTIFIER) {
		(*index)++;
	}

	// Verify if properties are there after identifier (also assume identifier is an object)
	if (peek(*index) && peek(*index)->type == DOT) {
		(*index)++;
		if (peek(*index) && peek(*index)->type == IDENTIFIER) {
			(*index)++;
		}
		while (peek(*index) && peek(*index)->type == DOT)
		{
			(*index)++;
			if (peek(*index) && peek(*index)->type == IDENTIFIER) {
				(*index)++;
			}
		}

		//Check if it is an array
		if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
			(*index)++;
			if (verify_arithmetic_expr(index)) {
				if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
					(*index)++;
				}
			}
		}
		switch (peek(*index)->type)
		{
		case PLUS:
		case MINUS:
		case MULTIPLY:
		case DIVIDE:
		case MODULO:
		case LESS:
		case GREATER:
		case IS:
		case NOT:
			return true;
		default:
			return false;
			break;
		}
	}

	//Check if it is an array
	if (peek(*index) && peek(*index)->type == OPEN_SQUAREBRACKET) {
		(*index)++;
		if (verify_arithmetic_expr(index)) {
			if (peek(*index) && peek(*index)->type == CLOSED_SQUAREBRACKET) {
				(*index)++;
			}
		}
	}

	switch (peek(*index)->type)
	{
	case PLUS:
	case MINUS:
	case MULTIPLY:
	case DIVIDE:
	case MODULO:
	case LESS:
	case GREATER:
	case IS:
	case NOT:
		return true;
	default:
		return false;
		break;
	}
}
