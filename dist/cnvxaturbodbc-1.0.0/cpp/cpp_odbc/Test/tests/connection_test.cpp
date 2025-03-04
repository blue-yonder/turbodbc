#include "cpp_odbc/connection.h"

#include <gtest/gtest.h>

#include "cpp_odbc_test/mock_statement.h"
#include "cpp_odbc_test/mock_connection.h"


using cpp_odbc_test::mock_connection;


TEST(ConnectionTest, IsSharable)
{
	auto connection = std::make_shared<mock_connection>();
	auto shared = connection->shared_from_this();
	EXPECT_TRUE(connection == shared);
}

TEST(ConnectionTest, MakeStatementForwards)
{
	mock_connection connection;
	auto statement = std::make_shared<cpp_odbc_test::mock_statement>();

	EXPECT_CALL(connection, do_make_statement())
		.WillOnce(testing::Return(statement));

	EXPECT_TRUE(statement == connection.make_statement());
}

TEST(ConnectionTest, SetIntegerAttributeForwards)
{
	mock_connection connection;
	SQLINTEGER const attribute = 42;
	intptr_t const value = 17;

	EXPECT_CALL(connection, do_set_attribute(attribute, value)).Times(1);

	connection.set_attribute(attribute, value);
}

TEST(ConnectionTest, CommitForwards)
{
	mock_connection connection;

	EXPECT_CALL(connection, do_commit()).Times(1);

	connection.commit();
}

TEST(ConnectionTest, RollbackForwards)
{
	mock_connection connection;

	EXPECT_CALL(connection, do_rollback()).Times(1);

	connection.rollback();
}

TEST(ConnectionTest, GetStringInfoForwards)
{
	mock_connection connection;
	SQLUSMALLINT const info_type = 42;
	std::string const expected = "test info";

	EXPECT_CALL(connection, do_get_string_info(info_type))
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, connection.get_string_info(info_type));
}

TEST(ConnectionTest, GetIntegerInfoForwards)
{
	mock_connection connection;
	SQLUSMALLINT const info_type = 42;
	SQLUINTEGER const expected = 23;

	EXPECT_CALL(connection, do_get_integer_info(info_type))
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, connection.get_integer_info(info_type));
}

TEST(ConnectionTest, SupportsFunctionForwards)
{
	mock_connection connection;
	SQLUSMALLINT const function_id = 5;

	EXPECT_CALL(connection, do_supports_function(function_id))
		.WillOnce(testing::Return(true));

	EXPECT_TRUE(connection.supports_function(function_id));
}