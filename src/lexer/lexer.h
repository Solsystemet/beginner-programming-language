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
    int line_num = 1;
    // Sanitizes the tokens to have correct amount of indents and dedents
    inline void FixTabIndent(std::vector<Token>* tokens);

    // helper function for verifying indent and dedent count. move elsewhere or delete
    inline void CountIndentDedent(std::vector<Token> tokens);

public:
    inline std::string SanitizeStringLiteral(const std::string& raw);
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
    line_num = 1;
    std::vector<Token> result;
    while ((ntoken = yylex())) {
        Token t;
        switch (ntoken)
        {
        case NEW_LINE:
            t.type = NEW_LINE;
            t.lineNum = line_num;
            line_num++;
            result.push_back(t);
            break;
        case TAB_INDENT:
            t.type = TAB_INDENT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case TAB_DEDENT:
            t.type = TAB_DEDENT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case DECIMAL:
            t.type = DECIMAL;
            t.lineNum = line_num;
            t.value = yytext;
            result.push_back(t);
            break;
        case NUMBER:
            t.type = NUMBER;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case PRINT:
            t.type = PRINT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case OPEN_PARANTHESIS:
            t.type = OPEN_PARANTHESIS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case CLOSED_PARANTHESIS:
            t.type = CLOSED_PARANTHESIS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case OPEN_SQUAREBRACKET:
            t.type = OPEN_SQUAREBRACKET;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case CLOSED_SQUAREBRACKET:
            t.type = CLOSED_SQUAREBRACKET;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case EQUAL:
            t.type = EQUAL;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case PLUS:
            t.type = PLUS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case MINUS:
            t.type = MINUS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case MULTIPLY:
            t.type = MULTIPLY;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case DIVIDE:
            t.type = DIVIDE;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case MODULO:
            t.type = MODULO;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case AND:
            t.type = AND;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case OR:
            t.type = OR;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case LESS:
            t.type = LESS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case NOT:
            t.type = NOT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case FUNCTION:
            t.type = FUNCTION;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case COMMA:
            t.type = COMMA;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case BOOLVAL:
            t.type = BOOLVAL;
            t.lineNum = line_num;
            t.value = yytext;
            result.push_back(t);
            break;
        case FOR:
            t.type = FOR;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case WHILE:
            t.type = WHILE;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case COLON:
            t.type = COLON;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case IF:
            t.type = IF;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case ELSE:
            t.type = ELSE;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case RETURN:
            t.type = RETURN;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case DOT:
            t.type = DOT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case IS:
            t.type = IS;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case IDENTIFIER:
            t.type = IDENTIFIER;
            t.lineNum = line_num;
            t.value = yytext;
            result.push_back(t);
            break;
        case GREATER:
            t.type = GREATER;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case BOOLEAN:
            t.type = BOOLEAN;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case STRING:
            t.type = STRING;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case INPUT:
            t.type = INPUT;
            t.lineNum = line_num;
            result.push_back(t);
            break;
        case STRING_VAL:
            t.type = STRING_VAL;
            t.lineNum = line_num;
            t.value = SanitizeStringLiteral(yytext);
            result.push_back(t);
            break;
        default:
            // error or unrecognized token
            break;
        }
    }

    FixTabIndent(&result);

    Token t = { EOF, "", line_num};
    result.push_back(t);

    return result;
}

inline void Lexer::FixTabIndent(std::vector<Token>* tokens) {
    int lastIndentLevel = 0;

    for (size_t i = 0; i < tokens->size(); ++i) {
        if (tokens->at(i).type == NEW_LINE) {
            int currentIndentLevel = 0;
            size_t j = i + 1;

            // Count the TAB_INDENTs after the newline
            while (j < tokens->size() && tokens->at(j).type == TAB_INDENT) {
                currentIndentLevel++;
                j++;
            }

            size_t indentStart = i + 1;
            size_t indentEnd = indentStart + currentIndentLevel;

            if (currentIndentLevel > lastIndentLevel) {
                // Keep only the difference (new indent level)
                // Remove extra TAB_INDENTs so total equals current indent level
                if (currentIndentLevel > 1) {
                    tokens->erase(tokens->begin() + indentStart + 1, tokens->begin() + indentEnd);
                }
            }
            else if (currentIndentLevel == lastIndentLevel) {
                // Remove all TAB_INDENTs (they are redundant)
                tokens->erase(tokens->begin() + indentStart, tokens->begin() + indentEnd);
            }
            else if (currentIndentLevel < lastIndentLevel) {
                // Remove all TAB_INDENTs and insert dedents
                tokens->erase(tokens->begin() + indentStart, tokens->begin() + indentEnd);
                int dedentsToInsert = lastIndentLevel - currentIndentLevel;
                tokens->insert(tokens->begin() + indentStart, dedentsToInsert, Token{ TAB_DEDENT, "", line_num });
                i += dedentsToInsert; // move index forward to skip dedents
            }

            lastIndentLevel = currentIndentLevel;
        }
    }

    // Insert remaining dedents at EOF
    while (lastIndentLevel-- > 0) {
        tokens->push_back(Token{ TAB_DEDENT, "", line_num });
    }
}




// just for verifying the amount
inline void Lexer::CountIndentDedent(std::vector<Token> tokens) {
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

std::string Lexer::SanitizeStringLiteral(const std::string& raw) {
    std::string result;

    // Sanity check: must start and end with quotes
    if (raw.size() < 2 || raw.front() != '"' || raw.back() != '"') {
        return raw; // Or throw an error
    }

    for (size_t i = 1; i < raw.size() - 1; ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size() - 1) {
            ++i;
            switch (raw[i]) {
            case 'n': result += '\n'; break;
            case 't': result += '\t'; break;
            case 'r': result += '\r'; break;
            case '"': result += '"';  break;
            case '\\': result += '\\'; break;
            case '0': result += '\0'; break;
            default:
                result += raw[i]; // Unknown escape, keep as-is
                break;
            }
        }
        else {
            result += raw[i];
        }
    }

    return result;
}