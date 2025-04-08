#pragma once
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include <vector>
#include "../mpark/variant.hpp"

namespace node {

	struct NodeTermIntLit
	{
		Token int_lit;
	};

	struct NodeTermIdentifier
	{
		Token identifier;
	};

	// Forward cast probably bad
	struct NodeExpr;
	struct  NodeExprAdd {
		NodeExpr* lhs;
		NodeExpr* rhs;
	};

	struct NodeStringExpr;
	struct NodeStringExprConcat {
		NodeStringExpr* lhs;
		NodeStringExpr* rhs;
	};

	struct NodeStringExpr{
		mpark::variant<Token, NodeStringExprConcat*> var;
	};

	struct NodeArithmeticExpr
	{
		mpark::variant<NodeExprAdd*> var;
	};

    //Identifier or Decimal can be passed to this struct
	struct NodeTerm{
		mpark::variant<NodeTermIntLit*, NodeTermIdentifier*> var;
	};
    
    //NodeTerm or note artihmetic expression can be passed to this struct
	struct NodeExpr
	{
        mpark::variant<NodeArithmeticExpr*, NodeTerm*> var;
	};

	struct NodeSimpleDecl {
		Token type ={};
		Token identifier;
		NodeExpr* expr;

	};

	struct NodeObjectDecl {
		Token identifier;
		std::vector<NodeSimpleDecl*> properties;
	};

	struct NodeDecl {
		mpark::variant<NodeSimpleDecl*, NodeObjectDecl*> var;
	};

	struct NodeStmtNumber
	{
		Token ident;
		NodeExpr* expr = nullptr;
	};

	struct NodeStmtPrint {
		mpark::variant<NodeExpr*, NodeStringExpr*> var;
	};

	struct NodeStmt
	{
		mpark::variant<NodeStmtNumber*, NodeStmtPrint*> var;
	};

	struct NodeProg
	{
		std::vector<NodeStmt*> stmts;
	};
}
class Parser
{
public:
	// Constructor
	Parser(std::vector<Token> tokens)
		: m_tokens(std::move(tokens)) {
	}

    node::NodeTerm* parse_term() {
        // <term> ::= <DECIMAL> | <IDENTIFIER>

        // Decimal
        Token* t = try_consume(DECIMAL);
        if (t != nullptr) {
			auto* term_int_lit = new node::NodeTermIntLit();
            term_int_lit->int_lit = *t;

            auto term = new node::NodeTerm();
            term->var = term_int_lit;
            return term;
        }

        // Identifier
        t = try_consume(IDENTIFIER);
        if (t != nullptr) {
            auto* term_identifier = new node::NodeTermIdentifier();
            term_identifier->identifier = *t;

            auto term = new node::NodeTerm();
            term->var = term_identifier;
            return term;
        }
        return nullptr;
    }

    node::NodeExpr* parse_expr(int min_prec = 0) {
        node::NodeTerm* term = parse_term();

        // Double check if term is empty
        if (term == nullptr) {
            return nullptr;
        }

        auto expr = new node::NodeExpr();
        expr->var = term;

        while (true) {

            Token* current_token = peek();
            int prec = -1;

			//Check for expr -> Aritmetic expr
            if (current_token != nullptr) {
				prec = op_prec(current_token->type);
                if (prec == -1 || prec < min_prec) {
                    break;
                }
            }
            else {
                break;
            }

			Token op = consume(); //Get the operator
			int  next_min_prec = prec+ 1;

			//Veryfiying Aritmetic expr
			node::NodeExpr* expr_rhs = parse_expr(next_min_prec);
            if (expr_rhs == nullptr) {
				std::cerr << "Expected expression after operator" << std::endl;
				exit(EXIT_FAILURE);
            }
			auto arithmetic_expr = new node::NodeArithmeticExpr();
			auto expr_lhs = new node::NodeExpr();

			//Switch case for rule 5
			// <Arithmetic Expr> -> <AddExpr>
			switch (op.type)
			{
				// Rule 6.1
				//<AddExpr> -> <Expr>+<Expr>
				case PLUS:
				auto add = new node::NodeExprAdd();
				expr_lhs->var = expr->var;
				add->lhs = expr_lhs;
				add->rhs = expr_rhs;
				arithmetic_expr->var = add;
			break;
			}
			expr->var = arithmetic_expr;
		}

        return expr;
    }

    node::NodeStringExpr* parse_string_expr(int min_prec = 0) {
		Token* t = try_consume(STRING_VAL);
		if (t == nullptr){
			return nullptr;
		}
		
	
		auto* string_expr = new node::NodeStringExpr();
		string_expr->var = *t;
	
		while (true) {
			Token* current_token = peek();
			int prec = -1;
	
			if (current_token != nullptr) {
				prec = op_prec(current_token->type);
				if (prec == -1 || prec < min_prec){
					break;
				} 
			} else{
				break;
			} 
	
			Token op = consume();
			int next_min_prec = prec + 1;
	
			node::NodeStringExpr* rhs = parse_string_expr(next_min_prec);
			if (rhs == nullptr) {
				std::cerr << "Expected string expression after '+'" << std::endl;
				exit(EXIT_FAILURE);
			}
	
			auto* concat = new node::NodeStringExprConcat();
			concat->lhs = string_expr;
			concat->rhs = rhs;
	
			string_expr = new node::NodeStringExpr();
			string_expr->var = concat;
		}
	
		return string_expr;
	}
	

    node::NodeStmt* parse_stmt() {
		//Rule 3.1
		// Stmt -> number identifier = <Expr>
		if (m_tokens.size() - m_currentIndex > 3 && peek()->type == NUMBER &&
			peek(1) && peek(1)->type == IDENTIFIER &&
			peek(2) && peek(2)->type == EQUAL && 
			peek(3)) 
		{
			// consume terminal symbols
			
			auto* node_stmt_number = new node::NodeStmtNumber();
			node:node_stmt_number->ident = consume();
			consume();
			consume();
			
			// Parse expression rule
			if (node::NodeExpr* expr = parse_expr()) {
				node_stmt_number->expr = expr;
			}
			else {
				std::cerr << "Invalid expression" << std::endl;
				exit(EXIT_FAILURE);
					
			}
			// consume terminal symbols
			try_consume(NEW_LINE, "Expected 'new_line'");
			auto* node_stmt = new node::NodeStmt();
			node_stmt->var = node_stmt_number;
			return node_stmt;
		}
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
			else if (const auto node_epxr = parse_expr()) {
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
		if (m_tokens.size() - m_currentIndex > 3 &&
				(peek()->type == NUMBER || peek()->type == STRING || peek()->type == BOOLEAN) &&
				peek(1) && peek(1)->type == IDENTIFIER &&
				peek(2) && peek(2)->type == EQUAL
			)

 			{
			auto* simple_decl = new node::NodeSimpleDecl();
			simple_decl->type = consume();      // <type>
			simple_decl->identifier = consume();// <identifier>
			consume();                    
	
			auto* expr = parse_expr();
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
};
