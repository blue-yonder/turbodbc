#include "mock_classes.h"

namespace turbodbc_test {

default_mock_connection::default_mock_connection() = default;
default_mock_connection::~default_mock_connection() = default;

default_mock_statement::default_mock_statement() = default;
default_mock_statement::~default_mock_statement() = default;

}

std::ostream& operator<<(std::ostream& os, const std::tm& mytm)
{
	os << std::asctime(&mytm);
	return os;
}

bool operator==(const std::tm LHS, const std::tm RHS)
{
	return LHS.tm_year == RHS.tm_year && LHS.tm_mon == RHS.tm_mon && LHS.tm_mday == RHS.tm_mday;
}
