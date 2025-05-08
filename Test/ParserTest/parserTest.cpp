#include "../pch.h"
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Mock class for Parser
class ParserMock : public Parser {
public:
    ParserMock() : Parser(std::vector<Token>{}) {}

    MOCK_METHOD(node::NodeTerm*, parse_term, (), (override));
};

class ParserTest : public testing::Test {
protected:
    ParserMock parser;

    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(ParserTest, TestParseTerm) {
    // Arrange
    node::NodeTerm mockNodeTerm;
    EXPECT_CALL(parser, parse_term())
        .WillOnce(::testing::Return(&mockNodeTerm));

    // Act
    node::NodeTerm* result = parser.parse_term();

    // Assert
    ASSERT_EQ(result, &mockNodeTerm);
}
