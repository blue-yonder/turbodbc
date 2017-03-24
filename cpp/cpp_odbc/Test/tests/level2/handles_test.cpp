#include "cpp_odbc/level2/handles.h"

#include <gtest/gtest.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include "sql.h"

namespace {
	//destinations for pointers
	int const dummy_value = 23;
	int value_a = dummy_value;
	int value_b = dummy_value;

	template <typename Handle>
	void test_handle(signed short int expected_type)
	{
		Handle const handle = {&value_a};
		EXPECT_EQ(&value_a, handle.handle);
		EXPECT_EQ( expected_type, handle.type() );
	}

	template <typename Handle>
	void test_handle_equality()
	{
		Handle const handle_a = {&value_a};
		Handle const handle_b = {&value_b};

		EXPECT_TRUE( handle_a == handle_a );
		EXPECT_TRUE( handle_a != handle_b );
		EXPECT_FALSE( handle_a == handle_b );
		EXPECT_FALSE( handle_a != handle_a );
	}

}

TEST(HandlesTest, ConnectionHandle)
{
	test_handle<cpp_odbc::level2::connection_handle>(SQL_HANDLE_DBC);
}

TEST(HandlesTest, ConnectionHandleEquality)
{
	test_handle_equality<cpp_odbc::level2::connection_handle>();
}

TEST(HandlesTest, EnvironmentHandle)
{
	test_handle<cpp_odbc::level2::environment_handle>(SQL_HANDLE_ENV);
}

TEST(HandlesTest, EnvironmentHandleEquality)
{
	test_handle_equality<cpp_odbc::level2::environment_handle>();
}

TEST(HandlesTest, StatementHandle)
{
	test_handle<cpp_odbc::level2::statement_handle>(SQL_HANDLE_STMT);
}

TEST(HandlesTest, StatementHandleEquality)
{
	test_handle_equality<cpp_odbc::level2::statement_handle>();
}
