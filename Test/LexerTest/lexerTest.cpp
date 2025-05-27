#include "../pch.h"

class LexerTest : public testing::Test {
public:
    Lexer* lexer;
    FILE* file;
    std::vector<Token> tokens;
    void SetUp() override {
    }

    void TearDown() override {
    }
};
TEST_F(LexerTest, TokenizeTest) {
    //Opens the file inside of the test, due to diffrent file, to different test 
    file = fopen("../../Test/TestFiles/test.txt", "r");
    ASSERT_NE(file, nullptr) << "Failed to open file";
    lexer = new Lexer(file);
    tokens = lexer->Tokenize();

    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].type, NEW_LINE);
    EXPECT_EQ(tokens[1].type, TAB_INDENT);
    EXPECT_EQ(tokens[2].type, DECIMAL);
    EXPECT_EQ(tokens[3].type, IDENTIFIER);
    EXPECT_EQ(tokens[4].type, COLON);
    EXPECT_EQ(tokens[5].type, IF);
    EXPECT_EQ(tokens[6].type, ELSE);
    EXPECT_EQ(tokens[7].type, RETURN);
    EXPECT_EQ(tokens[8].type, TAB_DEDENT);
    EXPECT_EQ(tokens[9].type, EOF);
}



TEST_F(LexerTest, AllTokensTest) {
    file = fopen("../../Test/TestFiles/allTokensTest.txt", "r");
    ASSERT_NE(file, nullptr) << "Failed to open allTokensTest.txt";

    lexer = new Lexer(file);
    tokens = lexer->Tokenize();

    ASSERT_GE(tokens.size(), 38);

    EXPECT_EQ(tokens[0].type, NEW_LINE);
    EXPECT_EQ(tokens[1].type, TAB_INDENT);
    EXPECT_EQ(tokens[2].type, DECIMAL);
    EXPECT_EQ(tokens[3].type, NUMBER);
    EXPECT_EQ(tokens[4].type, IDENTIFIER);
    EXPECT_EQ(tokens[5].type, PRINT);
    EXPECT_EQ(tokens[6].type, OPEN_PARANTHESIS);
    EXPECT_EQ(tokens[7].type, CLOSED_PARANTHESIS);
    EXPECT_EQ(tokens[8].type, EQUAL);
    EXPECT_EQ(tokens[9].type, PLUS);
    EXPECT_EQ(tokens[10].type, MINUS);
    EXPECT_EQ(tokens[11].type, MULTIPLY);
    EXPECT_EQ(tokens[12].type, DIVIDE);
    EXPECT_EQ(tokens[13].type, MODULO);
    EXPECT_EQ(tokens[14].type, AND);
    EXPECT_EQ(tokens[15].type, OR);
    EXPECT_EQ(tokens[16].type, NOT);
    EXPECT_EQ(tokens[17].type, FUNCTION);
    EXPECT_EQ(tokens[18].type, COMMA);
    EXPECT_EQ(tokens[19].type, BOOLVAL);
    EXPECT_EQ(tokens[20].type, FOR);
    EXPECT_EQ(tokens[21].type, WHILE);
    EXPECT_EQ(tokens[22].type, COLON);
    EXPECT_EQ(tokens[23].type, IF);
    EXPECT_EQ(tokens[24].type, ELSE);
    EXPECT_EQ(tokens[25].type, RETURN);
    EXPECT_EQ(tokens[26].type, OPEN_SQUAREBRACKET);
    EXPECT_EQ(tokens[27].type, CLOSED_SQUAREBRACKET);
    EXPECT_EQ(tokens[28].type, DOT);
    EXPECT_EQ(tokens[29].type, IS);
    EXPECT_EQ(tokens[30].type, NEW_LINE);
    EXPECT_EQ(tokens[31].type, TAB_DEDENT);
    EXPECT_EQ(tokens[32].type, STRING);
    EXPECT_EQ(tokens[33].type, BOOLEAN);
    EXPECT_EQ(tokens[34].type, STRING_VAL);
    EXPECT_EQ(tokens[35].type, INPUT);
    EXPECT_EQ(tokens[36].type, LESS);
    EXPECT_EQ(tokens[37].type, GREATER);
    EXPECT_EQ(tokens[38].type, EOF);
}

