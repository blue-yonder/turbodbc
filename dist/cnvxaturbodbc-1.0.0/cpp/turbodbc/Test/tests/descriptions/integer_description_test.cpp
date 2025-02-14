#include "turbodbc/descriptions/integer_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>


TEST(IntegerDescriptionTest, BasicProperties)
{
	turbodbc::integer_description const description;

	EXPECT_EQ(sizeof(int64_t), description.element_size());
	EXPECT_EQ(SQL_C_SBIGINT, description.column_c_type());
	EXPECT_EQ(SQL_BIGINT, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}

TEST(IntegerDescriptionTest, GetTypeCode)
{
	turbodbc::integer_description const description;
	EXPECT_EQ(turbodbc::type_code::integer, description.get_type_code());
}

TEST(IntegerDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::integer_description const description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
