#include "../pch.h"


class ParserTest : public testing::Test {
public:
	ParserTest() = default;
};


//parse_arithmetic_expr
TEST_F(ParserTest, TestParseArithmeticExprMinus) {

    // Arrange
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ MINUS });
	parser.m_tokens.push_back({ DECIMAL, "2" });

    // Act
    node::NodeArithmeticExpr* result = parser.parse_arithmetic_expr();

    // Assert
    ASSERT_NE(result, nullptr);
    ASSERT_TRUE(mpark::holds_alternative<node::NodeExprSubtract*>(result->var));
}
TEST_F(ParserTest, TestParseArithmeticExprPlus) {
	// Arrange
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ PLUS });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeArithmeticExpr* result = parser.parse_arithmetic_expr();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprAdd*>(result->var));
}

//parse_term
TEST_F(ParserTest, TestParseTermMultiply) {
	// Arrange
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ MULTIPLY });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parser.parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprMult*>(result->var));
}

TEST_F(ParserTest, TestParseTermDivide) {
	// Arrange
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ DIVIDE });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parser.parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprDivide*>(result->var));
}

TEST_F(ParserTest, TestParseTermModulo) {
	// Arrange
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ MODULO });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parser.parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprModulo*>(result->var));
}

//parse_factor
TEST_F(ParserTest, TestParseFactorDecimal) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ DECIMAL, "42" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorDecimal*>(result->var));
}

TEST_F(ParserTest, TestParseFactorIdentifier) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ IDENTIFIER, "x" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorIdentifier*>(result->var));
}

TEST_F(ParserTest, TestParseFactorParenthesizedExpr) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeArithmeticExpr*>(result->var));
}

//Parse_string_expr
TEST_F(ParserTest, testParseStringExpr) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ STRING_VAL, "Test" });

	node::NodeStringExpr* result = parser.parse_string_expr();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeStringValue*>(result->var));
}


//Parse statement
TEST_F(ParserTest, TestParseStmtDeclaration) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ NEW_LINE, "\n" });

	node::NodeStmt* result = parser.parse_stmt();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(result->var));
}

TEST_F(ParserTest, TestParseStmtFunctionCall) {
	Parser parser = Parser({}); // Example test case
	parser.m_tokens.push_back({ IDENTIFIER, "foo" });
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });
	parser.m_tokens.push_back({ NEW_LINE, "\n" });

	node::NodeStmt* result = parser.parse_stmt();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFunctionCall*>(result->var));
}
