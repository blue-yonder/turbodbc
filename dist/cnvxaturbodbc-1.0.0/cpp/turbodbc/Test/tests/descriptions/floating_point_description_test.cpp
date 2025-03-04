#include "turbodbc/descriptions/floating_point_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>


TEST(FloatingPointDescriptionTest, BasicProperties)
{
	turbodbc::floating_point_description const description;

	EXPECT_EQ(sizeof(double), description.element_size());
	EXPECT_EQ(SQL_C_DOUBLE, description.column_c_type());
	EXPECT_EQ(SQL_DOUBLE, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}

TEST(FloatingPointDescriptionTest, GetTypeCode)
{
	turbodbc::floating_point_description const description;
	EXPECT_EQ(turbodbc::type_code::floating_point, description.get_type_code());
}

TEST(FloatingPointDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::floating_point_description const description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
