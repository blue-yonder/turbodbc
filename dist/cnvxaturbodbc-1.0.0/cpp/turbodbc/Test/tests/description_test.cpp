#include "turbodbc/description.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace {

	struct mock_description : public turbodbc::description {
		mock_description() : turbodbc::description() {}
		mock_description(std::string name, bool supports_null_values) :
			turbodbc::description(std::move(name), supports_null_values)
		{
		}

		MOCK_CONST_METHOD0(do_element_size, std::size_t());
		MOCK_CONST_METHOD0(do_column_c_type, SQLSMALLINT());
		MOCK_CONST_METHOD0(do_column_sql_type, SQLSMALLINT());
		MOCK_CONST_METHOD0(do_digits, SQLSMALLINT());
		MOCK_CONST_METHOD2(do_set_field, void(cpp_odbc::writable_buffer_element &, turbodbc::field const &));
		MOCK_CONST_METHOD0(do_get_type_code, turbodbc::type_code());
	};

}


TEST(DescriptionTest, ElementSizeForwards)
{
	std::size_t const expected = 42;

	mock_description description;
	EXPECT_CALL(description, do_element_size())
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, description.element_size());
}

TEST(DescriptionTest, ColumnTypeForwards)
{
	SQLSMALLINT const expected = 42;

	mock_description description;
	EXPECT_CALL(description, do_column_c_type())
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, description.column_c_type());
}

TEST(DescriptionTest, ColumnSqlTypeForwards)
{
	SQLSMALLINT const expected = 42;

	mock_description description;
	EXPECT_CALL(description, do_column_sql_type())
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, description.column_sql_type());
}

TEST(DescriptionTest, DigitsForwards)
{
	SQLSMALLINT const expected = 42;

	mock_description description;
	EXPECT_CALL(description, do_digits())
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, description.digits());
}

TEST(DescriptionTest, TypeCodeForwards)
{
	auto const expected = turbodbc::type_code::string;
	mock_description description;
	EXPECT_CALL(description, do_get_type_code())
		.WillOnce(testing::Return(expected));

	EXPECT_EQ(expected, description.get_type_code());
}

TEST(DescriptionTest, DefaultName)
{
	mock_description description;

	EXPECT_EQ("parameter", description.name());
}

TEST(DescriptionTest, DefaultSupportsNullValues)
{
	mock_description description;

	EXPECT_TRUE(description.supports_null_values());
}

TEST(DescriptionTest, CustomNameAndNullableSupport)
{
	std::string const expected_name("my_name");
	bool const expected_supports_null = false;
	mock_description description(expected_name, expected_supports_null);

	EXPECT_EQ(expected_name, description.name());
	EXPECT_EQ(expected_supports_null, description.supports_null_values());
}
