#pragma once
#include <variant>
#include "../lexer/tokenvalues.h"

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
		std::variant<NodeExprAdd*> var;
	};

	struct NodeTerm {
		std::variant<NodeTermIntLit*, NodeTermIdentifier*> var;
	};

	struct NodeExpr
	{
		std::variant<NodeTerm*, NodeArithmeticExpr*> var;
	};

	struct NodeStmtNumber
	{
		Token ident;
		NodeExpr* expr{};
	};

	struct NodeStmtPrint {
		NodeExpr* expr;
	};

	struct NodeStmt
	{
		std::variant<NodeStmtNumber*, NodeStmtPrint*> var;
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
        Token* t = try_consume(TokenType::DECIMAL);
        if (t != nullptr) {
            auto* term_int_lit = m_allocator.alloc<node::NodeTermIntLit>();
            term_int_lit->int_lit = t->value;

            auto term = m_allocator.alloc<node::NodeTerm>();
            term->var = term_int_lit;
            return term;
        }

        // Identifier
        t = try_consume(TokenType::IDENTIFIER);
        if (t != nullptr) {
            auto* term_identifier = m_allocator.alloc<node::NodeTermIdentifier>();
            term_identifier->identifier = t->value;

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
                prec = op_prec(current_token->type);

                if (prec < min_prec) {
                    break;
                }
            }
            else {
                break;
            }

			Token op = consume(); //Get the operator
			int  next_min_prec = prec.value() + 1;

			//Veryfiying Aritmetic expr
			node::NodeExpr* expr_rhs = parse_expr(next_min_prec);
            if (expr_rhs == nullptr) {
				std::cerr << "Expected expression after operator" << std::endl;
				exit(EXIT_FAILURE);
            }

        }

        return expr;
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


    Token* peek(int offset = 0) const {
        if (m_currentIndex + offset >= m_tokens.size())
            return nullptr;
        return &m_tokens.at(m_currentIndex + offset);
    }

    Token consume() {
        return m_tokens.at(m_currentIndex++);
    }

    Token* try_consume(TokenType type, const std::string& err_msg = "") {
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
    Allocator m_allocator;
};
