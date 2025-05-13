#include "../pch.h"


class ParserTest :public Parser, public testing::Test {
public:
    ParserTest() : Parser(std::vector<Token>{}) {}
};


//parse_arithmetic_expr
TEST_F(ParserTest, TestParseArithmeticExprMinus) {

    // Arrange
    m_tokens.push_back({ DECIMAL, "2" });
    m_tokens.push_back({ MINUS });
    m_tokens.push_back({ DECIMAL, "2" });

    // Act
    node::NodeArithmeticExpr* result = parse_arithmetic_expr();

    // Assert
    ASSERT_NE(result, nullptr);
    ASSERT_TRUE(mpark::holds_alternative<node::NodeExprSubtract*>(result->var));
}
TEST_F(ParserTest, TestParseArithmeticExprPlus) {
	// Arrange
	m_tokens.push_back({ DECIMAL, "2" });
	m_tokens.push_back({ PLUS });
	m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeArithmeticExpr* result = parse_arithmetic_expr();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprAdd*>(result->var));
}

//parse_term
TEST_F(ParserTest, TestParseTermMultiply) {
	// Arrange
	m_tokens.push_back({ DECIMAL, "2" });
	m_tokens.push_back({ MULTIPLY });
	m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprMult*>(result->var));
}

TEST_F(ParserTest, TestParseTermDivide) {
	// Arrange
	m_tokens.push_back({ DECIMAL, "2" });
	m_tokens.push_back({ DIVIDE });
	m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprDivide*>(result->var));
}

TEST_F(ParserTest, TestParseTermModulo) {
	// Arrange
	m_tokens.push_back({ DECIMAL, "2" });
	m_tokens.push_back({ MODULO });
	m_tokens.push_back({ DECIMAL, "2" });
	// Act
	node::NodeTerm* result = parse_term();
	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeExprModulo*>(result->var));
}

//parse_factor
TEST_F(ParserTest, TestParseFactorDecimal) {
	m_tokens.push_back({ DECIMAL, "42" });

	node::NodeFactor* result = parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorDecimal*>(result->var));
}

TEST_F(ParserTest, TestParseFactorIdentifier) {
	m_tokens.push_back({ IDENTIFIER, "x" });

	node::NodeFactor* result = parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorIdentifier*>(result->var));
}

TEST_F(ParserTest, TestParseFactorParenthesizedExpr) {
	m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	m_tokens.push_back({ DECIMAL, "2" });
	m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	node::NodeFactor* result = parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeArithmeticExpr*>(result->var));
}

//Parse_string_expr
TEST_F(ParserTest, testParseStringExpr) {
	m_tokens.push_back({ STRING_VAL, "Test" });

	node::NodeStringExpr* result = parse_string_expr();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeStringValue*>(result->var));
}


//Parse statement
TEST_F(ParserTest, TestParseStmtDeclaration) {
	m_tokens.push_back({ NUMBER, "number" });
	m_tokens.push_back({ IDENTIFIER, "x" });
	m_tokens.push_back({ EQUAL, "=" });
	m_tokens.push_back({ DECIMAL, "1" });
	m_tokens.push_back({ NEW_LINE, "\n" });

	node::NodeStmt* result = ParserTest::parse_stmt();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(result->var));
}

TEST_F(ParserTest, TestParseStmtFunctionCall) {
	m_tokens.push_back({ IDENTIFIER, "foo" });
	m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });
	m_tokens.push_back({ NEW_LINE, "\n" });

	node::NodeStmt* result = parse_stmt();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFunctionCall*>(result->var));
}
