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
TEST_F(EvaluatorTest, TestEvaluatorPrintNumber) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "x" };
		node::NodeArithmeticExpr* expr = new node::NodeArithmeticExpr();
		node::NodeFactor* factor = new node::NodeFactor();
		node::NodeFactorDecimal* val = new node::NodeFactorDecimal();
		node::NodeTerm* term = new node::NodeTerm();
		val->decimal = { DECIMAL, "20"};
		factor->var = val;
		term->var = factor;
		expr->var = term;

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
	ASSERT_TRUE(output == "20");
}
TEST_F(EvaluatorTest, TestEvaluatorPrintBooleanTrue) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "x" };
		node::NodeBooleanExpr* expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* or = new node::NodeBooleanOr();
		node::NodeBooleanAnd * and = new node::NodeBooleanAnd(); 
		node::NodeBooleanEqual* equal = new node::NodeBooleanEqual(); 
		node::NodeBooleanRealExpression* realexpr = new node::NodeBooleanRealExpression(); 
		node::NodeBooleanNot* not = new node::NodeBooleanNot();
		node::NodeBooleanFactor* factor = new node::NodeBooleanFactor();
	 	node::NodeBooleanFactorValue* val = new node::NodeBooleanFactorValue();
		val->boolval = { BOOLVAL, "true" };
		factor->var = val;
		not->var = factor;
		realexpr->var = not;
		equal->var = realexpr;
		and ->var = equal;
		or ->var = and;
		expr->expr = or ;
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
	ASSERT_EQ(output, "1");
}



TEST_F(EvaluatorTest, TestEvaluatorPrintBooleanFalse) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "x" };
		node::NodeBooleanExpr* expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* or = new node::NodeBooleanOr();
		node::NodeBooleanAnd* and = new node::NodeBooleanAnd();
		node::NodeBooleanEqual* equal = new node::NodeBooleanEqual();
		node::NodeBooleanRealExpression* realexpr = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot * not = new node::NodeBooleanNot();
		node::NodeBooleanFactor* factor = new node::NodeBooleanFactor();
		node::NodeBooleanFactorValue* val = new node::NodeBooleanFactorValue();
		val->boolval = { BOOLVAL, "false" };
		factor->var = val;
		not->var = factor;
		realexpr->var = not;
		equal->var = realexpr;
		and ->var = equal;
		or ->var = and;
		expr->expr = or ;
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
	ASSERT_EQ(output, "0");
}


TEST_F(EvaluatorTest, TestEvaluatorPrintObjectProperty) {
	// Arrange

	node::NodeStmt* def_stmt = new node::NodeStmt();
	node::NodeDefinition* def = new node::NodeDefinition();
	def_stmt->var = def;
	node::NodeObjectDefinition* myObjectDef = new node::NodeObjectDefinition();
	def->var = myObjectDef;
	myObjectDef->identifier = { IDENTIFIER, "MyObject" };

	
	node::NodeDecl* property_decl = new node::NodeDecl();
	node::NodeSimpleDecl* property_simple_decl = new node::NodeSimpleDecl();
	property_simple_decl->identifier = { IDENTIFIER, "property1" };
	property_decl->var = property_simple_decl;
	node::NodeStringExpr* str_expr = new node::NodeStringExpr();
	node::NodeStringValue* str_val = new node::NodeStringValue();
	str_val->value = { STRING_VAL, "Hello" };
	str_expr->var = str_val;
	property_simple_decl->expr = str_expr;

	myObjectDef->props.push_back(property_decl);
	m_prog.stmts.push_back(def_stmt);

	
	node::NodeStmt* decl_stmt = new node::NodeStmt();
	node::NodeDecl* decl = new node::NodeDecl();
	node::NodeObjectDecl* obj_decl = new node::NodeObjectDecl();
	obj_decl->objectType = { IDENTIFIER, "MyObject" };
	obj_decl->identifier = { IDENTIFIER, "obj" };

	node::NodeAssignment* property = new node::NodeAssignment();
	property->identifierHead = { IDENTIFIER, "property1" };

	node::NodeValue* value = new node::NodeValue();
	node::NodeValueArithmeticExpression* val_expr = new node::NodeValueArithmeticExpression();
	node::NodeArithmeticExpr* arith_expr = new node::NodeArithmeticExpr();
	node::NodeTerm* term = new node::NodeTerm();
	node::NodeFactor* factor = new node::NodeFactor();
	node::NodeFactorDecimal* decimal = new node::NodeFactorDecimal();
	decimal->decimal = { DECIMAL, "42" };
	factor->var = decimal;
	term->var = factor;
	arith_expr->var = term;
	val_expr->expr = arith_expr;
	value->var = val_expr;
	property->rhs = value;

	obj_decl->properties.push_back(property);
	decl->var = obj_decl;
	decl_stmt->var = decl;
	m_prog.stmts.push_back(decl_stmt);

	node::NodeStmt* print_stmt = new node::NodeStmt();
	node::NodeStmtPrint* print = new node::NodeStmtPrint();
	node::NodeValue* value2 = new node::NodeValue();
	node::NodeValueIdentifierProperty* val_ident_prop = new node::NodeValueIdentifierProperty();
	val_ident_prop->identfierHead = { IDENTIFIER, "obj" };
	val_ident_prop->identifierproperties.push_back({ IDENTIFIER, "property1" });
	value2->var = val_ident_prop;
	print->value = value2;
	print_stmt->var = print;
	m_prog.stmts.push_back(print_stmt);

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "42");
}




TEST_F(EvaluatorTest, TestEvaluatorPrintGreaterThan) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "result" };

		
		node::NodeBooleanExpr* expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* boolOr = new node::NodeBooleanOr();
		node::NodeBooleanAnd* boolAnd = new node::NodeBooleanAnd();
		node::NodeBooleanEqual* boolEqual = new node::NodeBooleanEqual();
		node::NodeBooleanRealExpression* realExpr = new node::NodeBooleanRealExpression();

		
		node::NodeBooleanGreater* greaterThan = new node::NodeBooleanGreater();

		
		node::NodeBooleanRealExpression* lhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* lhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* lhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* lhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* lhsTerm = new node::NodeTerm();
		node::NodeFactor* lhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* lhsDecimal = new node::NodeFactorDecimal();
		lhsDecimal->decimal = { DECIMAL, "4" };
		lhsTermFactor->var = lhsDecimal;
		lhsTerm->var = lhsTermFactor;
		lhsExpr->var = lhsTerm;
		lhsFactor->var = lhsExpr;
		lhsNot->var = lhsFactor;
		lhs->var = lhsNot;

		node::NodeBooleanNot* rhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* rhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* rhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* rhsTerm = new node::NodeTerm();
		node::NodeFactor* rhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* rhsDecimal = new node::NodeFactorDecimal();
		rhsDecimal->decimal = { DECIMAL, "2" };
		rhsTermFactor->var = rhsDecimal;
		rhsTerm->var = rhsTermFactor;
		rhsExpr->var = rhsTerm;
		rhsFactor->var = rhsExpr;
		rhsNot->var = rhsFactor;

		greaterThan->lhs = lhs;
		greaterThan->rhs = rhsNot;

		realExpr->var = greaterThan;
		boolEqual->var = realExpr;
		boolAnd->var = boolEqual;
		boolOr->var = boolAnd;
		expr->expr = boolOr;

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
		val_ident->identifier = { IDENTIFIER, "result" };
		value->var = val_ident;
		print->value = value;
		print_stmt->var = print;
		m_prog.stmts.push_back(print_stmt);
	}

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "1");  
}

TEST_F(EvaluatorTest, TestEvaluatorPrintLessThan) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "result" };

		node::NodeBooleanExpr* expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* boolOr = new node::NodeBooleanOr();
		node::NodeBooleanAnd* boolAnd = new node::NodeBooleanAnd();
		node::NodeBooleanEqual* boolEqual = new node::NodeBooleanEqual();
		node::NodeBooleanRealExpression* realExpr = new node::NodeBooleanRealExpression();

		
		node::NodeBooleanLess* lessThan = new node::NodeBooleanLess();

		node::NodeBooleanRealExpression* lhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* lhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* lhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* lhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* lhsTerm = new node::NodeTerm();
		node::NodeFactor* lhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* lhsDecimal = new node::NodeFactorDecimal();
		lhsDecimal->decimal = { DECIMAL, "3" };
		lhsTermFactor->var = lhsDecimal;
		lhsTerm->var = lhsTermFactor;
		lhsExpr->var = lhsTerm;
		lhsFactor->var = lhsExpr;
		lhsNot->var = lhsFactor;
		lhs->var = lhsNot;

		
		node::NodeBooleanNot* rhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* rhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* rhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* rhsTerm = new node::NodeTerm();
		node::NodeFactor* rhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* rhsDecimal = new node::NodeFactorDecimal();
		rhsDecimal->decimal = { DECIMAL, "4" };
		rhsTermFactor->var = rhsDecimal;
		rhsTerm->var = rhsTermFactor;
		rhsExpr->var = rhsTerm;
		rhsFactor->var = rhsExpr;
		rhsNot->var = rhsFactor;

	
		lessThan->lhs = lhs;
		lessThan->rhs = rhsNot;

		
		realExpr->var = lessThan;
		boolEqual->var = realExpr;
		boolAnd->var = boolEqual;
		boolOr->var = boolAnd;
		expr->expr = boolOr;

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
		val_ident->identifier = { IDENTIFIER, "result" };
		value->var = val_ident;
		print->value = value;
		print_stmt->var = print;
		m_prog.stmts.push_back(print_stmt);
	}

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "1");  
}



TEST_F(EvaluatorTest, TestEvaluatorPrintEqual) {
	// Arrange
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "result" };

		node::NodeBooleanExpr* expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* boolOr = new node::NodeBooleanOr();
		node::NodeBooleanAnd* boolAnd = new node::NodeBooleanAnd();
		node::NodeBooleanEqual* boolEqual = new node::NodeBooleanEqual();
		node::NodeBooleanEqualIs* equalIs = new node::NodeBooleanEqualIs();


		node::NodeBooleanRealExpression* lhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* lhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* lhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* lhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* lhsTerm = new node::NodeTerm();
		node::NodeFactor* lhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* lhsDecimal = new node::NodeFactorDecimal();
		lhsDecimal->decimal = { DECIMAL, "3" };
		lhsTermFactor->var = lhsDecimal;
		lhsTerm->var = lhsTermFactor;
		lhsExpr->var = lhsTerm;
		lhsFactor->var = lhsExpr;
		lhsNot->var = lhsFactor;
		lhs->var = lhsNot;

		node::NodeBooleanRealExpression* rhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* rhsNot = new node::NodeBooleanNot();
		node::NodeBooleanFactor* rhsFactor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* rhsExpr = new node::NodeArithmeticExpr();
		node::NodeTerm* rhsTerm = new node::NodeTerm();
		node::NodeFactor* rhsTermFactor = new node::NodeFactor();
		node::NodeFactorDecimal* rhsDecimal = new node::NodeFactorDecimal();
		rhsDecimal->decimal = { DECIMAL, "3" };
		rhsTermFactor->var = rhsDecimal;
		rhsTerm->var = rhsTermFactor;
		rhsExpr->var = rhsTerm;
		rhsFactor->var = rhsExpr;
		rhsNot->var = rhsFactor;
		rhs->var = rhsNot;

		equalIs->lhs = new node::NodeBooleanEqual;
		equalIs->lhs->var = lhs;
		

		equalIs->rhs = rhs;
		boolEqual->var = equalIs;
		boolAnd->var = boolEqual;
		boolOr->var = boolAnd;
		expr->expr = boolOr;

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
		val_ident->identifier = { IDENTIFIER, "result" };
		value->var = val_ident;
		print->value = value;
		print_stmt->var = print;
		m_prog.stmts.push_back(print_stmt);
	}

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "1");
}


TEST_F(EvaluatorTest, TestEvaluatorPrintFunctionReturn) {
	// Arrange

	node::NodeStmt* func_def_stmt = new node::NodeStmt();
	node::NodeDefinition* func_def = new node::NodeDefinition();
	func_def_stmt->var = func_def;
	node::NodeFunctionDefinition* funcDef = new node::NodeFunctionDefinition();
	func_def->var = funcDef;
	Token* returnType = new Token{ NUMBER };
	funcDef->type = returnType;
	funcDef->functionName = { IDENTIFIER, "getNumber" };

	node::NodeFunctionStmt* ret_stmt = new node::NodeFunctionStmt();
	node::NodeFunctionReturn* ret = new node::NodeFunctionReturn();
	node::NodeValue* ret_val = new node::NodeValue();
	node::NodeValueArithmeticExpression* ret_val_expr = new node::NodeValueArithmeticExpression();
	node::NodeArithmeticExpr* arith_expr = new node::NodeArithmeticExpr();
	node::NodeTerm* term = new node::NodeTerm();
	node::NodeFactor* factor = new node::NodeFactor();
	node::NodeFactorDecimal* decimal = new node::NodeFactorDecimal();
	decimal->decimal = { DECIMAL, "123" };
	factor->var = decimal;
	term->var = factor;
	arith_expr->var = term;
	ret_val_expr->expr = arith_expr;
	ret_val->var = ret_val_expr;
	ret->val = ret_val;
	ret_stmt->var = ret;
	funcDef->stmts.push_back(ret_stmt);
	m_prog.stmts.push_back(func_def_stmt);

	node::NodeStmt* print_stmt = new node::NodeStmt();
	node::NodeStmtPrint* print = new node::NodeStmtPrint();
	node::NodeValue* value = new node::NodeValue();
	node::NodeValueFunctionCall* val_func_call = new node::NodeValueFunctionCall();
	node::NodeFunctionCall* func_call = new node::NodeFunctionCall();
	func_call->functionName = { IDENTIFIER, "getNumber" };
	val_func_call->functionCall = func_call;
	value->var = val_func_call;
	print->value = value;
	print_stmt->var = print;
	m_prog.stmts.push_back(print_stmt);

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "123");
}

TEST_F(EvaluatorTest, TestEvaluatorPrintFunctionReturnNumber) {
	// Arrange

	node::NodeStmt* func_def_stmt = new node::NodeStmt();
	node::NodeDefinition* func_def = new node::NodeDefinition();
	func_def_stmt->var = func_def;
	node::NodeFunctionDefinition* funcDef = new node::NodeFunctionDefinition();
	func_def->var = funcDef;
	Token* returnType = new Token{ NUMBER };
	funcDef->type = returnType;
	funcDef->functionName = { IDENTIFIER, "getNumber" };

	node::NodeFunctionStmt* ret_stmt = new node::NodeFunctionStmt();
	node::NodeFunctionReturn* ret = new node::NodeFunctionReturn();
	node::NodeValue* ret_val = new node::NodeValue();
	node::NodeValueArithmeticExpression* ret_val_expr = new node::NodeValueArithmeticExpression();
	node::NodeArithmeticExpr* arith_expr = new node::NodeArithmeticExpr();
	node::NodeTerm* term = new node::NodeTerm();
	node::NodeFactor* factor = new node::NodeFactor();
	node::NodeFactorDecimal* decimal = new node::NodeFactorDecimal();
	decimal->decimal = { DECIMAL, "22" };
	factor->var = decimal;
	term->var = factor;
	arith_expr->var = term;
	ret_val_expr->expr = arith_expr;
	ret_val->var = ret_val_expr;
	ret->val = ret_val;
	ret_stmt->var = ret;
	funcDef->stmts.push_back(ret_stmt);
	m_prog.stmts.push_back(func_def_stmt);

	node::NodeStmt* print_stmt = new node::NodeStmt();
	node::NodeStmtPrint* print = new node::NodeStmtPrint();
	node::NodeValue* value = new node::NodeValue();
	node::NodeValueFunctionCall* val_func_call = new node::NodeValueFunctionCall();
	node::NodeFunctionCall* func_call = new node::NodeFunctionCall();
	func_call->functionName = { IDENTIFIER, "getNumber" };
	val_func_call->functionCall = func_call;
	value->var = val_func_call;
	print->value = value;
	print_stmt->var = print;
	m_prog.stmts.push_back(print_stmt);

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "22");
}
TEST_F(EvaluatorTest, TestEvaluatorPrintFunctionReturnString) {
	// Arrange

	node::NodeStmt* func_def_stmt = new node::NodeStmt();
	node::NodeDefinition* func_def = new node::NodeDefinition();
	func_def_stmt->var = func_def;
	node::NodeFunctionDefinition* funcDef = new node::NodeFunctionDefinition();
	func_def->var = funcDef;
	Token* returnType = new Token{ STRING };
	funcDef->type = returnType;
	funcDef->functionName = { IDENTIFIER, "getString" };

	node::NodeFunctionStmt* ret_stmt = new node::NodeFunctionStmt();
	node::NodeFunctionReturn* ret = new node::NodeFunctionReturn();
	node::NodeValue* ret_val = new node::NodeValue();
	node::NodeValueStringExpression* ret_val_expr = new node::NodeValueStringExpression();
	node::NodeStringExpr* str_expr = new node::NodeStringExpr();
	node::NodeStringValue* str_val = new node::NodeStringValue();
	str_val->value = { STRING_VAL, "Hello" };
	str_expr->var = str_val;
	ret_val_expr->expr = str_expr;
	ret_val->var = ret_val_expr;
	ret->val = ret_val;
	ret_stmt->var = ret;
	funcDef->stmts.push_back(ret_stmt);
	m_prog.stmts.push_back(func_def_stmt);

	node::NodeStmt* print_stmt = new node::NodeStmt();
	node::NodeStmtPrint* print = new node::NodeStmtPrint();
	node::NodeValue* value = new node::NodeValue();
	node::NodeValueFunctionCall* val_func_call = new node::NodeValueFunctionCall();
	node::NodeFunctionCall* func_call = new node::NodeFunctionCall();
	func_call->functionName = { IDENTIFIER, "getString" };
	val_func_call->functionCall = func_call;
	value->var = val_func_call;
	print->value = value;
	print_stmt->var = print;
	m_prog.stmts.push_back(print_stmt);

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "Hello");
}

TEST_F(EvaluatorTest, TestEvaluatorPrintFunctionReturnBoolean) {
	// Arrange

	node::NodeStmt* func_def_stmt = new node::NodeStmt();
	node::NodeDefinition* func_def = new node::NodeDefinition();
	func_def_stmt->var = func_def;
	node::NodeFunctionDefinition* funcDef = new node::NodeFunctionDefinition();
	func_def->var = funcDef;
	Token* returnType = new Token{ BOOLEAN };
	funcDef->type = returnType;
	funcDef->functionName = { IDENTIFIER, "getBoolean" };

	node::NodeFunctionStmt* ret_stmt = new node::NodeFunctionStmt();
	node::NodeFunctionReturn* ret = new node::NodeFunctionReturn();
	node::NodeValue* ret_val = new node::NodeValue();

	node::NodeValueBooleanExpression* ret_val_expr = new node::NodeValueBooleanExpression();
	node::NodeBooleanExpr* bool_expr = new node::NodeBooleanExpr();
	node::NodeBooleanOr* bool_or = new node::NodeBooleanOr();
	node::NodeBooleanAnd* bool_and = new node::NodeBooleanAnd();
	node::NodeBooleanEqual* bool_equal = new node::NodeBooleanEqual();
	node::NodeBooleanRealExpression* real_expr = new node::NodeBooleanRealExpression();
	node::NodeBooleanNot* not_expr = new node::NodeBooleanNot();
	node::NodeBooleanFactor* bool_factor = new node::NodeBooleanFactor();
	node::NodeBooleanFactorValue* bool_val = new node::NodeBooleanFactorValue();
	bool_val->boolval = { BOOLVAL, "true" };
	bool_factor->var = bool_val;
	not_expr->var = bool_factor;
	real_expr->var = not_expr;
	bool_equal->var = real_expr;
	bool_and->var = bool_equal;
	bool_or->var = bool_and;
	bool_expr->expr = bool_or;
	ret_val_expr->expr = bool_expr;
	ret_val->var = ret_val_expr;
	ret->val = ret_val;
	ret_stmt->var = ret;
	funcDef->stmts.push_back(ret_stmt);
	m_prog.stmts.push_back(func_def_stmt);

	node::NodeStmt* print_stmt = new node::NodeStmt();
	node::NodeStmtPrint* print = new node::NodeStmtPrint();
	node::NodeValue* value = new node::NodeValue();
	node::NodeValueFunctionCall* val_func_call = new node::NodeValueFunctionCall();
	node::NodeFunctionCall* func_call = new node::NodeFunctionCall();
	func_call->functionName = { IDENTIFIER, "getBoolean" };
	val_func_call->functionCall = func_call;
	value->var = val_func_call;
	print->value = value;
	print_stmt->var = print;
	m_prog.stmts.push_back(print_stmt);

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "1");
}




TEST_F(EvaluatorTest, TestEvaluatorAssignmentAndEquality) {
	// Arrange

	
	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "x" };
		node::NodeArithmeticExpr* expr = new node::NodeArithmeticExpr();
		node::NodeTerm* term = new node::NodeTerm();
		node::NodeFactor* factor = new node::NodeFactor();
		node::NodeFactorDecimal* val = new node::NodeFactorDecimal();
		val->decimal = { DECIMAL, "10" };
		factor->var = val;
		term->var = factor;
		expr->var = term;
		simp_decl->expr = expr;
		decl->var = simp_decl;
		decl_stmt->var = decl;
		m_prog.stmts.push_back(decl_stmt);
	}

	{
		node::NodeStmt* decl_stmt = new node::NodeStmt();
		node::NodeDecl* decl = new node::NodeDecl();
		node::NodeSimpleDecl* simp_decl = new node::NodeSimpleDecl();
		simp_decl->identifier = { IDENTIFIER, "y" };
		node::NodeArithmeticExpr* expr = new node::NodeArithmeticExpr();
		node::NodeTerm* term = new node::NodeTerm();
		node::NodeFactor* factor = new node::NodeFactor();
		node::NodeFactorDecimal* val = new node::NodeFactorDecimal();
		val->decimal = { DECIMAL, "20" };
		factor->var = val;
		term->var = factor;
		expr->var = term;
		simp_decl->expr = expr;
		decl->var = simp_decl;
		decl_stmt->var = decl;
		m_prog.stmts.push_back(decl_stmt);
	}
	
	{
		node::NodeStmt* assign_stmt = new node::NodeStmt();
		node::NodeAssignment* assign = new node::NodeAssignment();
		assign->identifierHead = { IDENTIFIER, "x" };
		node::NodeValue* rhs = new node::NodeValue();
		node::NodeValueIdentifier* rhs_ident = new node::NodeValueIdentifier();
		rhs_ident->identifier = { IDENTIFIER, "y" };
		rhs->var = rhs_ident;
		assign->rhs = rhs;
		assign_stmt->var = assign;
		m_prog.stmts.push_back(assign_stmt);
	}
	
	{
		node::NodeStmt* print_stmt = new node::NodeStmt();
		node::NodeStmtPrint* print = new node::NodeStmtPrint();
		node::NodeValue* value = new node::NodeValue();
		node::NodeValueBooleanExpression* bool_expr_val = new node::NodeValueBooleanExpression();
		node::NodeBooleanExpr* bool_expr = new node::NodeBooleanExpr();
		node::NodeBooleanOr* bool_or = new node::NodeBooleanOr();
		node::NodeBooleanAnd* bool_and = new node::NodeBooleanAnd();
		node::NodeBooleanEqual* bool_equal = new node::NodeBooleanEqual();
		node::NodeBooleanEqualIs* equal_is = new node::NodeBooleanEqualIs();

	
		node::NodeBooleanRealExpression* lhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* lhs_not = new node::NodeBooleanNot();
		node::NodeBooleanFactor* lhs_factor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* lhs_expr = new node::NodeArithmeticExpr();
		node::NodeTerm* lhs_term = new node::NodeTerm();
		node::NodeFactor* lhs_term_factor = new node::NodeFactor();
		node::NodeFactorIdentifier* lhs_ident = new node::NodeFactorIdentifier();
		lhs_ident->identifier = { IDENTIFIER, "x" };
		lhs_term_factor->var = lhs_ident;
		lhs_term->var = lhs_term_factor;
		lhs_expr->var = lhs_term;
		lhs_factor->var = lhs_expr;
		lhs_not->var = lhs_factor;
		lhs->var = lhs_not;

		
		node::NodeBooleanRealExpression* rhs = new node::NodeBooleanRealExpression();
		node::NodeBooleanNot* rhs_not = new node::NodeBooleanNot();
		node::NodeBooleanFactor* rhs_factor = new node::NodeBooleanFactor();
		node::NodeArithmeticExpr* rhs_expr = new node::NodeArithmeticExpr();
		node::NodeTerm* rhs_term = new node::NodeTerm();
		node::NodeFactor* rhs_term_factor = new node::NodeFactor();
		node::NodeFactorIdentifier* rhs_ident = new node::NodeFactorIdentifier();
		rhs_ident->identifier = { IDENTIFIER, "y" };
		rhs_term_factor->var = rhs_ident;
		rhs_term->var = rhs_term_factor;
		rhs_expr->var = rhs_term;
		rhs_factor->var = rhs_expr;
		rhs_not->var = rhs_factor;
		rhs->var = rhs_not;

		node::NodeBooleanEqual* lhs_equal = new node::NodeBooleanEqual();
		lhs_equal->var = lhs;
		equal_is->lhs = lhs_equal;
		equal_is->rhs = rhs;
		bool_equal->var = equal_is;
		bool_and->var = bool_equal;
		bool_or->var = bool_and;
		bool_expr->expr = bool_or;
		bool_expr_val->expr = bool_expr;
		value->var = bool_expr_val;
		print->value = value;
		print_stmt->var = print;
		m_prog.stmts.push_back(print_stmt);
	}

	// Act
	evaluate_prog();

	// Assert
	ASSERT_EQ(output, "1");
}
