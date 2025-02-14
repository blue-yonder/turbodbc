#include "cpp_odbc/level2/diagnostic_record.h"

#include <gtest/gtest.h>

TEST(DiagnosticRecordTest, Members)
{
	std::string const odbc_state = "ABCDE";
	int const native_state = -1;
	std::string const message = "Everything is bad.";

	cpp_odbc::level2::diagnostic_record record = {odbc_state, native_state, message};

	EXPECT_EQ( odbc_state, record.odbc_status_code );
	EXPECT_EQ( native_state, record.native_error_code );
	EXPECT_EQ( message, record.message );
}
