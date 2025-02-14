#include "cpp_odbc/error.h"

#include <gtest/gtest.h>

#include "cpp_odbc/level2/diagnostic_record.h"

TEST(ErrorTest, MessageConstruction)
{
	std::string const expected_message = "test message";
	cpp_odbc::error exception(expected_message);
	EXPECT_EQ(expected_message, exception.what());
}

TEST(ErrorTest, DiagnosticConstruction)
{
	cpp_odbc::level2::diagnostic_record const record = {"ABCDE", 42, "test message"};

	std::string const expected_message = "ODBC error\nstate: ABCDE\nnative error code: 42\nmessage: test message";

	cpp_odbc::error exception(record);
	EXPECT_EQ(expected_message, exception.what());
}
