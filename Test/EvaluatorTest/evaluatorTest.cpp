#include "../pch.h"


class EvaluatorTest : public Evaluator, public testing::Test {
public:
    EvaluatorTest() : Evaluator(node::NodeProg{}) {}
	void SetUp() override {
		// Initialize the evaluator with an empty program
		m_prog = node::NodeProg{};
	}
	void TearDown() override {
		// Clean up any resources if needed
		for (auto stmt : m_prog.stmts) {
			delete stmt;
		}
		m_prog.stmts.clear();
	}
	void evaluate_print(const node::NodeStmtPrint* print_stmt) override {
		struct PrintVisitor
		{
			EvaluatorTest* evaluator;

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
	std::string output;
};

//parse_arithmetic_expr
TEST_F(EvaluatorTest, TestEvaluatorPrintString) {
    // Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "x" };
		node::NodeStringExpr* expr = new node::NodeStringExpr();
		node::NodeStringValue* val = new node::NodeStringValue();
		val->value = { STRING_VAL, "Test" };
		expr->var = val;
		simp_decl->expr = expr;
		decl->var = simp_decl;
		decl_stmt->var = decl;
		m_prog.stmts.push_back(decl_stmt);
	}
   
	{
		node::NodeStmt* print_stmt = new node::NodeStmt();
		node::NodeStmtPrint* print = new node::NodeStmtPrint();
		node::NodeValue* value = new node::NodeValue();
		node::NodeValueIdentifier* val_ident = new node::NodeValueIdentifier();
		val_ident->identifier = { IDENTIFIER, "x" };
		value->var = val_ident;
		print->value = value;
		print_stmt->var = print;
		m_prog.stmts.push_back(print_stmt);
	}

    // Act
    evaluate_prog();

    // Assert
	ASSERT_TRUE(output == "Test");
}



