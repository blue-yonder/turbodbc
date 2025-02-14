#include "cpp_odbc/column_description.h"

#include <gtest/gtest.h>

#include <sstream>
#include <ciso646>
#include <sqlext.h>

using cpp_odbc::column_description;

TEST(ColumnDescriptionTest, Members)
{
	std::string const expected_name("dummy");
	SQLSMALLINT const expected_data_type = 42;
	SQLULEN const expected_size = 17;
	SQLSMALLINT const expected_decimal_digits = 3;
	bool const expected_allows_nullable = false;

	column_description const description = {
		expected_name,
		expected_data_type,
		expected_size,
		expected_decimal_digits,
		expected_allows_nullable
	};

	EXPECT_EQ(expected_name, description.name);
	EXPECT_EQ(expected_data_type, description.data_type);
	EXPECT_EQ(expected_size, description.size);
	EXPECT_EQ(expected_decimal_digits, description.decimal_digits);
	EXPECT_EQ(expected_allows_nullable, description.allows_null_values);
}

TEST(ColumnDescriptionTest, Equality)
{
	column_description const original = {"dummy", 1, 2, 3, false};
	EXPECT_TRUE( original == original );

	column_description different_name(original);
	different_name.name = "other";
	EXPECT_FALSE(original == different_name);

	column_description different_type(original);
	different_type.data_type += 1;
	EXPECT_FALSE(original == different_type);

	column_description different_size(original);
	different_size.size += 1;
	EXPECT_FALSE(original == different_size);

	column_description different_digits(original);
	different_digits.decimal_digits += 1;
	EXPECT_FALSE(original == different_digits);

	column_description different_null(original);
	different_null.allows_null_values = not original.allows_null_values;
	EXPECT_FALSE(original == different_null);
}

namespace {
	void test_output(std::string const & expected, column_description const & description)
	{
		std::ostringstream actual;
		actual << description;
		EXPECT_EQ(expected, actual.str());
	}
}

TEST(ColumnDescriptionTest, OutputKnownType)
{
	test_output(
			"test_name @ SQL_INTEGER (precision 2, scale 3)",
			{"test_name", SQL_INTEGER, 2, 3, false}
		);

	test_output(
			"test_name @ NULLABLE SQL_INTEGER (precision 2, scale 3)",
			{"test_name", SQL_INTEGER, 2, 3, true}
		);
}

TEST(ColumnDescriptionTest, OutputUnknownType)
{
	test_output(
			"test_name @ UNKNOWN TYPE (precision 2, scale 3)",
			{"test_name", 666, 2, 3, false}
		);
}
