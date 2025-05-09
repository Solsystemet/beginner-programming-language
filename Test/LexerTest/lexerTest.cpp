#include "../pch.h"
class LexerTest : public testing::Test {
public:
    Lexer* lexer;
    FILE* file;
    std::vector<Token> tokens;
    void SetUp() override {
        file = fopen("C:\\repos\\beginner-programming-language\\language-test\\test.txt", "r");
        ASSERT_NE(file, nullptr) << "Failed to open file";
        lexer = new Lexer(file);
        tokens = lexer->Tokenize();
    }

    void TearDown() override {
        fclose(file);
        delete lexer;
    }
};
TEST_F(LexerTest, TokenizeTest) {
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