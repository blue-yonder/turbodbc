#include "cpp_odbc/level2/api.h"

#include <gtest/gtest.h>

#include "cpp_odbc_test/level2_mock_api.h"

#include "sqlext.h"


namespace level2 = cpp_odbc::level2;

namespace {

    // destinations for pointers, values irrelevant
    int value_a = 17;
    int value_b = 23;

}

using cpp_odbc_test::level2_mock_api;

TEST(Level2APITest, AllocateStatementHandleForwards)
{
    level2::connection_handle input_handle = {&value_a};
    level2::statement_handle expected = {&value_b};

    level2_mock_api api;
    EXPECT_CALL(api, do_allocate_statement_handle(input_handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.allocate_statement_handle(input_handle);
    EXPECT_EQ(expected, actual);
}

TEST(Level2APITest, AllocateConnectionHandleForwards)
{
    level2::environment_handle input_handle = {&value_a};
    level2::connection_handle expected = {&value_b};

    level2_mock_api api;
    EXPECT_CALL(api, do_allocate_connection_handle(input_handle))
        .WillOnce(testing::Return(expected));

    auto const actual = api.allocate_connection_handle(input_handle);
    EXPECT_EQ(expected, actual);
}

TEST(Level2APITest, AllocateEnvironmentHandleForwards)
{
    level2::environment_handle expected = {&value_b};

    level2_mock_api api;
    EXPECT_CALL(api, do_allocate_environment_handle())
        .WillOnce(testing::Return(expected));

    auto const actual = api.allocate_environment_handle();
    EXPECT_EQ(expected, actual);
}

TEST(Level2APITest, FreeStatementHandleForwards)
{
    level2::statement_handle handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_free_handle(handle)).Times(1);

    api.free_handle(handle);
}

TEST(Level2APITest, FreeConnectionHandleForwards)
{
    level2::connection_handle handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_free_handle(handle)).Times(1);

    api.free_handle(handle);
}

TEST(Level2APITest, FreeEnvironmentHandleForwards)
{
    level2::environment_handle handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_free_handle(handle)).Times(1);

    api.free_handle(handle);
}

namespace {

    template <typename Handle>
    void test_get_diagnostic_record_forwards()
    {
        Handle const handle = {&value_a};
        level2::diagnostic_record const expected = {"abcde", 17, "test"};

        level2_mock_api api;
        EXPECT_CALL(api, do_get_diagnostic_record(handle))
            .WillOnce(testing::Return(expected));

        auto actual = api.get_diagnostic_record(handle);

        EXPECT_EQ(expected.odbc_status_code, actual.odbc_status_code);
        EXPECT_EQ(expected.native_error_code, actual.native_error_code);
        EXPECT_EQ(expected.message, actual.message);
    }

}

TEST(Level2APITest, GetStatementDiagnosticRecordForwards)
{
    test_get_diagnostic_record_forwards<level2::statement_handle>();
}

TEST(Level2APITest, GetConnectionDiagnosticRecordForwards)
{
    test_get_diagnostic_record_forwards<level2::connection_handle>();
}

TEST(Level2APITest, GetEnvironmentDiagnosticRecordForwards)
{
    test_get_diagnostic_record_forwards<level2::environment_handle>();
}

TEST(Level2APITest, SetEnvironmentAttributeForwards)
{
    level2::environment_handle const handle = {&value_a};
    SQLINTEGER const attribute = 42;
    intptr_t const value = 17;

    level2_mock_api api;
    EXPECT_CALL(api, do_set_environment_attribute(handle, attribute, value))
        .Times(1);

    api.set_environment_attribute(handle, attribute, value);
}

TEST(Level2APITest, SetConnectionAttributeForwards)
{
    level2::connection_handle const handle = {&value_a};
    SQLINTEGER const attribute = 42;
    intptr_t const value = 17;

    level2_mock_api api;
    EXPECT_CALL(api, do_set_connection_attribute(handle, attribute, value))
        .Times(1);

    api.set_connection_attribute(handle, attribute, value);
}

TEST(Level2APITest, EstablishConnectionForwards)
{
    level2::connection_handle handle = {&value_a};
    std::string const connection_string = "My fancy database";

    level2_mock_api api;
    EXPECT_CALL(api, do_establish_connection(handle, connection_string))
        .Times(1);

    api.establish_connection(handle, connection_string);
}

TEST(Level2APITest, DisconnectForwards)
{
    level2::connection_handle handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_disconnect(handle))
        .Times(1);

    api.disconnect(handle);
}

TEST(Level2APITest, EndTransactionForwards)
{
    level2::connection_handle handle = {&value_a};
    SQLSMALLINT const completion_type = SQL_COMMIT;

    level2_mock_api api;
    EXPECT_CALL(api, do_end_transaction(handle, completion_type))
        .Times(1);

    api.end_transaction(handle, completion_type);
}

TEST(Level2APITest, GetStringConnectionInfoForwards)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_DRIVER_ODBC_VER;
    std::string const expected_info("dummy");

    level2_mock_api api;
    EXPECT_CALL(api, do_get_string_connection_info(handle, info_type))
        .WillOnce(testing::Return(expected_info));

    EXPECT_EQ(expected_info, api.get_string_connection_info(handle, info_type));
}

TEST(Level2APITest, GetIntegerConnectionInfoForwards)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_DRIVER_ODBC_VER;
    SQLUINTEGER const expected = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_get_integer_connection_info(handle, info_type))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, api.get_integer_connection_info(handle, info_type));
}

TEST(Level2APITest, BindColumnForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT column_id = 17;
    SQLSMALLINT column_type = 42;
    cpp_odbc::multi_value_buffer column_buffer(2,3);

    level2_mock_api api;
    EXPECT_CALL(api, do_bind_column(handle, column_id, column_type, testing::Ref(column_buffer))).Times(1);

    api.bind_column(handle, column_id, column_type, column_buffer);
}

TEST(Level2APITest, BindInputParameterForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT parameter_id = 17;
    SQLSMALLINT c_data_type = 42;
    SQLSMALLINT sql_data_type = 23;
    SQLSMALLINT digits = 5;
    cpp_odbc::multi_value_buffer column_buffer(2,3);

    level2_mock_api api;
    EXPECT_CALL(api, do_bind_input_parameter(handle, parameter_id, c_data_type, sql_data_type, digits, testing::Ref(column_buffer))).Times(1);

    api.bind_input_parameter(handle, parameter_id, c_data_type, sql_data_type, digits, column_buffer);
}

TEST(Level2APITest, GetStringColumnAttributeForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT column_id = 17;
    SQLUSMALLINT field_identifier = 23;
    std::string const expected("value");

    level2_mock_api api;
    EXPECT_CALL(api, do_get_string_column_attribute(handle, column_id, field_identifier))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, api.get_string_column_attribute(handle, column_id, field_identifier) );
}

TEST(Level2APITest, GetIntegerColumnAttributeForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT column_id = 17;
    SQLUSMALLINT field_identifier = 23;
    intptr_t const expected = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_get_integer_column_attribute(handle, column_id, field_identifier))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, api.get_integer_column_attribute(handle, column_id, field_identifier) );
}

TEST(Level2APITest, ExecutePreparedStatementForwards)
{
    level2::statement_handle const handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_execute_prepared_statement(handle)).Times(1);

    api.execute_prepared_statement(handle);
}

TEST(Level2APITest, ExecuteStatementForwards)
{
    level2::statement_handle const handle = {&value_a};
    std::string const query("SELECT * FROM table");

    level2_mock_api api;
    EXPECT_CALL(api, do_execute_statement(handle, query)).Times(1);

    api.execute_statement(handle, query);
}

TEST(Level2APITest, FetchScrollForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLSMALLINT const orientation = SQL_FETCH_NEXT;
    SQLLEN const offset = 17;
    bool const expected = true;

    level2_mock_api api;
    EXPECT_CALL(api, do_fetch_scroll(handle, orientation, offset)).WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, api.fetch_scroll(handle, orientation, offset));
}

TEST(Level2APITest, FreeStatementForwards)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const option = SQL_CLOSE;

    level2_mock_api api;
    EXPECT_CALL(api, do_free_statement(handle, option)).Times(1);

    api.free_statement(handle, option);
}

TEST(Level2APITest, GetIntegerStatementAttributeForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLINTEGER const attribute = 42;
    intptr_t const expected = 17;

    level2_mock_api api;
    EXPECT_CALL(api, do_get_integer_statement_attribute(handle, attribute))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, api.get_integer_statement_attribute(handle, attribute));
}

TEST(Level2APITest, NumberOfResultColumnsForwards)
{
    level2::statement_handle const handle = {&value_a};
    short int const expected = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_number_of_result_columns(handle))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, api.number_of_result_columns(handle));
}

TEST(Level2APITest, NumberOfParametersForwards)
{
    level2::statement_handle const handle = {&value_a};
    short int const expected = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_number_of_parameters(handle))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, api.number_of_parameters(handle));
}

TEST(Level2APITest, PrepareStatementForwards)
{
    level2::statement_handle const handle = {&value_a};
    std::string const query("SELECT * FROM table");

    level2_mock_api api;
    EXPECT_CALL(api, do_prepare_statement(handle, query)).Times(1);

    api.prepare_statement(handle, query);
}

TEST(Level2APITest, PrepareWideStatementForwards)
{
    level2::statement_handle const handle = {&value_a};
    std::u16string const query(u"SELECT * FROM table");

    level2_mock_api api;
    EXPECT_CALL(api, do_prepare_statement(handle, query)).Times(1);

    api.prepare_statement(handle, query);
}

TEST(Level2APITest, SetLongStatementAttributeForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLINTEGER const attribute = 23;
    intptr_t const value = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_set_statement_attribute(handle, attribute, value)).Times(1);

    api.set_statement_attribute(handle, attribute, value);
}

TEST(Level2APITest, SetPointerStatementAttributeForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLINTEGER const attribute = 23;
    SQLULEN value = 42;

    level2_mock_api api;
    EXPECT_CALL(api, do_set_statement_attribute(handle, attribute, &value)).Times(1);

    api.set_statement_attribute(handle, attribute, &value);
}

TEST(Level2APITest, RowCountForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLLEN const expected = 23;

    level2_mock_api api;
    EXPECT_CALL(api, do_row_count(handle)).WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, api.row_count(handle));
}

TEST(Level2APITest, DescribeColumnForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT const column_id = 42;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    level2_mock_api api;
    EXPECT_CALL(api, do_describe_column(handle, column_id)).WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, api.describe_column(handle, column_id));
}

TEST(Level2APITest, DescribeColumnWideForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT const column_id = 42;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};
    
    level2_mock_api api;
    EXPECT_CALL(api, do_describe_column_wide(handle, column_id)).WillOnce(testing::Return(expected));
    
    EXPECT_EQ(expected, api.describe_column_wide(handle, column_id));
}

TEST(Level2APITest, DescribeParameterForwards)
{
    level2::statement_handle const handle = {&value_a};
    SQLUSMALLINT const parameter_id = 42;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    level2_mock_api api;
    EXPECT_CALL(api, do_describe_parameter(handle, parameter_id)).WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, api.describe_parameter(handle, parameter_id));
}

TEST(Level2APITest, MoreResultsForwards)
{
    level2::statement_handle const handle = {&value_a};

    level2_mock_api api;
    EXPECT_CALL(api, do_more_results(handle)).WillOnce(testing::Return(false));

    EXPECT_FALSE(api.more_results(handle));
}

TEST(Level2APITest, SupportsFunctionForwards)
{
    level2::connection_handle const handle = {&value_a};
    SQLUSMALLINT const function_id = 13;

    level2_mock_api api;
    EXPECT_CALL(api, do_supports_function(handle, function_id)).WillOnce(testing::Return(false));

    EXPECT_FALSE(api.supports_function(handle, function_id));
}
