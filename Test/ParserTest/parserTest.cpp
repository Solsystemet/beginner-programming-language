#include "../pch.h"

class ParserTest : public testing::Test {
public:
	ParserTest() = default;
	Parser parser = Parser({});
};

//parse_prog
TEST_F(ParserTest, TestParseProg_MultipleStmts) {
	// Arrange
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "5" });
	parser.m_tokens.push_back({ NEW_LINE });

	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ IDENTIFIER, "y" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "10" });
	parser.m_tokens.push_back({ NEW_LINE });

	// Act
	node::NodeProg result = parser.parse_prog();

	// Assert
	ASSERT_EQ(result.stmts.size(), 2);
	for (auto* stmt : result.stmts) {
		ASSERT_NE(stmt, nullptr);
		ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(stmt->var));
	}
	auto* decl1 = mpark::get<node::NodeDecl*>(result.stmts[0]->var);
	auto* decl2 = mpark::get<node::NodeDecl*>(result.stmts[1]->var);
	ASSERT_NE(decl1, nullptr);
	ASSERT_NE(decl2, nullptr);
	auto* simpleDecl1 = mpark::get<node::NodeSimpleDecl*>(decl1->var);
	auto* simpleDecl2 = mpark::get<node::NodeSimpleDecl*>(decl2->var);
	ASSERT_NE(simpleDecl1, nullptr);
	ASSERT_NE(simpleDecl2, nullptr);
	EXPECT_EQ(simpleDecl1->identifier.value, "x");
	EXPECT_EQ(simpleDecl2->identifier.value, "y");
}

//parse_stmt
TEST_F(ParserTest, TestParseStmt_SimpleDecl) {
	// Arrange
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "5" });
	parser.m_tokens.push_back({ NEW_LINE });

	// Act
	node::NodeStmt* result = parser.parse_stmt();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeDecl*>(result->var));
	auto* decl = mpark::get<node::NodeDecl*>(result->var);
	ASSERT_NE(decl, nullptr);
	ASSERT_EQ(decl->var.index(), 0);
	auto* simpleDecl = mpark::get<node::NodeSimpleDecl*>(decl->var);
	ASSERT_NE(simpleDecl, nullptr);
	EXPECT_EQ(simpleDecl->identifier.value, "x");
}

//parse_decleration
TEST_F(ParserTest, TestParseDecleration_Number) {
	// Arrange
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "5" });

	// Act
	node::NodeDecl* result = parser.parse_decleration();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_EQ(result->var.index(), 0);
	auto* simpleDecl = mpark::get<node::NodeSimpleDecl*>(result->var);
	ASSERT_NE(simpleDecl, nullptr);
	EXPECT_EQ(simpleDecl->identifier.value, "x");
}

//parse_array_decleration
TEST_F(ParserTest, TestParseArrayDeclaration_NumberArray) {
	// Arrange
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ OPEN_SQUAREBRACKET });
	parser.m_tokens.push_back({ CLOSED_SQUAREBRACKET });
	parser.m_tokens.push_back({ IDENTIFIER, "arr" });
	parser.m_tokens.push_back({ EQUAL });
	parser.m_tokens.push_back({ OPEN_SQUAREBRACKET });
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ COMMA });
	parser.m_tokens.push_back({ DECIMAL, "2" });
	parser.m_tokens.push_back({ COMMA });
	parser.m_tokens.push_back({ DECIMAL, "3" });
	parser.m_tokens.push_back({ CLOSED_SQUAREBRACKET });

	// Act
	node::NodeArrayDecl* result = parser.parse_array_decleration();

	// Assert
	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->type.value, "number");
	EXPECT_EQ(result->identifier.value, "arr");
	ASSERT_EQ(result->var.index(), 0);
	auto* numberArray = mpark::get<node::NodeNumberArrayDecl*>(result->var);
	ASSERT_NE(numberArray, nullptr);
	EXPECT_EQ(numberArray->elements.size(), 3);
}

//parse_object_decleration
TEST_F(ParserTest, TestParseObjectDecl_WithAssignment) {
	// Arrange
	parser.m_tokens.push_back({ IDENTIFIER, "Person" });
	parser.m_tokens.push_back({ IDENTIFIER, "p" });
	parser.m_tokens.push_back({ COLON, ":" });
	parser.m_tokens.push_back({ NEW_LINE });
	parser.m_tokens.push_back({ TAB_INDENT });

	// Assignment: x = 5
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL });
	parser.m_tokens.push_back({ DECIMAL, "5" });
	parser.m_tokens.push_back({ NEW_LINE });

	parser.m_tokens.push_back({ TAB_DEDENT });
	// Act
	node::NodeObjectDecl* result = parser.parse_object_decleration();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_EQ(result->properties.size(), 1);
	ASSERT_NE(result->properties[0], nullptr);
}

//parse_nested_stmt
TEST_F(ParserTest, TestParseNestedStmt_Assignment) {
	// Arrange
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL });
	parser.m_tokens.push_back({ DECIMAL, "5" });
	parser.m_tokens.push_back({ NEW_LINE });

	// Act
	node::NodeNestedStmt* result = parser.parse_nested_stmt();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeAssignment*>(result->var));
}

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

//parse_string_expr
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

//parse_boolean_expr
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

//parse_function_call
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

//parse_value
TEST_F(ParserTest, TestParseValueDecimal) {
	// Arrange
	parser.m_tokens.push_back({ DECIMAL, "42" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueArithmeticExpression*>(result->var));
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
	// Arrange: 
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ IS });
	parser.m_tokens.push_back({ DECIMAL, "1" });

	// Act
	node::NodeValue* result = parser.parse_value();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueBooleanExpression*>(result->var));
}

//parse_assignment
TEST_F(ParserTest, TestParseAssignmentSimple) {
	// Arrange: x = 42
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL, "=" });
	parser.m_tokens.push_back({ DECIMAL, "42" });

	// Act
	node::NodeAssignment* result = parser.parse_assignment();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_EQ(result->identifierHead.value, "x");
	ASSERT_NE(result->rhs, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeValueArithmeticExpression*>(result->rhs->var));
}

//parse_global_control_flow
TEST_F(ParserTest, TestParseGlobalControlFlow) {
	// Arrange
	parser.m_tokens.push_back({ IF, "if" });
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ IS, "is" });
	parser.m_tokens.push_back({ DECIMAL, "1" });
	parser.m_tokens.push_back({ COLON, ":" });
	parser.m_tokens.push_back({ TAB_INDENT });
	parser.m_tokens.push_back({ IDENTIFIER, "x" });
	parser.m_tokens.push_back({ EQUAL });
	parser.m_tokens.push_back({ NUMBER, "3" });
	parser.m_tokens.push_back({ TAB_DEDENT });

	// Act
	node::NodeGlobalControlFlow* result = parser.parse_global_control_flow();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeGlobalIf*>(result->var));
	auto* ifNode = mpark::get<node::NodeGlobalIf*>(result->var);
	ASSERT_NE(ifNode, nullptr);
	ASSERT_NE(ifNode->condition, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeBooleanOr*>(ifNode->condition->expr));
	ASSERT_TRUE(ifNode->stmts.size() > 0);
}

//parse_definition
TEST_F(ParserTest, TestParseDefinition_Function) {
	// Arrange: def number foo():
	parser.m_tokens.push_back({ NUMBER, "number" });
	parser.m_tokens.push_back({ FUNCTION });
	parser.m_tokens.push_back({ IDENTIFIER, "foo" });
	parser.m_tokens.push_back({ OPEN_PARANTHESIS, "(" });
	parser.m_tokens.push_back({ CLOSED_PARANTHESIS, ")" });
	parser.m_tokens.push_back({ COLON, ":" });

	// Act
	node::NodeDefinition* result = parser.parse_definition();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeFunctionDefinition*>(result->var));
	auto* funcDef = mpark::get<node::NodeFunctionDefinition*>(result->var);
	ASSERT_NE(funcDef, nullptr);
	ASSERT_EQ(funcDef->functionName.value, "foo");
	ASSERT_EQ(funcDef->args.size(), 0);
	ASSERT_EQ(funcDef->stmts.size(), 0);
}

TEST_F(ParserTest, TestParseDefinition_Object) {
	// Arrange: object Foo:
	parser.m_tokens.push_back({ IDENTIFIER, "object" });
	parser.m_tokens.push_back({ COLON });
	parser.m_tokens.push_back({ NEW_LINE });
	parser.m_tokens.push_back({ TAB_INDENT });
	parser.m_tokens.push_back({ TAB_DEDENT });

	// Act
	node::NodeDefinition* result = parser.parse_definition();

	// Assert
	ASSERT_NE(result, nullptr);
	ASSERT_TRUE(mpark::holds_alternative<node::NodeObjectDefinition*>(result->var));
	auto* objDef = mpark::get<node::NodeObjectDefinition*>(result->var);
	ASSERT_NE(objDef, nullptr);
	ASSERT_EQ(objDef->identifier.value, "object");
}
