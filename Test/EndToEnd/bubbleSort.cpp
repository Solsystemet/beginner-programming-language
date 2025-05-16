#include "../pch.h"

class BubbleSortTest : public Evaluator, public testing::Test {
public:
    BubbleSortTest() : Evaluator(node::NodeProg{}) {}
    Lexer* lexer;
    Parser* parser;
    FILE* file;
    std::vector<Token> tokens;
    std::string output;
    void SetUp() override {
        file = fopen("../../language-test/EndToEnd/bubbleSort.txt", "r");
    }

    void TearDown() override {
        delete file;
    }

    void evaluate_print(const node::NodeStmtPrint* print_stmt) override {
        struct PrintVisitor
        {
            BubbleSortTest* evaluator;

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

TEST_F(BubbleSortTest, BubbleSort) {
    //Arrange lexer
    ASSERT_NE(file, nullptr) << "Failed to open file";
    lexer = new Lexer(file);

    std::vector<Token> expected_lexer_output;
    expected_lexer_output.push_back({NUMBER});
    expected_lexer_output.push_back({OPEN_SQUAREBRACKET});
    expected_lexer_output.push_back({CLOSED_SQUAREBRACKET});
    expected_lexer_output.push_back({FUNCTION});
    expected_lexer_output.push_back({IDENTIFIER, "bubble_sort"});
    expected_lexer_output.push_back({OPEN_PARANTHESIS});
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({CLOSED_PARANTHESIS});
    expected_lexer_output.push_back({COLON});
    expected_lexer_output.push_back({NEW_LINE});

    expected_lexer_output.push_back({TAB_INDENT});
    expected_lexer_output.push_back({NUMBER});
    expected_lexer_output.push_back({ IDENTIFIER, "n" });
    expected_lexer_output.push_back({ EQUAL});
    expected_lexer_output.push_back({ DECIMAL, "5"});
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ FOR });
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "i"});
    expected_lexer_output.push_back({ EQUAL});
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ IDENTIFIER, "i" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ IDENTIFIER, "n" });
    expected_lexer_output.push_back({ MINUS });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ FOR });
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ IDENTIFIER, "n" });
    expected_lexer_output.push_back({ MINUS });
    expected_lexer_output.push_back({ IDENTIFIER, "i" });
    expected_lexer_output.push_back({ MINUS });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ IF });
    expected_lexer_output.push_back({ IDENTIFIER, "arr"});
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET});
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET});
    expected_lexer_output.push_back({ GREATER });
    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ PLUS });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "temp"});
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ PLUS });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "j" });
    expected_lexer_output.push_back({ PLUS });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ IDENTIFIER, "temp" });
    expected_lexer_output.push_back({ NEW_LINE });
    expected_lexer_output.push_back({ TAB_DEDENT });
    expected_lexer_output.push_back({ TAB_DEDENT });
    expected_lexer_output.push_back({ TAB_DEDENT });

    expected_lexer_output.push_back({ RETURN });
    expected_lexer_output.push_back({ IDENTIFIER, "arr" });
    expected_lexer_output.push_back({ NEW_LINE });
    expected_lexer_output.push_back({ TAB_DEDENT });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "1"});
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "10" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "69" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "5" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ DECIMAL, "3" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ IDENTIFIER, "bubble_sort" });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "1" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "2" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "3" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "example" });
    expected_lexer_output.push_back({ OPEN_SQUAREBRACKET });
    expected_lexer_output.push_back({ DECIMAL, "4" });
    expected_lexer_output.push_back({ CLOSED_SQUAREBRACKET });
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

    node::NodeStmt* stmt1 = new node::NodeStmt();
    node::NodeDefinition* definition = new node::NodeDefinition();
    stmt1->var = definition;
    expected_prog.stmts.push_back(stmt1);

    node::NodeStmt* stmt2 = new node::NodeStmt();
    node::NodeDecl* decl = new node::NodeDecl();
    stmt2->var = decl;
    expected_prog.stmts.push_back(stmt2);

    node::NodeStmt* stmt3 = new node::NodeStmt();
    node::NodeAssignment* ass = new node::NodeAssignment();
    stmt3->var = ass;
    expected_prog.stmts.push_back(stmt3);

    node::NodeStmt* stmt4 = new node::NodeStmt();
    node::NodeStmtPrint* print1 = new node::NodeStmtPrint();
    stmt4->var = print1;
    expected_prog.stmts.push_back(stmt4);

    node::NodeStmt* stmt5 = new node::NodeStmt();
    node::NodeStmtPrint* print2 = new node::NodeStmtPrint();
    stmt5->var = print2;
    expected_prog.stmts.push_back(stmt5);

    node::NodeStmt* stmt6 = new node::NodeStmt();
    node::NodeStmtPrint* print3 = new node::NodeStmtPrint();
    stmt6->var = print3;
    expected_prog.stmts.push_back(stmt6);

    node::NodeStmt* stmt7 = new node::NodeStmt();
    node::NodeStmtPrint* print4 = new node::NodeStmtPrint();
    stmt7->var = print4;
    expected_prog.stmts.push_back(stmt7);

    node::NodeStmt* stmt8 = new node::NodeStmt();
    node::NodeStmtPrint* print5 = new node::NodeStmtPrint();
    stmt8->var = print5;
    expected_prog.stmts.push_back(stmt8);


    // Act parser
    node::NodeProg prog = parser->parse_prog();

    // Assert Parser
    ASSERT_EQ(prog.stmts.size(), expected_prog.stmts.size());

    ASSERT_TRUE(mpark::holds_alternative<node::NodeDefinition*>(prog.stmts[0]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[1]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeAssignment*>(prog.stmts[2]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[3]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[4]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[5]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[6]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeStmtPrint*>(prog.stmts[7]->var));

    // Arrange Evaluator
    m_prog = prog;

    // Act Evaluator
    evaluate_prog();

    // Assert Evaluator output
    ASSERT_EQ(output, "1351069");
}