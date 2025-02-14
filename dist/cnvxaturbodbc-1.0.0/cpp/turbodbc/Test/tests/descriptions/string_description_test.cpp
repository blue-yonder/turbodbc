#include "turbodbc/descriptions/string_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>


TEST(StringDescriptionTest, BasicProperties)
{
	std::size_t const size = 42;
	turbodbc::string_description const description(size);

	EXPECT_EQ(size + 1, description.element_size());
	EXPECT_EQ(SQL_C_CHAR, description.column_c_type());
	EXPECT_EQ(SQL_VARCHAR, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}


TEST(StringDescriptionTest, GetTypeCode)
{
	turbodbc::string_description const description(10);
	EXPECT_EQ(turbodbc::type_code::string, description.get_type_code());
}

TEST(StringDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::string_description const description(expected_name, expected_supports_null, 10);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
