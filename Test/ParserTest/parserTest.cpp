#include "../pch.h"


class ParserTest : public testing::Test {
public:
	ParserTest() = default;
	Parser parser = Parser({});

};


//parse_arithmetic_expr
TEST_F(ParserTest, TestParseArithmeticExprMinus) {

    // Arrange
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
	parser.m_tokens.push_back({ DECIMAL, "42" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorDecimal*>(result->var));
}

TEST_F(ParserTest, TestParseFactorIdentifier) {
	parser.m_tokens.push_back({ IDENTIFIER, "x" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFactorIdentifier*>(result->var));
}

TEST_F(ParserTest, TestParseFactorParenthesizedExpr) {
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	node::NodeFactor* result = parser.parse_factor();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeArithmeticExpr*>(result->var));
}

//Parse_string_expr
TEST_F(ParserTest, testParseStringExpr) {
	parser.m_tokens.push_back({ STRING_VAL, "Test" });

	node::NodeStringExpr* result = parser.parse_string_expr();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeStringValue*>(result->var));
}

TEST_F(ParserTest, TestParseStringExpr_IdentifierWithIndex) {
	parser.m_tokens.push_back({ IDENTIFIER, "arr" });
	parser.m_tokens.push_back({ OPEN_SQUAREBRACKET, "[" });
	parser.m_tokens.push_back({ DECIMAL, "0" });
	parser.m_tokens.push_back({ CLOSED_SQUAREBRACKET, "]" });

	node::NodeStringExpr* result = parser.parse_string_expr();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeStringIdentifier*>(result->var));
}

TEST_F(ParserTest, TestParseStringExpr_FunctionCall) {
	parser.m_tokens.push_back({ IDENTIFIER, "foo" });
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	node::NodeStringExpr* result = parser.parse_string_expr();

	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFunctionCall*>(result->var));
}


// parse_boolean_expr
TEST_F(ParserTest, TestParseBooleanExprl) {
	// Arrange
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ IS });
	parser.m_tokens.push_back({ DECIMAL, "2" });



	// Act
	node::NodeBooleanExpr* result = parser.parse_boolean_expr();

	// Assert

	ASSERT_NE(result, nullptr);
    ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanOr*>(result->expr));
	auto* orNode = mpark::get<node::NodeBooleanOr*>(result->expr);

	ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanAnd*>(orNode->var));
	auto* andNode = mpark::get<node::NodeBooleanAnd*>(orNode->var);

	ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanEqual*>(andNode->var));
	auto* equalNode = mpark::get<node::NodeBooleanEqual*>(andNode->var);

	ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanEqualIs*>(equalNode->var));

}

TEST_F(ParserTest, TestParseBooleanGreaterEqual) {
	// Arrange
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ GREATER });
	parser.m_tokens.push_back({ EQUAL });
	parser.m_tokens.push_back({ DECIMAL, "1" });

	// Act
	node::NodeBooleanExpr* result = parser.parse_boolean_expr();

	// Assert
	ASSERT_NE(result, nullptr);
	auto* orNode = mpark::get<node::NodeBooleanOr*>(result->expr);
	ASSERT_NE(orNode, nullptr);
	auto* andNode = mpark::get<node::NodeBooleanAnd*>(orNode->var);
	ASSERT_NE(andNode, nullptr);
	auto* equalNode = mpark::get<node::NodeBooleanEqual*>(andNode->var);
	ASSERT_NE(equalNode, nullptr);
	auto* realExpr = mpark::get<node::NodeBooleanRealExpression*>(equalNode->var);
	ASSERT_NE(realExpr, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanGreaterEqual*>(realExpr->var));
}

TEST_F(ParserTest, TestParseFunctionCall) {
	// Arrange
	parser.m_tokens.push_back({ IDENTIFIER, "foo" });
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ COMMA, "," });
	parser.m_tokens.push_back({ DECIMAL, "3" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	// Act
	node::NodeFunctionCall* result = parser.parse_function_Call();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_EQ(result->functionName.value, "foo");
	ASSERT_EQ(result->args.size(), 2);
}

TEST_F(ParserTest, TestParseValueDecimal) {
	// Arrange
	parser.m_tokens.push_back({ DECIMAL, "42" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueArithmeticExpression*>(result->var));
}

TEST_F(ParserTest, TestParseValueIdentifier) {
	// Arrange
	parser.m_tokens.push_back({ IDENTIFIER, "x" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueIdentifier*>(result->var));
}

TEST_F(ParserTest, TestParseValueFunctionCall) {
	// Arrange: foo()
	parser.m_tokens.push_back({ IDENTIFIER, "foo" });
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueFunctionCall*>(result->var));
}

TEST_F(ParserTest, TestParseValueString) {
	// Arrange
	parser.m_tokens.push_back({ STRING_VAL, "hello" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueStringExpression*>(result->var));
}

TEST_F(ParserTest, TestParseValueBoolean) {
	// Arrange: 1 is 1
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ IS });
	parser.m_tokens.push_back({ DECIMAL, "1" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueBooleanExpression*>(result->var));
}



