#include <turbodbc/make_description.h>

#include <gtest/gtest.h>

#include <turbodbc/descriptions.h>

#include <sqlext.h>
#include <sstream>
#include <stdexcept>


using turbodbc::make_description;

namespace {

	std::string const name("custom_name");
	bool const supports_null_values = false;
	bool const prefer_strings = false;
	bool const prefer_unicode = true;

	void assert_custom_name_and_nullable_support(turbodbc::description const & description)
	{
		EXPECT_EQ(name, description.name());
		EXPECT_EQ(supports_null_values, description.supports_null_values());
	}

	void test_as_integer(cpp_odbc::column_description const & column_description)
	{
		auto const description = make_description(column_description, prefer_strings);
		ASSERT_TRUE(dynamic_cast<turbodbc::integer_description const *>(description.get()))
			<< "Could not convert type identifier '" << column_description.data_type << "' to integer description";

		assert_custom_name_and_nullable_support(*description);
	}

	void test_as_floating_point(cpp_odbc::column_description const & column_description)
	{
		auto const description = make_description(column_description, prefer_strings);
		ASSERT_TRUE(dynamic_cast<turbodbc::floating_point_description const *>(description.get()))
			<< "Could not convert type identifier '" << column_description.data_type << "' to floating point description";

		assert_custom_name_and_nullable_support(*description);
	}

	void test_unsupported(cpp_odbc::column_description const & column_description)
	{
		ASSERT_THROW(make_description(column_description, prefer_strings), std::runtime_error);
	}

	template <typename Description>
	void test_text_with_string_preference(cpp_odbc::column_description const & column_description, std::size_t expected_size)
	{
		auto const description = make_description(column_description, prefer_strings);

		ASSERT_TRUE(dynamic_cast<Description const *>(description.get()))
			<< "Could not convert type identifier '" << column_description.data_type << "' to expected description";

		EXPECT_EQ(expected_size, description->element_size());
		assert_custom_name_and_nullable_support(*description);
	}

	template <typename Description>
	void test_text_with_unicode_preference(cpp_odbc::column_description const & column_description, std::size_t expected_size)
	{
		auto const description = make_description(column_description, prefer_unicode);

		ASSERT_TRUE(dynamic_cast<Description const *>(description.get()))
			<< "Could not convert type identifier '" << column_description.data_type << "' to expected description";

		EXPECT_EQ(expected_size, description->element_size());
		assert_custom_name_and_nullable_support(*description);
	}

}

TEST(MakeDescriptionOfDescriptionTest, UnsupportedTypeThrows)
{
	SQLSMALLINT const unsupported_type = SQL_GUID;
	cpp_odbc::column_description column_description = {name, unsupported_type, 0, 0, supports_null_values};
	test_unsupported(column_description);
}

TEST(MakeDescriptionOfDescriptionTest, IntegerTypes)
{
	std::vector<SQLSMALLINT> const types = {
			SQL_SMALLINT, SQL_INTEGER, SQL_TINYINT, SQL_BIGINT,
		};

	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
		test_as_integer(column_description);
	}
}

TEST(MakeDescriptionOfDescriptionTest, StringTypesWithStringPreferenceYieldsString)
{
	std::vector<SQLSMALLINT> const types = {
			SQL_CHAR, SQL_VARCHAR, SQL_LONGVARCHAR
		};

	std::size_t const size = 42;
	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, size, 0, supports_null_values};
		test_text_with_string_preference<turbodbc::string_description>(column_description, 43);
	}
}

TEST(MakeDescriptionOfDescriptionTest, StringTypesWithUnicodePreferenceYieldsUnicode)
{
	std::vector<SQLSMALLINT> const types = {
		SQL_CHAR, SQL_VARCHAR, SQL_LONGVARCHAR
	};

	std::size_t const size = 42;
	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, size, 0, supports_null_values};
		test_text_with_unicode_preference<turbodbc::unicode_description>(column_description, 86);
	}
}

TEST(MakeDescriptionOfDescriptionTest, UnicodeTypesWithStringPreferenceYieldsUnicode)
{
	std::vector<SQLSMALLINT> const types = {
		SQL_WCHAR, SQL_WVARCHAR, SQL_WLONGVARCHAR
	};

	std::size_t const size = 42;
	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, size, 0, supports_null_values};
		test_text_with_string_preference<turbodbc::unicode_description>(column_description, 86);
	}
}

TEST(MakeDescriptionOfDescriptionTest, UnicodeTypesWithUnicodePreferenceYieldsUnicode)
{
	std::vector<SQLSMALLINT> const types = {
		SQL_WCHAR, SQL_WVARCHAR, SQL_WLONGVARCHAR
	};

	std::size_t const size = 42;
	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, size, 0, supports_null_values};
		test_text_with_unicode_preference<turbodbc::unicode_description>(column_description, 86);
	}
}


TEST(MakeDescriptionOfDescriptionTest, FloatingPointTypes)
{
	std::vector<SQLSMALLINT> const types = {
			SQL_REAL, SQL_FLOAT, SQL_DOUBLE
		};

	for (auto const type : types) {
		cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
		test_as_floating_point(column_description);
	}
}

TEST(MakeDescriptionOfDescriptionTest, BitType)
{
	SQLSMALLINT const type = SQL_BIT;

	cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
	auto const description = make_description(column_description, prefer_strings);
	ASSERT_TRUE(dynamic_cast<turbodbc::boolean_description const *>(description.get()));
	assert_custom_name_and_nullable_support(*description);
}

TEST(MakeDescriptionOfDescriptionTest, DateType)
{
	SQLSMALLINT const type = SQL_TYPE_DATE;

	cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
	auto const description = make_description(column_description, prefer_strings);
	ASSERT_TRUE(dynamic_cast<turbodbc::date_description const *>(description.get()));
	assert_custom_name_and_nullable_support(*description);
}

TEST(MakeDescriptionOfDescriptionTest, TimestampTypes)
{
	SQLSMALLINT const type = SQL_TYPE_TIMESTAMP;

	cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
	auto const description = make_description(column_description, prefer_strings);
	ASSERT_TRUE(dynamic_cast<turbodbc::timestamp_description const *>(description.get()));
	assert_custom_name_and_nullable_support(*description);
}

namespace {

	cpp_odbc::column_description make_decimal_column_description(SQLULEN size, SQLSMALLINT precision)
	{
		return {name, SQL_DECIMAL, size, precision, supports_null_values};
	}

	cpp_odbc::column_description make_numeric_column_description(SQLULEN size, SQLSMALLINT precision)
	{
		return {name, SQL_NUMERIC, size, precision, supports_null_values};
	}

}

TEST(MakeDescriptionOfDescriptionTest, DecimalAsInteger)
{
	test_as_integer(make_decimal_column_description(18, 0));
	test_as_integer(make_decimal_column_description(9, 0));
	test_as_integer(make_decimal_column_description(1, 0));
	test_as_integer(make_numeric_column_description(18, 0));
	test_as_integer(make_numeric_column_description(9, 0));
	test_as_integer(make_numeric_column_description(1, 0));
}

TEST(MakeDescriptionOfDescriptionTest, DecimalAsFloatingPoint)
{
	test_as_floating_point(make_decimal_column_description(18, 1));
	test_as_floating_point(make_numeric_column_description(18, 1));
}

TEST(MakeDescriptionOfDescriptionTest, DecimalAsString)
{
	std::size_t const size = 19;
	// add three bytes to size (null-termination, sign, decimal point
	test_text_with_string_preference<turbodbc::string_description>(make_decimal_column_description(size, 0), size + 3);
	test_text_with_string_preference<turbodbc::string_description>(make_decimal_column_description(size, 5), size + 3);
	test_text_with_string_preference<turbodbc::string_description>(make_numeric_column_description(size, 0), size + 3);
	test_text_with_string_preference<turbodbc::string_description>(make_numeric_column_description(size, 5), size + 3);
}
