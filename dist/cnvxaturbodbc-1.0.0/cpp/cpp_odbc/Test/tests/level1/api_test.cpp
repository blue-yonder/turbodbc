#include "cpp_odbc/level1/api.h"

#include <gtest/gtest.h>

#include "cpp_odbc_test/level1_mock_api.h"


#include <array>

namespace level1 = cpp_odbc::level1;
using cpp_odbc_test::level1_mock_api;

namespace {

    // destinations for pointers
    int value_a = 23;
    int value_b = 17;

}


TEST(Level1APITest, AllocateHandleForwards)
{
    SQLRETURN const expected = 42;
    SQLSMALLINT const handle_type = 17;
    SQLHANDLE const input_handle = &value_a;
    SQLHANDLE output_handle = &value_b;

    level1_mock_api api;
    EXPECT_CALL(api, do_allocate_handle(handle_type, input_handle, &output_handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.allocate_handle(handle_type, input_handle, &output_handle);
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, FreeHandleForwards)
{
    SQLRETURN const expected = 42;
    SQLSMALLINT const handle_type = 17;
    SQLHANDLE const input_handle = &value_a;

    level1_mock_api api;
    EXPECT_CALL(api, do_free_handle(handle_type, input_handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.free_handle(handle_type, input_handle);
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, GetDiagnosticRecordForwards)
{
    SQLRETURN const expected = 1;
    SQLSMALLINT const handle_type = 42;
    SQLHANDLE const handle = &value_a;
    SQLSMALLINT const record_id = 17;
    std::array<unsigned char, 5> status_code;
    SQLINTEGER native_error_ptr = 23;
    std::array<unsigned char, 6> message_text;
    SQLSMALLINT const buffer_length = 123;
    SQLSMALLINT text_length = 95;

    level1_mock_api api;
    EXPECT_CALL(api, do_get_diagnostic_record(handle_type, handle, record_id, status_code.data(), &native_error_ptr, message_text.data(), buffer_length, &text_length))
        .WillOnce(testing::Return(expected));

    auto const actual = api.get_diagnostic_record(handle_type, handle, record_id, status_code.data(), &native_error_ptr, message_text.data(), buffer_length, &text_length);
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, SetEnvironmentAttributeForwards)
{
    SQLRETURN const expected = 1;
    SQLHENV const handle = &value_a;
    SQLINTEGER const attribute = 17;
    std::array<unsigned char, 6> value;

    level1_mock_api api;
    EXPECT_CALL(api, do_set_environment_attribute(handle, attribute, value.data(), value.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.set_environment_attribute(handle, attribute, value.data(), value.size());
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, SetConnectionAttributeForwards)
{
    SQLRETURN const expected = 1;
    SQLHDBC const handle = &value_a;
    SQLINTEGER const attribute = 17;
    std::array<unsigned char, 6> value;

    level1_mock_api api;
    EXPECT_CALL(api, do_set_connection_attribute(handle, attribute, value.data(), value.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.set_connection_attribute(handle, attribute, value.data(), value.size());
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, EstablishConnectionForwards)
{
    SQLRETURN const expected = 27;
    SQLHDBC connection_handle = &value_a;
    SQLHWND window_handle = reinterpret_cast<SQLHWND>(&value_b);
    std::array<unsigned char, 6> input_string;
    std::array<unsigned char, 7> output_string;
    SQLSMALLINT output_string_length = 123;
    SQLUSMALLINT const driver_completion = 24;

    level1_mock_api api;
    EXPECT_CALL(api, do_establish_connection(connection_handle, window_handle, input_string.data(), input_string.size(), output_string.data(), output_string.size(), &output_string_length, driver_completion))
        .WillOnce(testing::Return(expected));

    auto const actual = api.establish_connection(connection_handle, window_handle, input_string.data(), input_string.size(), output_string.data(), output_string.size(), &output_string_length, driver_completion);
    EXPECT_EQ( expected, actual );
}

TEST(Level1APITest, DisconnectForwards)
{
    SQLHDBC connection_handle = &value_a;
    SQLRETURN const expected = 27;

    level1_mock_api api;
    EXPECT_CALL(api, do_disconnect(connection_handle))
        .WillOnce(testing::Return(expected));

    api.disconnect(connection_handle);
}

TEST(Level1APITest, EndTransactionForwards)
{
    SQLRETURN const expected = 23;
    SQLSMALLINT const handle_type = 17;
    SQLHANDLE handle = &value_a;
    SQLSMALLINT const completion_type = 42;

    level1_mock_api api;
    EXPECT_CALL(api, do_end_transaction(handle_type, handle, completion_type))
        .WillOnce(testing::Return(expected));

    auto const actual = api.end_transaction(handle_type, handle, completion_type);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, GetConnectionInfoForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    SQLUSMALLINT const info_type = 17;
    std::vector<char> buffer(10);
    SQLSMALLINT string_length = 0;

    level1_mock_api api;
    EXPECT_CALL(api, do_get_connection_info(handle, info_type, buffer.data(), buffer.size(), &string_length))
        .WillOnce(testing::Return(expected));

    auto const actual = api.get_connection_info(handle, info_type, buffer.data(), buffer.size(), &string_length);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, BindColumnForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    SQLUSMALLINT const column_id = 17;
    SQLSMALLINT const target_type = 42;
    std::vector<char> buffer(100);
    SQLLEN buffer_length;

    level1_mock_api api;
    EXPECT_CALL(api, do_bind_column(handle, column_id, target_type, buffer.data(), buffer.size(), &buffer_length))
        .WillOnce(testing::Return(expected));

    auto const actual = api.bind_column(handle, column_id, target_type, buffer.data(), buffer.size(), &buffer_length);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, BindParameterForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    SQLUSMALLINT const parameter_id = 17;
    SQLSMALLINT const input_output_type = 42;
    SQLSMALLINT const value_type = 33;
    SQLSMALLINT const parameter_type = 91;

    SQLULEN const column_size = 5;
    SQLSMALLINT const decimal_digits = 6;
    std::vector<char> buffer(100);
    SQLLEN buffer_length;

    level1_mock_api api;
    EXPECT_CALL(api, do_bind_parameter(handle, parameter_id, input_output_type, value_type, parameter_type, column_size, decimal_digits, buffer.data(), buffer.size(), &buffer_length))
        .WillOnce(testing::Return(expected));

    auto const actual = api.bind_parameter(handle, parameter_id, input_output_type, value_type, parameter_type, column_size, decimal_digits, buffer.data(), buffer.size(), &buffer_length);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, ColumnAttributeForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    SQLUSMALLINT const column_id = 1;
    SQLUSMALLINT const field_identifier = 2;
    std::vector<char> buffer(100);
    SQLSMALLINT buffer_length;
    SQLLEN numeric_attribute;

    level1_mock_api api;
    EXPECT_CALL(api, do_column_attribute(handle, column_id, field_identifier, buffer.data(), buffer.size(), &buffer_length, &numeric_attribute))
        .WillOnce(testing::Return(expected));

    auto const actual = api.column_attribute(handle, column_id, field_identifier, buffer.data(), buffer.size(), &buffer_length, &numeric_attribute);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, ExecutePreparedStatementForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;

    level1_mock_api api;
    EXPECT_CALL(api, do_execute_prepared_statement(handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.execute_prepared_statement(handle);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, ExecuteStatementForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    std::array<unsigned char, 5> statement;

    level1_mock_api api;
    EXPECT_CALL(api, do_execute_statement(handle, statement.data(), statement.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.execute_statement(handle, statement.data(), statement.size());
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, FetchScrollForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    SQLSMALLINT const fetch_orientation = 17;
    SQLLEN const fetch_offset = 42;

    level1_mock_api api;
    EXPECT_CALL(api, do_fetch_scroll(handle, fetch_orientation, fetch_offset))
        .WillOnce(testing::Return(expected));

    auto const actual = api.fetch_scroll(handle, fetch_orientation, fetch_offset);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, FreeStatementForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLUSMALLINT const option = 42;

    level1_mock_api api;
    EXPECT_CALL(api, do_free_statement(handle, option))
        .WillOnce(testing::Return(expected));

    auto const actual = api.free_statement(handle, option);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, GetStatementAttributeForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLINTEGER const attribute = 42;
    std::vector<char> buffer(100);
    SQLINTEGER string_length = 1;

    level1_mock_api api;
    EXPECT_CALL(api, do_get_statement_attribute(handle, attribute, buffer.data(), buffer.size(), &string_length))
        .WillOnce(testing::Return(expected));

    auto const actual = api.get_statement_attribute(handle, attribute, buffer.data(), buffer.size(), &string_length);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, NumberOfResultColumnsForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLSMALLINT number_of_columns = 0;

    level1_mock_api api;
    EXPECT_CALL(api, do_number_of_result_columns(handle, &number_of_columns))
        .WillOnce(testing::Return(expected));

    auto const actual = api.number_of_result_columns(handle, &number_of_columns);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, NumberOfParametersForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLSMALLINT number_of_parameters = 0;

    level1_mock_api api;
    EXPECT_CALL(api, do_number_of_parameters(handle, &number_of_parameters))
        .WillOnce(testing::Return(expected));

    auto const actual = api.number_of_parameters(handle, &number_of_parameters);
    EXPECT_EQ(expected, actual);
}


TEST(Level1APITest, PrepareStatementForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    std::array<unsigned char, 5> statement;

    level1_mock_api api;
    EXPECT_CALL(api, do_prepare_statement(handle, statement.data(), statement.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.prepare_statement(handle, statement.data(), statement.size());
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, PrepareWideStatementForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;
    std::array<SQLWCHAR, 5> statement;

    level1_mock_api api;
    EXPECT_CALL(api, do_prepare_statement(handle, statement.data(), statement.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.prepare_statement(handle, statement.data(), statement.size());
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, SetStatementAttributeForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLINTEGER const attribute = 42;
    std::vector<char> buffer(100);

    level1_mock_api api;
    EXPECT_CALL(api, do_set_statement_attribute(handle, attribute, buffer.data(), buffer.size()))
        .WillOnce(testing::Return(expected));

    auto const actual = api.set_statement_attribute(handle, attribute, buffer.data(), buffer.size());
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, RowCountForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLLEN count = 42;

    level1_mock_api api;
    EXPECT_CALL(api, do_row_count(handle, &count))
        .WillOnce(testing::Return(expected));

    auto const actual = api.row_count(handle, &count);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, DescribeColumnForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLUSMALLINT const column_id = 17;
    std::array<SQLCHAR, 2> column_name;
    SQLSMALLINT const buffer_length = 42;
    SQLSMALLINT name_length = 99;
    SQLSMALLINT data_type = 123;
    SQLULEN column_size = 456;
    SQLSMALLINT decimal_digits = 666;
    SQLSMALLINT nullable = 5;

    level1_mock_api api;
    EXPECT_CALL(api, do_describe_column(handle, column_id, column_name.data(), buffer_length, &name_length, &data_type, &column_size, &decimal_digits, &nullable))
        .WillOnce(testing::Return(expected));

    auto const actual = api.describe_column(handle, column_id, column_name.data(), buffer_length, &name_length, &data_type, &column_size, &decimal_digits, &nullable);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, DescribeWideColumnForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLUSMALLINT const column_id = 17;
    std::array<SQLWCHAR, 2> column_name;
    SQLSMALLINT const buffer_length = 42;
    SQLSMALLINT name_length = 99;
    SQLSMALLINT data_type = 123;
    SQLULEN column_size = 456;
    SQLSMALLINT decimal_digits = 666;
    SQLSMALLINT nullable = 5;
    
    level1_mock_api api;
    EXPECT_CALL(api, do_describe_column(handle, column_id, column_name.data(), buffer_length, &name_length, &data_type, &column_size, &decimal_digits, &nullable))
        .WillOnce(testing::Return(expected));
    
    auto const actual = api.describe_column(handle, column_id, column_name.data(), buffer_length, &name_length, &data_type, &column_size, &decimal_digits, &nullable);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, DescribeParameterForwards)
{
    SQLRETURN const expected = 23;
    SQLHSTMT handle = &value_a;
    SQLUSMALLINT const parameter_id = 17;
    SQLSMALLINT data_type = 123;
    SQLULEN column_size = 456;
    SQLSMALLINT decimal_digits = 666;
    SQLSMALLINT nullable = 5;

    level1_mock_api api;
    EXPECT_CALL(api, do_describe_parameter(handle, parameter_id, &data_type, &column_size, &decimal_digits, &nullable))
        .WillOnce(testing::Return(expected));

    auto const actual = api.describe_parameter(handle, parameter_id, &data_type, &column_size, &decimal_digits, &nullable);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, MoreResultsForwards)
{
    SQLRETURN const expected = 23;
    SQLHDBC handle = &value_a;

    level1_mock_api api;
    EXPECT_CALL(api, do_more_results(handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.more_results(handle);
    EXPECT_EQ(expected, actual);
}

TEST(Level1APITest, GetFunctionsForwards)
{
    SQLRETURN const expected = 23;
    SQLUSMALLINT const function_id = 17;
    SQLUSMALLINT destination = 0;
    SQLHDBC handle = &value_a;

    level1_mock_api api;
    EXPECT_CALL(api, do_get_functions(handle, function_id, &destination))
        .WillOnce(testing::Return(expected));

    auto const actual = api.get_functions(handle, function_id, &destination);
    EXPECT_EQ(expected, actual);
}
