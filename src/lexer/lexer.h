#pragma once
#include <stdio.h>
#include <iostream>
#include "./tokens.h"
#include <string.h>
#include <vector>
#include "tokenvalues.h"
#include "flex.flex.h"


class Lexer
{
private:
    // Sanitizes the tokens to have correct amount of indents and dedents
    inline void FixTabIndent(std::vector<Token>* tokens);

    // helper function for verifying indent and dedent count. move elsewhere or delete
    void CountIndentDedent(std::vector<Token> tokens);

public:
    inline Lexer(FILE* file);
    inline std::vector<Token> Tokenize();
    inline ~Lexer();
};

Lexer::Lexer(FILE* file)
{
    yyin = file;
}

Lexer::~Lexer()
{
}

std::vector<Token> Lexer::Tokenize()
{
    int ntoken;
    std::vector<Token> result;
    while ((ntoken = yylex())) {
        Token t;
        switch (ntoken)
        {
        case NEW_LINE:
            t.type = NEW_LINE;
            result.push_back(t);
            break;
        case TAB_INDENT:
            t.type = TAB_INDENT;
            result.push_back(t);
            break;
        case TAB_DEDENT:
            t.type = TAB_DEDENT;
            result.push_back(t);
            break;
        case DECIMAL:
            t.type = DECIMAL;
            t.value = yytext;
            result.push_back(t);
            break;
        case NUMBER:
            t.type = NUMBER;
            result.push_back(t);
            break;
        case PRINT:
            t.type = PRINT;
            result.push_back(t);
            break;
        case OPEN_PARANTHESIS:
            t.type = OPEN_PARANTHESIS;
            result.push_back(t);
            break;
        case CLOSED_PARANTHESIS:
            t.type = CLOSED_PARANTHESIS;
            result.push_back(t);
            break;
        case OPEN_SQUAREBRACKET:
            t.type = OPEN_SQUAREBRACKET;
            result.push_back(t);
            break;
        case CLOSED_SQUAREBRACKET:
            t.type = CLOSED_SQUAREBRACKET;
            result.push_back(t);
            break;
        case EQUAL:
            t.type = EQUAL;
            result.push_back(t);
            break;
        case PLUS:
            t.type = PLUS;
            result.push_back(t);
            break;
        case MINUS:
            t.type = MINUS;
            result.push_back(t);
            break;
        case MULTIPLY:
            t.type = MULTIPLY;
            result.push_back(t);
            break;
        case DIVIDE:
            t.type = DIVIDE;
            result.push_back(t);
            break;
        case MODULO:
            t.type = MODULO;
            result.push_back(t);
            break;
        case AND:
            t.type = AND;
            result.push_back(t);
            break;
        case OR:
            t.type = OR;
            result.push_back(t);
            break;
        case LESS:
            t.type = LESS;
            result.push_back(t);
            break;
        case NOT:
            t.type = NOT;
            result.push_back(t);
            break;
        case FUNCTION:
            t.type = FUNCTION;
            result.push_back(t);
            break;
        case COMMA:
            t.type = COMMA;
            result.push_back(t);
            break;
        case BOOLVAL:
            t.type = BOOLVAL;
            t.value = yytext;
            result.push_back(t);
            break;
        case FOR:
            t.type = FOR;
            result.push_back(t);
            break;
        case WHILE:
            t.type = WHILE;
            result.push_back(t);
            break;
        case COLON:
            t.type = COLON;
            result.push_back(t);
            break;
        case IF:
            t.type = IF;
            result.push_back(t);
            break;
        case ELSE:
            t.type = ELSE;
            result.push_back(t);
            break;
        case RETURN:
            t.type = RETURN;
            result.push_back(t);
            break;
        case DOT:
            t.type = DOT;
            result.push_back(t);
            break;
        case IS:
            t.type = IS;
            result.push_back(t);
            break;
        case IDENTIFIER:
            t.type = IDENTIFIER;
            t.value = yytext;
            result.push_back(t);
            break;
        case GREATER:
            t.type = GREATER;
            result.push_back(t);
            break;
        case BOOLEAN:
            t.type = BOOLEAN;
            result.push_back(t);
            break;
        case STRING:
            t.type = STRING;
            result.push_back(t);
            break;
        case INPUT:
            t.type = INPUT;
            result.push_back(t);
            break;
        case STRING_VAL:
            t.type = STRING_VAL;
            t.value = yytext;
            result.push_back(t);
            break;
        default:
            // error or unrecognized token
            break;
        }
    }

    FixTabIndent(&result);

    Token t = { EOF };
    result.push_back(t);

    return result;
}

inline void Lexer::FixTabIndent(std::vector<Token>* tokens) {

    int maxTabIndent = 0;
    int currentTabIndent = 0;

    for (size_t i = 0; i < tokens->size(); i++)
    {

        if (i == tokens->size() - 1 && maxTabIndent == 1) {
            Token t = { TAB_DEDENT };
            tokens->push_back(t);
            break;
        }

        // if we find a new line token we read ahead an check amount of tabs
        if (tokens->at(i).type == NEW_LINE) {
            size_t count = 1;
            while (tokens->at(i + count).type == TAB_INDENT)
            {
                currentTabIndent++;
                count++;
            }
            // update maxTabIndent and remove tab indents until 1 left
            if (currentTabIndent > maxTabIndent) {
                int difference = currentTabIndent - maxTabIndent;
                maxTabIndent = currentTabIndent;

                if (currentTabIndent > 1) {
                    auto start = tokens->begin() + i + 2;
                    auto end = tokens->begin() + i + count;
                    tokens->erase(start, end);
                    count = 1;
                }

            }
            // remove all the tab indent tokens
            else if (currentTabIndent == maxTabIndent) {
                auto start = tokens->begin() + i + 1;
                auto end = tokens->begin() + i + count;
                tokens->erase(start, end);
                count = 1;
            }
            // remove all tab indent tokens and add the corresponding dedents
            else if (currentTabIndent < maxTabIndent) {
                int difference = maxTabIndent - currentTabIndent;
                maxTabIndent = currentTabIndent;
                Token t = { TAB_DEDENT };
                for (size_t j = 0; j < difference; j++)
                {
                    tokens->insert(tokens->begin() + i + count, t);
                }
                auto start = tokens->begin() + i + 1;
                auto end = tokens->begin() + i + count;
                tokens->erase(start, end);
                count -= difference - 1;
            }
            i += count - 1;
            currentTabIndent = 0;
        }
    }

}

// just for verifying the amount
void Lexer::CountIndentDedent(std::vector<Token> tokens) {
    int indentCount = 0;
    int dedentCount = 0;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens.at(i).type == TAB_INDENT)
            indentCount++;
        else if (tokens.at(i).type == TAB_DEDENT)
            dedentCount++;
    }

    std::cout << "Indents: " << indentCount << std::endl;
    std::cout << "Dedents: " << dedentCount << std::endl;
}