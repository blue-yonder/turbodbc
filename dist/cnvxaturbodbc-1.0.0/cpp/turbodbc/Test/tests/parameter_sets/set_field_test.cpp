#include "turbodbc/parameter_sets/set_field.h"

#include <gtest/gtest.h>
#include <tests/mock_classes.h>

#include <turbodbc/descriptions.h>



namespace {

std::size_t const param_index = 42;
std::size_t const n_params = 23;

}

using namespace turbodbc;
typedef turbodbc_test::mock_statement mock_statement;


TEST(SetFieldTest, ParameterIsSuitableForBoolean)
{
    mock_statement statement;
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));
    parameter const integer_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new integer_description()));

    field const value(true);
    EXPECT_TRUE(parameter_is_suitable_for(boolean_parameter, value));
    EXPECT_FALSE(parameter_is_suitable_for(integer_parameter, value));
}

TEST(SetFieldTest, ParameterIsSuitableForInteger)
{
    mock_statement statement;
    parameter const integer_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new integer_description()));
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));

    field const value(int64_t(42));
    EXPECT_TRUE(parameter_is_suitable_for(integer_parameter, value));
    EXPECT_FALSE(parameter_is_suitable_for(boolean_parameter, value));
}

TEST(SetFieldTest, ParameterIsSuitableForFloatingPoint)
{
    mock_statement statement;
    parameter const float_parameter(statement, param_index, n_params,
                                    std::unique_ptr<description>(new floating_point_description()));
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));

    field const value(3.14);
    EXPECT_TRUE(parameter_is_suitable_for(float_parameter, value));
    EXPECT_FALSE(parameter_is_suitable_for(boolean_parameter, value));
}

TEST(SetFieldTest, ParameterIsSuitableForTimestamp)
{
    mock_statement statement;
    parameter const ts_parameter(statement, param_index, n_params,
                                 std::unique_ptr<description>(new timestamp_description()));
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));

    field const value(boost::posix_time::ptime({2016, 9, 23}, {1, 2, 3}));
    EXPECT_TRUE(parameter_is_suitable_for(ts_parameter, value));
    EXPECT_FALSE(parameter_is_suitable_for(boolean_parameter, value));
}

TEST(SetFieldTest, ParameterIsSuitableForDate)
{
    mock_statement statement;
    parameter const date_parameter(statement, param_index, n_params,
                                   std::unique_ptr<description>(new date_description()));
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));

    field const value(boost::gregorian::date{2016, 9, 23});
    EXPECT_TRUE(parameter_is_suitable_for(date_parameter, value));
    EXPECT_FALSE(parameter_is_suitable_for(boolean_parameter, value));
}

TEST(SetFieldTest, ParameterIsSuitableForString)
{
    mock_statement statement;
    parameter const string_parameter(statement, param_index, n_params,
                                     std::unique_ptr<description>(new string_description(7)));
    parameter const boolean_parameter(statement, param_index, n_params,
                                      std::unique_ptr<description>(new boolean_description()));

    field const shorter(std::string("123456"));
    field const maximum_length(std::string("1234567"));
    field const too_long(std::string("12345678"));

    EXPECT_TRUE(parameter_is_suitable_for(string_parameter, shorter));
    EXPECT_TRUE(parameter_is_suitable_for(string_parameter, maximum_length));
    EXPECT_FALSE(parameter_is_suitable_for(string_parameter, too_long));
    EXPECT_FALSE(parameter_is_suitable_for(boolean_parameter, shorter));
}


TEST(SetFieldTest, SetFieldBoolean)
{
    cpp_odbc::multi_value_buffer buffer(1, 1);
    auto element = buffer[0];

    set_field(turbodbc::field{true}, element);
    EXPECT_EQ(1, *element.data_pointer);
    EXPECT_EQ(1, element.indicator);

    set_field(turbodbc::field{false}, element);
    EXPECT_EQ(0, *element.data_pointer);
    EXPECT_EQ(1, element.indicator);
}


TEST(SetFieldTest, SetFieldInteger)
{
    cpp_odbc::multi_value_buffer buffer(8, 1);
    auto element = buffer[0];

    set_field(turbodbc::field{int64_t(42)}, element);
    EXPECT_EQ(42, *reinterpret_cast<int64_t *>(element.data_pointer));
    EXPECT_EQ(8, element.indicator);
}


TEST(SetFieldTest, SetFieldFloatingPoint)
{
    cpp_odbc::multi_value_buffer buffer(8, 1);
    auto element = buffer[0];

    set_field(turbodbc::field{3.14}, element);
    EXPECT_EQ(3.14, *reinterpret_cast<double *>(element.data_pointer));
    EXPECT_EQ(8, element.indicator);
}


TEST(SetFieldTest, SetFieldTimestamp)
{
    boost::posix_time::ptime const timestamp{{2015, 12, 31}, {1, 2, 3, 123456}};

    cpp_odbc::multi_value_buffer buffer(sizeof(SQL_TIMESTAMP_STRUCT), 1);
    auto element = buffer[0];

    set_field(turbodbc::field{timestamp}, element);
    auto const as_sql_ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(element.data_pointer);
    EXPECT_EQ(2015, as_sql_ts->year);
    EXPECT_EQ(12, as_sql_ts->month);
    EXPECT_EQ(31, as_sql_ts->day);
    EXPECT_EQ(1, as_sql_ts->hour);
    EXPECT_EQ(2, as_sql_ts->minute);
    EXPECT_EQ(3, as_sql_ts->second);
    EXPECT_EQ(123456000, as_sql_ts->fraction);
    EXPECT_EQ(sizeof(SQL_TIMESTAMP_STRUCT), element.indicator);
}


TEST(SetFieldTest, SetFieldDate)
{
    boost::gregorian::date const date{2015, 12, 31};

    cpp_odbc::multi_value_buffer buffer(sizeof(SQL_DATE_STRUCT), 1);
    auto element = buffer[0];

    set_field(turbodbc::field{date}, element);
    auto const as_sql_date = reinterpret_cast<SQL_DATE_STRUCT const *>(element.data_pointer);
    EXPECT_EQ(2015, as_sql_date->year);
    EXPECT_EQ(12, as_sql_date->month);
    EXPECT_EQ(31, as_sql_date->day);
    EXPECT_EQ(sizeof(SQL_DATE_STRUCT), element.indicator);
}


TEST(SetFieldTest, SetFieldString)
{
    std::string const expected("another test string");
    auto const bytes_with_null_termination = expected.size() + 1;

    cpp_odbc::multi_value_buffer buffer(bytes_with_null_termination, 1);
    auto element = buffer[0];

    set_field(turbodbc::field{expected}, element);
    EXPECT_EQ(expected, std::string(element.data_pointer));
    EXPECT_EQ(expected.size(), element.indicator);
}


TEST(SetFieldTest, SetFieldNull)
{
    cpp_odbc::multi_value_buffer buffer(8, 1);
    auto element = buffer[0];

    nullable_field const value;
    set_null(element);
    EXPECT_EQ(SQL_NULL_DATA, element.indicator);
}