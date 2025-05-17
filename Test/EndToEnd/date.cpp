#include "../pch.h"

class DateTest : public Evaluator, public testing::Test {
public:
    DateTest() : Evaluator(node::NodeProg{}) {}
    Lexer* lexer;
    Parser* parser;
    FILE* file;
    std::vector<Token> tokens;
    std::string output;
    void SetUp() override {
    }

    void TearDown() override {
    }

    void evaluate_print(const node::NodeStmtPrint* print_stmt) override {
        struct PrintVisitor
        {
            DateTest* evaluator;

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

TEST_F(DateTest, date_test) {
    //Arrange lexer
    file = fopen("../../Test/EndToEnd/date.txt", "r");
    ASSERT_NE(file, nullptr) << "Failed to open file";
    lexer = new Lexer(file);

    std::vector<Token> expected_lexer_output;

    expected_lexer_output.push_back({ IDENTIFIER, "date"});
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "day"});
    expected_lexer_output.push_back({ EQUAL});
    expected_lexer_output.push_back({ DECIMAL, "0"});
    expected_lexer_output.push_back({ NEW_LINE});

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ NUMBER });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "0" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_DEDENT });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ BOOLEAN });
    expected_lexer_output.push_back({ FUNCTION });
    expected_lexer_output.push_back({ IDENTIFIER, "date_before" });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "date" });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ COMMA });
    expected_lexer_output.push_back({ IDENTIFIER, "date" });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ BOOLEAN });
    expected_lexer_output.push_back({ IDENTIFIER, "result"});
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT});
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ LESS});
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ OR });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ IS });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ OR });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ IS });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ IS });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ AND });
    expected_lexer_output.push_back({ IDENTIFIER, "d1" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "day" });
    expected_lexer_output.push_back({ LESS });
    expected_lexer_output.push_back({ IDENTIFIER, "d2" });
    expected_lexer_output.push_back({ DOT });
    expected_lexer_output.push_back({ IDENTIFIER, "day" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ RETURN });
    expected_lexer_output.push_back({ IDENTIFIER, "result" });
    expected_lexer_output.push_back({ NEW_LINE });
    expected_lexer_output.push_back({ TAB_DEDENT });

    expected_lexer_output.push_back({ IDENTIFIER, "date" });
    expected_lexer_output.push_back({ IDENTIFIER, "today" });
    expected_lexer_output.push_back({ COLON});
    expected_lexer_output.push_back({ NEW_LINE});

    expected_lexer_output.push_back({ TAB_INDENT});
    expected_lexer_output.push_back({ IDENTIFIER, "day" });
    expected_lexer_output.push_back({ EQUAL});
    expected_lexer_output.push_back({ DECIMAL, "8"});
    expected_lexer_output.push_back({ NEW_LINE});

    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "11" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "2011" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_DEDENT });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "date" });
    expected_lexer_output.push_back({ IDENTIFIER, "tomorrow" });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ IDENTIFIER, "day" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "9" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "month" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "11" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IDENTIFIER, "year" });
    expected_lexer_output.push_back({ EQUAL });
    expected_lexer_output.push_back({ DECIMAL, "2012" });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_DEDENT });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ IF });
    expected_lexer_output.push_back({ IDENTIFIER, "date_before" });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS});
    expected_lexer_output.push_back({ IDENTIFIER, "today"});
    expected_lexer_output.push_back({ COMMA});
    expected_lexer_output.push_back({ IDENTIFIER, "tomorrow" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS});
    expected_lexer_output.push_back({ COLON});
    expected_lexer_output.push_back({ NEW_LINE});

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "OK"});
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });
    expected_lexer_output.push_back({ TAB_DEDENT });

    expected_lexer_output.push_back({ ELSE });
    expected_lexer_output.push_back({ COLON });
    expected_lexer_output.push_back({ NEW_LINE });

    expected_lexer_output.push_back({ TAB_INDENT });
    expected_lexer_output.push_back({ PRINT });
    expected_lexer_output.push_back({ OPEN_PARANTHESIS });
    expected_lexer_output.push_back({ STRING_VAL, "Problems!" });
    expected_lexer_output.push_back({ CLOSED_PARANTHESIS });
    expected_lexer_output.push_back({ NEW_LINE });
    expected_lexer_output.push_back({ TAB_DEDENT });
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

    expected_prog.stmts[0]->var = new node::NodeDefinition();
    expected_prog.stmts[1]->var = new node::NodeDefinition();
    expected_prog.stmts[2]->var = new node::NodeDecl();
    expected_prog.stmts[3]->var = new node::NodeDecl();
    expected_prog.stmts[4]->var = new node::NodeGlobalControlFlow();

    // Act parser
    node::NodeProg prog = parser->parse_prog();

    // Assert Parser
    ASSERT_EQ(prog.stmts.size(), expected_prog.stmts.size());

    ASSERT_TRUE(mpark::holds_alternative<node::NodeDefinition*>(prog.stmts[0]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDefinition*>(prog.stmts[1]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[2]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(prog.stmts[3]->var));
    ASSERT_TRUE(mpark::holds_alternative<node::NodeGlobalControlFlow*>(prog.stmts[4]->var));

    // Arrange Evaluator
     m_prog = prog;

    // Act Evaluator
     evaluate_prog();

    // Assert Evaluator
    ASSERT_EQ(output, "OK");
}