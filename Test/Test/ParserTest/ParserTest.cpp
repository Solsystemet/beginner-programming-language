#include "../../pch.h"

class parserMock : public Parser {
public:
	MOCK_METHOD(node::NodeTerm*, parse_term, (), (override));
};
