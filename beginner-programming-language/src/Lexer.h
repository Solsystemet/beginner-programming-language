#pragma once
#include "Tokens.h"
#include <vector>
#include <regex>
#include <unordered_map>
#include <optional>



class Lexer {
public:
	Lexer(std::string src):
		m_src(std::move(src)) {}

	std::vector<Token> Tokenize() {
		std::vector<Token> result;
		std::string buf;
		

		while (peek().has_value()) {
			char a = peek().value();
			if (std::regex_match(std::string(1, a), std::regex("[a-zA-Z_]"))) {
				buf.push_back(consume());
				a = peek().value();
				while (peek().has_value() && std::regex_match(std::string(1, a), std::regex("[a-zA-Z_0-9]"))) {
					buf.push_back(consume());
					a = peek().value();
				}
				if (m_reservedKeywords.contains(buf)) {
					result.push_back(m_reservedKeywords.at(buf));
					buf.clear();
				}
				else
				{
					result.push_back({ .type = TokenType::identifier, .val = buf });
					buf.clear();
				}
			}
			else if (std::regex_match(std::string(1, a), std::regex("[1-9]"))) {
				buf.push_back(consume());
				a = peek().value();
				while (peek().has_value() && std::regex_match(std::string(1, a), std::regex("[0-9]"))) {
					buf.push_back(consume());
					a = peek().value();
				}
				result.push_back({ .type = TokenType::decimal, .val = buf });
				buf.clear();
			}
			else if (m_specialChars.contains(a)) {
				result.push_back(m_specialChars.at(a));
				consume();
			}
			else if (std::regex_match(std::string(1, a), std::regex(R"(\t)"))) {
				std::cout << "found tab" << std::endl;
				consume();
			}
			else if (std::regex_match(std::string(1, a), std::regex(" "))) {
				consume();
			}
		}
		m_currentIndex = 0;
		result.push_back({ .type = TokenType::end_of_file });
		return result;
	}


private:

	std::optional<char> peek(int offset = 0) {
		if (m_currentIndex + offset >= m_src.length()) return {};
		else return m_src.at(m_currentIndex);
	}

	char consume() {
		return m_src.at(m_currentIndex++);
	}

	std::string m_src;
	int m_currentIndex{};

	std::unordered_map<std::string, Token> m_reservedKeywords = {
		{"number", {.type = TokenType::number}},
		{"print", {.type = TokenType::print}},
	};
	std::unordered_map<char, Token> m_specialChars = {
		{'=', {.type = TokenType::equal}},
		{'+', {.type = TokenType::plus}},
		{'(', {.type = TokenType::open_paranthesis}},
		{')', {.type = TokenType::closed_paranthesis}},
		{'\n', {.type = TokenType::new_line}},
	};
};
