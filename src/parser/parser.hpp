#pragma once
#include "../lexer/tokenvalues.h"
#include "../lexer/tokens.h"
#include <vector>
#include "../arena.hpp"
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

	struct NodeStmtNumber
	{
		Token ident;
		NodeExpr* expr = nullptr;
	};

	struct NodeStmtPrint {
		NodeExpr* expr;
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
    Parser(std::vector<Token>& tokens)
        : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 8), m_currentIndex(0) {
    }

    node::NodeTerm* parse_term() {
        // <term> ::= <DECIMAL> | <IDENTIFIER>

        // Decimal
        Token* t = try_consume(DECIMAL);
        if (t != nullptr) {
            auto* term_int_lit = m_allocator.alloc<node::NodeTermIntLit>();
            term_int_lit->int_lit = *t;

            auto term = m_allocator.alloc<node::NodeTerm>();
            term->var = term_int_lit;
            return term;
        }

        // Identifier
        t = try_consume(IDENTIFIER);
        if (t != nullptr) {
            auto* term_identifier = m_allocator.alloc<node::NodeTermIdentifier>();
            term_identifier->identifier = *t;

            auto term = m_allocator.alloc<node::NodeTerm>();
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

        auto expr = m_allocator.alloc<node::NodeExpr>();
        expr->var = term;

        while (true) {

            Token* current_token = peek();
            int prec;

			//Check for expr -> Aritmetic expr
            if (current_token != nullptr) {
                prec = *op_prec(current_token->type);

                if (prec < min_prec) {
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

        }

        return expr;
    }

    node::NodeProg parse_prog() {
		node::NodeProg prog;
		// Rule 2
		// Stmts -> <Stmt><Stmts>
		while (peek())
		{	
			//Parse a statement
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

    node::NodeStmt* parse_stmt() {
		//Rule 3.1
		// Stmt -> number identifier = <Expr>
		if (peek() && peek()->type == NUMBER &&
			peek(1) && peek(1)->type == IDENTIFIER &&
			peek(2) && peek(2)->type == EQUAL) 
		{
			// consume terminal symbols
			consume();
			auto* node_stmt_number = m_allocator.alloc<node::NodeStmtNumber>();
			node:node_stmt_number->ident = consume();
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
			auto* node_stmt = m_allocator.alloc<node::NodeStmt>();
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
			auto* node_stmt_print = m_allocator.alloc<node::NodeStmtPrint>();

			// Parse expression rule
			if (const auto node_epxr = parse_expr()) {
				node_stmt_print->expr = node_epxr;
			}
			else {
				std::cerr << "Invalid expression" << std::endl;
				exit(EXIT_FAILURE);
			}

			// consume terminal symbols
			try_consume(CLOSED_PARANTHESIS, "Exprected ')'");
			try_consume(EOF, "Expected 'end_of_file'");

			auto* node_stmt = m_allocator.alloc<node::NodeStmt>();
			node_stmt->var = node_stmt_print;
			return node_stmt;
		}

		return {};

	}

private:

    //Arithmetic Prec
	int* op_prec(int t) {
        switch (t)
        {
        case PLUS:
            return 0;
        default:
            return nullptr;
        };
	}


    Token* peek(int offset = 0){
        if (m_currentIndex + offset >= m_tokens.size())
            return nullptr;
        return &m_tokens.at(m_currentIndex + offset);
    }

    Token consume() {
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
    ArenaAllocater m_allocator;
};
