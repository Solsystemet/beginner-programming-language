#include "src/lexer/tokens.h"
#include "src/mpark/variant.hpp"
#include "src/parser/parser.hpp"


class ParserHelperFunctions {  
public:  
   nodeExpr* try_comsume_arithmetic(TOKEN token, nodeExpr* expr) {  
       string name = token.tolower();  
       if (peek()->type == PLUS) {  
           consume();  
           auto* expr_lhs = new node::NodeArithmeticExpr();  
           auto* name = new nodeExpr();  
           expr_lhs->var = expr->var;  
           add->lhs = expr_lhs;  

           auto* term_rhs = parse_term();  
           if (term_rhs == nullptr) {  
               std::cerr << "Expected term after operator" << std::endl;  
               exit(EXIT_FAILURE);  
           }  

           name->rhs = term_rhs;  
           expr->var = name;  
       }  
       return expr;  
   }  
};