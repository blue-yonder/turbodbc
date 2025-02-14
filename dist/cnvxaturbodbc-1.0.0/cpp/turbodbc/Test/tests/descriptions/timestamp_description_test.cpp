#include "turbodbc/descriptions/timestamp_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>


TEST(TimestampDescriptionTest, BasicProperties)
{
	turbodbc::timestamp_description const description;

	EXPECT_EQ(16, description.element_size());
	EXPECT_EQ(SQL_C_TYPE_TIMESTAMP, description.column_c_type());
	EXPECT_EQ(SQL_TYPE_TIMESTAMP, description.column_sql_type());
	EXPECT_EQ(6, description.digits());
}

TEST(TimestampDescriptionTest, GetTypeCode)
{
	turbodbc::timestamp_description const description;
	EXPECT_EQ(turbodbc::type_code::timestamp, description.get_type_code());
}

TEST(TimestampDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::timestamp_description const description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
