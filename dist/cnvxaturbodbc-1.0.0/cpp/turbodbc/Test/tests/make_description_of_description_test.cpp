#include <turbodbc/make_description.h>

#include <gtest/gtest.h>

#include <turbodbc/descriptions.h>

#include <sqlext.h>
#include <sstream>
#include <stdexcept>


using turbodbc::make_description;

namespace {

    turbodbc::options make_options(bool prefer_unicode, bool large_decimals_as_64_bit_types)
    {
        turbodbc::options options;
        options.prefer_unicode = prefer_unicode;
        options.large_decimals_as_64_bit_types = large_decimals_as_64_bit_types;
        return options;
    }

    bool const prefer_strings = false;
    bool const prefer_unicode = true;
    bool const large_decimals_as_strings = false;
    bool const large_decimals_as_64_bit_types = true;

    std::string const name("custom_name");
    bool const supports_null_values = false;

    void assert_custom_name_and_nullable_support(turbodbc::description const & description)
    {
        EXPECT_EQ(name, description.name());
        EXPECT_EQ(supports_null_values, description.supports_null_values());
    }

    void test_as_integer(cpp_odbc::column_description const & column_description, bool use_64_bit_types)
    {
        auto const description = make_description(column_description, make_options(prefer_strings, use_64_bit_types));
        ASSERT_TRUE(dynamic_cast<turbodbc::integer_description const *>(description.get()))
            << "Could not convert type identifier '" << column_description.data_type << "' to integer description";

        assert_custom_name_and_nullable_support(*description);
    }

    void test_as_integer(cpp_odbc::column_description const & column_description)
    {
        test_as_integer(column_description, large_decimals_as_strings);
    }

    void test_as_floating_point(cpp_odbc::column_description const & column_description, bool use_64_bit_types)
    {
        auto const description = make_description(column_description, make_options(prefer_strings, use_64_bit_types));
        ASSERT_TRUE(dynamic_cast<turbodbc::floating_point_description const *>(description.get()))
            << "Could not convert type identifier '" << column_description.data_type << "' to floating point description";

        assert_custom_name_and_nullable_support(*description);
    }

    void test_as_floating_point(cpp_odbc::column_description const & column_description)
    {
        test_as_floating_point(column_description, large_decimals_as_strings);
    }

    void test_unsupported(cpp_odbc::column_description const & column_description)
    {
        ASSERT_THROW(make_description(column_description, make_options(prefer_strings, large_decimals_as_strings)), std::runtime_error);
    }

    template <typename Description>
    void test_text_with_string_preference(cpp_odbc::column_description const & column_description, std::size_t expected_size)
    {
        auto const description = make_description(column_description, make_options(prefer_strings, large_decimals_as_strings));

        ASSERT_TRUE(dynamic_cast<Description const *>(description.get()))
            << "Could not convert type identifier '" << column_description.data_type << "' to expected description";

        EXPECT_EQ(expected_size, description->element_size());
        assert_custom_name_and_nullable_support(*description);
    }

    template <typename Description>
    void test_text_with_unicode_preference(cpp_odbc::column_description const & column_description, std::size_t expected_size)
    {
        auto const description = make_description(column_description, make_options(prefer_unicode, large_decimals_as_strings));

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


TEST(MakeDescriptionOfDescriptionTest, VarcharMaxRespectsConfiguredLimitForStringPreference)
{
    std::size_t const max_size = 0;
    cpp_odbc::column_description column_description = {name, SQL_VARCHAR, max_size, 0, supports_null_values};

    turbodbc::options options;
    options.prefer_unicode = false;
    options.varchar_max_character_limit = 10;

    auto const description = make_description(column_description, options);
    EXPECT_EQ(10 + 1, description->element_size());
}


TEST(MakeDescriptionOfDescriptionTest, VarcharMaxRespectsConfiguredLimitForUnicodePreference)
{
    std::size_t const max_size = 0;
    cpp_odbc::column_description column_description = {name, SQL_VARCHAR, max_size, 0, supports_null_values};

    turbodbc::options options;
    options.prefer_unicode = true;
    options.varchar_max_character_limit = 10;

    auto const description = make_description(column_description, options);
    EXPECT_EQ(2 * (10 + 1), description->element_size());
}


TEST(MakeDescriptionOfDescriptionTest, VarcharMaxRespectsConfiguredLimitForUnicode)
{
    std::size_t const max_size = 0;
    cpp_odbc::column_description column_description = {name, SQL_WVARCHAR, max_size, 0, supports_null_values};

    turbodbc::options options;
    options.varchar_max_character_limit = 10;

    auto const description = make_description(column_description, options);
    EXPECT_EQ(2 * (10 + 1), description->element_size());
}


TEST(MakeDescriptionOfDescriptionTest, VarcharFieldsCanBeLimited)
{
    std::size_t const reported_size = 20;
    std::size_t const limit = 10;
    cpp_odbc::column_description column_description = {name, SQL_VARCHAR, reported_size, 0, supports_null_values};

    turbodbc::options options;
    options.varchar_max_character_limit = limit;
    options.limit_varchar_results_to_max = false;

    auto const unlimited = make_description(column_description, options);
    EXPECT_EQ(reported_size + 1, unlimited->element_size());

    options.limit_varchar_results_to_max = true;
    auto const limited = make_description(column_description, options);
    EXPECT_EQ(limit + 1, limited->element_size());
}

TEST(MakeDescriptionOfDescriptionTest, VarcharFieldsModifiedMemoryAllocation)
{
    std::size_t const reported_size = 10;
    std::size_t const size_with_extra_capacity = 40;
    cpp_odbc::column_description column_description = {name, SQL_VARCHAR, reported_size, 0, supports_null_values};

    turbodbc::options options;

    options.force_extra_capacity_for_unicode = false;
    EXPECT_EQ(reported_size + 1, make_description(column_description, options)->element_size());

    options.force_extra_capacity_for_unicode = true;
    EXPECT_EQ(size_with_extra_capacity + 1, make_description(column_description, options)->element_size());
}

TEST(MakeDescriptionOfDescriptionTest, VarcharFieldsModifiedMemoryAllocationForUnicode)
{
    std::size_t const reported_size = 10;
    std::size_t const size_without_extra_capacity = (reported_size + 1) * sizeof(char16_t);
    std::size_t const size_with_extra_capacity = (reported_size*2 + 1) * sizeof(char16_t);
    cpp_odbc::column_description column_description = {name, SQL_WVARCHAR, reported_size, 0, supports_null_values};

    turbodbc::options options;

    options.force_extra_capacity_for_unicode = false;
    EXPECT_EQ(size_without_extra_capacity, make_description(column_description, options)->element_size());

    options.force_extra_capacity_for_unicode = true;
    EXPECT_EQ(size_with_extra_capacity, make_description(column_description, options)->element_size());
}

TEST(MakeDescriptionOfDescriptionTest, VarcharFieldsLimitedModifiedMemoryAllocation)
{
    std::size_t const reported_size = 10;
    std::size_t const limited_size = 5;
    std::size_t const size_with_extra_capacity = 20;
    cpp_odbc::column_description column_description = {name, SQL_VARCHAR, reported_size, 0, supports_null_values};

    turbodbc::options options;
    options.varchar_max_character_limit = 5;
    options.limit_varchar_results_to_max = true;

    options.force_extra_capacity_for_unicode = false;
    EXPECT_EQ(limited_size + 1, make_description(column_description, options)->element_size());

    options.force_extra_capacity_for_unicode = true;
    EXPECT_EQ(size_with_extra_capacity + 1, make_description(column_description, options)->element_size());
}

TEST(MakeDescriptionOfDescriptionTest, VarcharFieldsLimitedModifiedMemoryAllocationForUnicode)
{
    std::size_t const reported_size = 10;
    std::size_t const limited_size = 5;
    std::size_t const size_without_extra_capacity = (limited_size + 1) * sizeof(char16_t);
    std::size_t const size_with_extra_capacity = (limited_size*2 + 1) * sizeof(char16_t);
    cpp_odbc::column_description column_description = {name, SQL_WVARCHAR, reported_size, 0, supports_null_values};

    turbodbc::options options;
    options.varchar_max_character_limit = 5;
    options.limit_varchar_results_to_max = true;

    options.force_extra_capacity_for_unicode = false;
    EXPECT_EQ(size_without_extra_capacity, make_description(column_description, options)->element_size());

    options.force_extra_capacity_for_unicode = true;
    EXPECT_EQ(size_with_extra_capacity, make_description(column_description, options)->element_size());
}

TEST(MakeDescriptionOfDescriptionTest, UnicodeTypesForcedNarrowDecoding)
{
    std::size_t const size = 10;
    cpp_odbc::column_description const column_description = {name, SQL_WVARCHAR, size, 0, supports_null_values};

    turbodbc::options options;

    auto const test_unicode_description = make_description(column_description, options);

    options.fetch_wchar_as_char = true;

    auto const test_string_description = make_description(column_description, options);

    ASSERT_TRUE(dynamic_cast<turbodbc::unicode_description const *>(test_unicode_description.get()))
        << "Did not convert type identifier to expected unicode_description";
    ASSERT_TRUE(dynamic_cast<turbodbc::string_description const *>(test_string_description.get()))
        << "Did not convert type identifier to expected string_description";

    EXPECT_EQ(size + 1, test_string_description->element_size());
    EXPECT_EQ((size + 1) * 2, test_unicode_description->element_size());

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
    auto const description = make_description(column_description, make_options(prefer_strings, large_decimals_as_strings));
    ASSERT_TRUE(dynamic_cast<turbodbc::boolean_description const *>(description.get()));
    assert_custom_name_and_nullable_support(*description);
}

TEST(MakeDescriptionOfDescriptionTest, DateType)
{
    SQLSMALLINT const type = SQL_TYPE_DATE;

    cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
    auto const description = make_description(column_description, make_options(prefer_strings, large_decimals_as_strings));
    ASSERT_TRUE(dynamic_cast<turbodbc::date_description const *>(description.get()));
    assert_custom_name_and_nullable_support(*description);
}

TEST(MakeDescriptionOfDescriptionTest, TimestampTypes)
{
    SQLSMALLINT const type = SQL_TYPE_TIMESTAMP;

    cpp_odbc::column_description column_description = {name, type, 0, 0, supports_null_values};
    auto const description = make_description(column_description, make_options(prefer_strings, large_decimals_as_strings));
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

TEST(MakeDescriptionOfDescriptionTest, SmallDecimalAsInteger)
{
    test_as_integer(make_decimal_column_description(18, 0));
    test_as_integer(make_decimal_column_description(9, 0));
    test_as_integer(make_decimal_column_description(1, 0));
    test_as_integer(make_numeric_column_description(18, 0));
    test_as_integer(make_numeric_column_description(9, 0));
    test_as_integer(make_numeric_column_description(1, 0));
}

TEST(MakeDescriptionOfDescriptionTest, SmallDecimalAsFloatingPoint)
{
    test_as_floating_point(make_decimal_column_description(18, 1));
    test_as_floating_point(make_numeric_column_description(18, 1));
}

TEST(MakeDescriptionOfDescriptionTest, LargeDecimalAsString)
{
    std::size_t const size = 19;
    // add three bytes to size (null-termination, sign, decimal point
    test_text_with_string_preference<turbodbc::string_description>(make_decimal_column_description(size, 0), size + 3);
    test_text_with_string_preference<turbodbc::string_description>(make_decimal_column_description(size, 5), size + 3);
    test_text_with_string_preference<turbodbc::string_description>(make_numeric_column_description(size, 0), size + 3);
    test_text_with_string_preference<turbodbc::string_description>(make_numeric_column_description(size, 5), size + 3);
}


TEST(MakeDescriptionOfDescriptionTest, LargeDecimalAsInteger)
{
    std::size_t const size = 19;
    test_as_integer(make_decimal_column_description(size, 0), large_decimals_as_64_bit_types);
}

TEST(MakeDescriptionOfDescriptionTest, LargeDecimalAsFloatingPoint)
{
    std::size_t const size = 19;
    test_as_floating_point(make_decimal_column_description(size, 1), large_decimals_as_64_bit_types);
}

