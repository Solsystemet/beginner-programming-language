#include "../pch.h"

class ParserMock : public Parser {
public:
    ParserMock() : Parser(std::vector<Token>{}){}
};


class ParserTest : public testing::Test {
public:
    ParserTest() = default;

protected:
    ParserMock parser;

    void SetUp() override {}
    void TearDown() override {}
};


TEST_F(ParserTest, TestParseArithmeticExpr) {

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


