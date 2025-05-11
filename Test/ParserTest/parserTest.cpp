#include "../pch.h"

class ParserMock : public Parser {
public:
    ParserMock() : Parser(std::vector<Token>{}) {}

    MOCK_METHOD(node::NodeTerm*, parse_term, (), (override));

    MOCK_METHOD(void, try_consume_arithmetic,
        (int tokenType, node::NodeArithmeticExpr* expr, node::NodeExprAdd* add), (override));

    MOCK_METHOD(void, try_consume_arithmetic,
        (int tokenType, node::NodeArithmeticExpr* expr, node::NodeExprSubtract* sub), (override));
};


class ParserTest : public Parser, public testing::Test {
protected:
    ParserMock parser;
    node::NodeArithmeticExpr* parse_arithmetic_expr();

    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(ParserTest, TestParseArithmeticExpr) {
    // Arrange
    node::NodeTerm mockTerm1, mockTerm2, mockTerm3;
    node::NodeExprAdd mockAdd;
    node::NodeExprSubtract mockSubtract;

    EXPECT_CALL(parser, parse_term())
        .WillOnce(::testing::Return(&mockTerm1)) 
        .WillOnce(::testing::Return(&mockTerm2)) 
        .WillOnce(::testing::Return(&mockTerm3)); 

    EXPECT_CALL(parser, try_consume_arithmetic(PLUS, ::testing::_, &mockAdd))
        .WillOnce(::testing::Invoke([](int, node::NodeArithmeticExpr* expr, node::NodeExprAdd* add) {
        expr->var = add;
            }));

    EXPECT_CALL(parser, try_consume_arithmetic(MINUS, ::testing::_, &mockSubtract))
        .WillOnce(::testing::Invoke([](int, node::NodeArithmeticExpr* expr, node::NodeExprSubtract* sub) {
        expr->var = sub;
            }));

    // Act
    node::NodeArithmeticExpr* result = parse_arithmetic_expr();

    // Assert
    ASSERT_NE(result, nullptr);
    ASSERT_TRUE(mpark::holds_alternative<node::NodeExprSubtract*>(result->var));
}


