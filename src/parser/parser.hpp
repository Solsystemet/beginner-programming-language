#pragma once
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include <vector>
#include "../mpark/variant.hpp"
#include "./nodes.hpp"



class Parser
{
public:
	// Constructor
	Parser(std::vector<Token> tokens)
		: m_tokens(std::move(tokens)) {
	}

	node::NodeFactor* parse_factor() {
		Token* t;

		// Decimal
		try_consume_symbol(t, new node::NodeFactorDecimal());

		// Identifier
		try_consume_symbol(t, new node::NodeFactorIdentifier());
		
		// (<AExpr>)
		try_consume_symbol(t, new node::NodeArithmeticExpr());
	
	}

    node::NodeTerm* parse_term() {
        // <term> ::= <DECIMAL> | <IDENTIFIER
		auto* term = new node::NodeTerm();
		node::NodeFactor* factor = parse_factor();

		if (factor == nullptr) {
			return nullptr;
		}
		term->var = factor;


		try_consume_arithmetic(MULTIPLY, term, new node::NodeExprMult());

		try_consume_arithmetic(DIVIDE, term, new node::NodeExprDivide());

		return term;
    }

	node::NodeArithmeticExpr* parse_arithmetic_expr() {
		node::NodeTerm* term = parse_term();

		if (term == nullptr) {
			return nullptr;
		}

		auto expr = new node::NodeArithmeticExpr();
		expr->var = term;

		
		try_consume_arithmetic(PLUS, expr,new node::NodeExprAdd());

		try_consume_arithmetic(MINUS, expr, new node::NodeExprSubtract());

		return expr;
	}

    node::NodeStringExpr* parse_string_expr() {
		Token* t = try_consume(STRING_VAL);
		if (t == nullptr){
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
	

    node::NodeStmt* parse_stmt() {
		//Rule 3.2
		// Stmt -> print(<Expr>)
		if (peek() && peek()->type == PRINT &&
			peek(1) && peek(1)->type == OPEN_PARANTHESIS) {
			// consume terminal symbols
			consume();
			consume();
			auto* node_stmt_print = new node::NodeStmtPrint();
			if(const auto string_expr = parse_string_expr()){
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

	node::NodeDecl* parse_declaration() {
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
					} else {
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

	node::NodeProg parse_prog() {
		node::NodeProg prog;
		// Rule 2
		// Stmts -> <Stmt><Stmts>
		while (peek())
		{	
			if(node::NodeDecl* decl = parse_declaration()){
				node::NodeStmt* stmt = new node::NodeStmt();
				stmt->var = decl;


				prog.stmts.push_back(stmt);
				std::cout<< "Parsed declaration\n";
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

private:

    //Arithmetic Prec
	int op_prec(int t) {
        switch (t)
        {
        case PLUS:
            return 0;
        default:
            return -1;
        };
	}


    Token* peek(int offset = 0){
        if (m_currentIndex + offset >= m_tokens.size())
            return nullptr;
        return &m_tokens.at(m_currentIndex + offset);
    }

    Token& consume() {
        return m_tokens.at(m_currentIndex++);
    }

    Token* try_consume(int type, const std::string& err_msg = "") {
        if (peek() != nullptr && peek()->type == type) {
            return &consume();
        }
        if (!err_msg.empty()) {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
        return nullptr;
    }

    std::vector<Token> m_tokens;
    size_t m_currentIndex;


	// Overload for DECIMAL
	node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorDecimal* factor_decimal) {
		t = try_consume(DECIMAL);
		if (t != nullptr) {
			factor_decimal->decimal = *t;

			auto* factor = new node::NodeFactor();
			factor->var = factor_decimal;
			return factor;
		}
		return nullptr;
	}

	// Overload for IDENTIFIER
	node::NodeFactor* try_consume_symbol(Token*& t, node::NodeFactorIdentifier* factor_identifier) {
		t = try_consume(IDENTIFIER);
		if (t != nullptr) {
			factor_identifier->identifier = *t;

			auto* factor = new node::NodeFactor();
			factor->var = factor_identifier;
			return factor;
		}
		return nullptr;
	}

	// Overload for Parentheses (OPEN_PARANTHESIS)
	node::NodeFactor* try_consume_symbol(Token*& t, node::NodeArithmeticExpr* arithmetic_expr) {
		t = try_consume(OPEN_PARANTHESIS);
		if (t != nullptr) {
			arithmetic_expr = parse_arithmetic_expr();
			try_consume(CLOSED_PARANTHESIS, "Expected ')'");
			auto factor = new node::NodeFactor();
			factor->var = arithmetic_expr;
			return factor;
		}
		return nullptr;
	}




	void try_consume_arithmetic(
		int tokenType,
		node::NodeArithmeticExpr* expr,
		node::NodeExprAdd* add
	) {
		if (peek()->type == tokenType) {
			consume();
			auto expr_lhs = new node::NodeArithmeticExpr();
			expr_lhs->var = expr->var;
			add->lhs = expr_lhs;

			auto* term_rhs = parse_term();
			if (term_rhs == nullptr) {
				std::cerr << "Expected term after operator" << std::endl;
				exit(EXIT_FAILURE);
			}

			add->rhs = term_rhs;
			expr->var = add;
		}
	}

	void try_consume_arithmetic(
		int tokenType,
		node::NodeArithmeticExpr* expr,
		node::NodeExprSubtract* sub
	) {
		if (peek()->type == tokenType) {
			consume();
			auto expr_lhs = new node::NodeArithmeticExpr();
			expr_lhs->var = expr->var;
			sub->lhs = expr_lhs;

			auto* term_rhs = parse_term();
			if (term_rhs == nullptr) {
				std::cerr << "Expected term after operator" << std::endl;
				exit(EXIT_FAILURE);
			}

			sub->rhs = term_rhs;
			expr->var = sub;
		}
	}

	void try_consume_arithmetic(
		int tokenType,
		node::NodeTerm* term,
		node::NodeExprMult* mult
	) {
		if (peek()->type == tokenType) {
			consume();

			auto* term_mult = new node::NodeExprMult();
			term_mult->lhs = term;
			node::NodeFactor* factor_rhs = parse_factor();
			if (factor_rhs == nullptr) {
				std::cerr << "Invalid factor!" << std::endl;
				exit(EXIT_FAILURE);
			}
			term_mult->rhs = factor_rhs;
			term->var = term_mult;
		}
	}

	void try_consume_arithmetic(
		int tokenType,
		node::NodeTerm* term,
		node::NodeExprDivide* div
	) {
		if (peek()->type == DIVIDE) {
			consume();

			auto* term_divide = new node::NodeExprDivide();
			term_divide->lhs = term;
			node::NodeFactor* factor_rhs = parse_factor();
			if (factor_rhs == nullptr) {
				std::cerr << "Invalid factor!" << std::endl;
				exit(EXIT_FAILURE);
			}
			term_divide->rhs = factor_rhs;
			term->var = term_divide;
		}
	}



	
};
