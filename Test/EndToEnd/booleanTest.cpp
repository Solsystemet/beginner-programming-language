#include "../pch.h"

class BooleanTest : public Evaluator, public testing::Test {
public:
    BooleanTest() : Evaluator(node::NodeProg{}) {}
    Lexer* lexer;
    Parser* parser;
    FILE* file;
    std::vector<Token> tokens;
    std::string output;
    void SetUp() override {
        file = fopen("../../language-test/EndToEnd/booleanTest.txt", "r");
    }

    void TearDown() override {
        delete file;
    }

    void evaluate_print(const node::NodeStmtPrint* print_stmt) override {
        struct PrintVisitor
        {
            BooleanTest* evaluator;

            // node value
            void operator()(const node::NodeValue* val) const {
                evaluator->evaluate_value(val);

                auto result = evaluator->m_stack.top();
                evaluator->m_stack.pop();
                testing::internal::CaptureStdout();
                if (mpark::holds_alternative<double>(result)) {
                    std::cout << mpark::get<double>(result);
                }
                else if (mpark::holds_alternative<bool>(result)) {
                    std::cout << mpark::get<bool>(result);
                }
                else if (mpark::holds_alternative<std::string>(result)) {
                    std::cout << mpark::get<std::string>(result);
                }
                evaluator->output += testing::internal::GetCapturedStdout();
            }

        }; mpark::visit(PrintVisitor{ this }, print_stmt->value);
    }
};

TEST_F(BooleanTest, boolean_test) {
    //Arrange lexer
    ASSERT_NE(file, nullptr) << "Failed to open file";
    lexer = new Lexer(file);

    std::vector<Token> expected_lexer_output;
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "age"});
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "19"});
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "age" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "21"});
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "age" });
    expected_lexer_output.push_back({ GREATER });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "18"});
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "\n"});
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "water" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "1.49" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "water" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ DECIMAL, "1.5" });
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "water" });
    expected_lexer_output.push_back({ GREATER });
    expected_lexer_output.push_back({ DECIMAL, "0.1" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "\n" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "36" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ BOOLEAN });
    expected_lexer_output.push_back({ IDENTIFIER, "isLeapYear" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ MODULO });
    expected_lexer_output.push_back({ DECIMAL, "4" });
    expected_lexer_output.push_back({ IS });
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "isLeapYear" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "\n" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "speed" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "45" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "speed" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ DECIMAL, "55" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "\n" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "y" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "7" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "x" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "5" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "z" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "8" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "y" });
    expected_lexer_output.push_back({ GREATER });
    expected_lexer_output.push_back({ IDENTIFIER, "x" });
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "y" });
    expected_lexer_output.push_back({ GREATER });
    expected_lexer_output.push_back({ IDENTIFIER, "z" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "\n" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "w" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "6" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "w" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "6" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ EOF });

    // Act lexer
    tokens = lexer->Tokenize();

    // Assert lexer
    ASSERT_EQ(tokens.size(), expected_lexer_output.size());
    for (size_t i = 0; i < tokens.size(); i++)
    {
        EXPECT_EQ(tokens[i].type, expected_lexer_output[i].type);
        EXPECT_EQ(tokens[i].value, expected_lexer_output[i].value);
    }

    // Arrange parser
    parser = new Parser(tokens);
    node::NodeProg expected_prog;
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());
    expected_prog.stmts.push_back(new node::NodeStmt());

    expected_prog.stmts[0]->var = new node::NodeDecl();
    expected_prog.stmts[1]->var = new node::NodeStmtPrint();
    expected_prog.stmts[2]->var = new node::NodeStmtPrint();
    expected_prog.stmts[3]->var = new node::NodeDecl();
    expected_prog.stmts[4]->var = new node::NodeStmtPrint();
    expected_prog.stmts[5]->var = new node::NodeStmtPrint();
    expected_prog.stmts[6]->var = new node::NodeDecl();
    expected_prog.stmts[7]->var = new node::NodeDecl();
    expected_prog.stmts[8]->var = new node::NodeStmtPrint();
    expected_prog.stmts[9]->var = new node::NodeStmtPrint();
    expected_prog.stmts[10]->var = new node::NodeDecl();
    expected_prog.stmts[11]->var = new node::NodeStmtPrint();
    expected_prog.stmts[12]->var = new node::NodeStmtPrint();
    expected_prog.stmts[13]->var = new node::NodeDecl();
    expected_prog.stmts[14]->var = new node::NodeDecl();
    expected_prog.stmts[15]->var = new node::NodeDecl();
    expected_prog.stmts[16]->var = new node::NodeStmtPrint();
    expected_prog.stmts[17]->var = new node::NodeStmtPrint();
    expected_prog.stmts[18]->var = new node::NodeDecl();
    expected_prog.stmts[19]->var = new node::NodeStmtPrint();


    // Act parser
    node::NodeProg prog = parser->parse_prog();

    // Assert Parser
    ASSERT_TRUE(prog.stmts.size(), expected_prog.stmts.size());
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[0]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[1]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[2]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[3]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[4]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[5]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[6]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[7]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[8]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[9]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[10]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[11]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[12]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[13]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[14]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[15]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[16]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[17]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[18]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[19]->var));

    // Arrange Evaluator
    m_prog = prog;

    // Act Evaluator
    evaluate_prog();

    // Assert Evaluator
    ASSERT_EQ(output, "1\n1\n1\n1\n0\n1");
}