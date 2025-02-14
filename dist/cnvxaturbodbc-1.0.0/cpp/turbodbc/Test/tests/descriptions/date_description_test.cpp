#include "turbodbc/descriptions/date_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>

TEST(DateDescriptionTest, BasicProperties)
{
	turbodbc::date_description const description;

	EXPECT_EQ(6, description.element_size());
	EXPECT_EQ(SQL_C_TYPE_DATE, description.column_c_type());
	EXPECT_EQ(SQL_TYPE_DATE, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}

TEST(DateDescriptionTest, GetTypeCode)
{
	turbodbc::date_description const description;
	EXPECT_EQ( turbodbc::type_code::date, description.get_type_code() );
}

TEST(DateDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::date_description const description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
