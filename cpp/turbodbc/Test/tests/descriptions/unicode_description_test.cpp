#include "turbodbc/descriptions/unicode_description.h"

#include <gtest/gtest.h>
#include <sqlext.h>


TEST(UnicodeDescriptionTest, BasicProperties)
{
	std::size_t const size = 42;
	turbodbc::unicode_description const description(size);

	EXPECT_EQ(86, description.element_size());
	EXPECT_EQ(SQL_C_WCHAR, description.column_c_type());
	EXPECT_EQ(SQL_WVARCHAR, description.column_sql_type());
	EXPECT_EQ(0, description.digits());
}


TEST(UnicodeDescriptionTest, GetTypeCode)
{
	turbodbc::unicode_description const description(10);
	EXPECT_EQ(turbodbc::type_code::unicode, description.get_type_code());
}

TEST(UnicodeDescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;

	turbodbc::unicode_description const description(expected_name, expected_supports_null, 10);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
