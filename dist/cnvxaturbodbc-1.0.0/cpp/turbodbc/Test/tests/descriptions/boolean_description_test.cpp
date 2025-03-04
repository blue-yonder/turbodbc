#include "turbodbc/descriptions/boolean_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>

TEST(BooleanDescriptionTest, BasicProperties)
{
	turbodbc::boolean_description const description;
	EXPECT_EQ(1, description.element_size());
	EXPECT_EQ(SQL_C_BIT, description.column_c_type());
	EXPECT_EQ(SQL_BIT, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}


TEST(BooleanDescriptionTest, GetTypeCode)
{
	turbodbc::boolean_description const description;
	EXPECT_EQ( turbodbc::type_code::boolean, description.get_type_code() );
}

TEST(BooleanDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::boolean_description const description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
