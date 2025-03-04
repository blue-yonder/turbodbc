#include "cpp_odbc/level2/level1_connector.h"

#include <gtest/gtest.h>

#include "cpp_odbc/error.h"

#include "cpp_odbc_test/level1_mock_api.h"

#include "sqlext.h"

#include <type_traits>


namespace level2 = cpp_odbc::level2;
using level2::level1_connector;

namespace {

    // destinations for pointers, values irrelevant
    int value_a = 17;
    int value_b = 23;

    level2::diagnostic_record const expected_error = {"ABCDE", 23, "This is a test error message"};
    level2::diagnostic_record const expected_info = {"XYZ12", 42, "This is a test info message"};

    void expect_diagnostic_record(cpp_odbc_test::level1_mock_api const & mock, level2::diagnostic_record const & expected)
    {
        EXPECT_CALL(mock, do_get_diagnostic_record(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
            .WillOnce(testing::DoAll(
                        testing::SetArrayArgument<3>(expected.odbc_status_code.begin(), expected.odbc_status_code.end()),
                        testing::SetArgPointee<4>(expected.native_error_code),
                        testing::SetArrayArgument<5>(expected.message.begin(), expected.message.end()),
                        testing::SetArgPointee<7>(expected.message.size()),
                        testing::Return(SQL_SUCCESS)
                    ));
    }

}

TEST(Level1ConnectorTest, IsLevel2API)
{
    bool const is_level_2_api = std::is_base_of<cpp_odbc::level2::api, level1_connector>::value;
    EXPECT_TRUE( is_level_2_api );
}

TEST(Level1ConnectorTest, AllocateStatementHandleCallsAPI)
{
    level2::connection_handle input_handle = {&value_a};
    level2::statement_handle const expected_output_handle = {&value_b};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_STMT, input_handle.handle, testing::_) )
        .WillOnce(testing::DoAll(
                testing::SetArgPointee<2>(expected_output_handle.handle),
                testing::Return(SQL_SUCCESS)
        ));

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_statement_handle(input_handle);
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateConnectionHandleCallsAPI)
{
    level2::environment_handle input_handle = {&value_a};
    level2::connection_handle const expected_output_handle = {&value_b};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_DBC, input_handle.handle, testing::_) )
        .WillOnce(testing::DoAll(
                testing::SetArgPointee<2>(expected_output_handle.handle),
                testing::Return(SQL_SUCCESS)
        ));

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_connection_handle(input_handle);
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateEnvironmentHandleCallsAPI)
{
    level2::environment_handle const expected_output_handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_ENV, nullptr, testing::_) )
        .WillOnce(testing::DoAll(
                testing::SetArgPointee<2>(expected_output_handle.handle),
                testing::Return(SQL_SUCCESS)
        ));

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_environment_handle();
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateStatementHandleSucceedsWithInfo)
{
    level2::connection_handle input_handle = {&value_a};
    level2::statement_handle const expected_output_handle = {&value_b};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_STMT, input_handle.handle, testing::_))
        .WillOnce(testing::DoAll(
            testing::SetArgPointee<2>(expected_output_handle.handle),
            testing::Return(SQL_SUCCESS_WITH_INFO)
        ));
    expect_diagnostic_record(*api, expected_info);

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_statement_handle(input_handle);
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateConnectionHandleSucceedsWithInfo)
{
    level2::environment_handle input_handle = {&value_a};
    level2::connection_handle const expected_output_handle = {&value_b};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_DBC, input_handle.handle, testing::_) )
        .WillOnce(testing::DoAll(
            testing::SetArgPointee<2>(expected_output_handle.handle),
            testing::Return(SQL_SUCCESS_WITH_INFO)
        ));
    expect_diagnostic_record(*api, expected_info);

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_connection_handle(input_handle);
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateEnvironmentHandleSucceedsWithInfo)
{
    level2::environment_handle const expected_output_handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(SQL_HANDLE_ENV, nullptr, testing::_) )
        .WillOnce(testing::DoAll(
            testing::SetArgPointee<2>(expected_output_handle.handle),
            testing::Return(SQL_SUCCESS_WITH_INFO)
        ));
    // do not expect a diagnostic record to be queried because environment handles
    // are not derived from higher-order handles

    level1_connector const connector(api);

    auto const actual_output_handle = connector.allocate_environment_handle();
    EXPECT_EQ(expected_output_handle, actual_output_handle);
}

TEST(Level1ConnectorTest, AllocateStatementHandleFails)
{
    level2::connection_handle input_handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(testing::_, testing::_, testing::_) )
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);

    EXPECT_THROW( connector.allocate_statement_handle(input_handle), cpp_odbc::error );
}

TEST(Level1ConnectorTest, AllocateConnectionHandleFails)
{
    level2::environment_handle input_handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(testing::_, testing::_, testing::_) )
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);

    EXPECT_THROW( connector.allocate_connection_handle(input_handle), cpp_odbc::error );
}

TEST(Level1ConnectorTest, AllocateEnvironmentHandleFails)
{
    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_allocate_handle(testing::_, testing::_, testing::_) )
        .WillOnce(testing::Return(SQL_ERROR));

    level1_connector const connector(api);

    EXPECT_THROW( connector.allocate_environment_handle(), cpp_odbc::error );
}

namespace {
    template <typename Handle>
    void test_free_handle_calls_api(short signed int expected_handle_type)
    {
        Handle handle = {&value_a};

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_free_handle(expected_handle_type, handle.handle) )
            .WillOnce(testing::Return(SQL_SUCCESS));

        level1_connector const connector(api);

        connector.free_handle(handle);
    }
}

TEST(Level1ConnectorTest, FreeStatementHandleCallsAPI)
{
    test_free_handle_calls_api<level2::statement_handle>(SQL_HANDLE_STMT);
}

TEST(Level1ConnectorTest, FreeConnectionHandleCallsAPI)
{
    test_free_handle_calls_api<level2::connection_handle>(SQL_HANDLE_DBC);
}

TEST(Level1ConnectorTest, FreeEnvironmentHandleCallsAPI)
{
    test_free_handle_calls_api<level2::environment_handle>(SQL_HANDLE_ENV);
}

namespace {
    template <typename Handle>
    void test_free_handle_fails()
    {
        Handle handle = {&value_a};

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_free_handle(testing::_, testing::_) )
            .WillOnce(testing::Return(SQL_ERROR));
        expect_diagnostic_record(*api, expected_error);

        level1_connector const connector(api);

        EXPECT_THROW( connector.free_handle(handle), cpp_odbc::error);
    }
}

TEST(Level1ConnectorTest, FreeStatementHandleFails)
{
    test_free_handle_fails<level2::statement_handle>();
}

TEST(Level1ConnectorTest, FreeConnectionHandleFails)
{
    test_free_handle_fails<level2::connection_handle>();
}

TEST(Level1ConnectorTest, FreeEnvironmentHandleFails)
{
    test_free_handle_fails<level2::environment_handle>();
}

namespace {

    template <typename Handle>
    void test_diagnostic_record_calls_api(signed short int expected_type)
    {
        Handle const handle = {&value_a};
        std::string const expected_status_code("ABCDE");
        SQLINTEGER const expected_native_error = 23;
        std::string const expected_message = "This is a test error message";

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_get_diagnostic_record(expected_type, handle.handle, 1, testing::_, testing::_, testing::_, SQL_MAX_MESSAGE_LENGTH, testing::_))
            .WillOnce(testing::DoAll(
                        testing::SetArrayArgument<3>(expected_status_code.begin(), expected_status_code.end()),
                        testing::SetArgPointee<4>(expected_native_error),
                        testing::SetArrayArgument<5>(expected_message.begin(), expected_message.end()),
                        testing::SetArgPointee<7>(expected_message.size()),
                        testing::Return(SQL_SUCCESS)
                    ));

        level1_connector const connector(api);

        auto const actual = connector.get_diagnostic_record(handle);

        EXPECT_EQ( expected_status_code, actual.odbc_status_code );
        EXPECT_EQ( expected_native_error, actual.native_error_code );
        EXPECT_EQ( expected_message, actual.message );
    }

}

TEST(Level1ConnectorTest, GetStatementDiagnosticRecordCallsAPI)
{
    test_diagnostic_record_calls_api<level2::statement_handle>(SQL_HANDLE_STMT);
}

TEST(Level1ConnectorTest, GetConnectionDiagnosticRecordCallsAPI)
{
    test_diagnostic_record_calls_api<level2::connection_handle>(SQL_HANDLE_DBC);
}

TEST(Level1ConnectorTest, GetEnvironmentDiagnosticRecordCallsAPI)
{
    test_diagnostic_record_calls_api<level2::environment_handle>(SQL_HANDLE_ENV);
}

namespace {

    template <typename Handle>
    void test_diagnostic_record_fails()
    {
        Handle const handle = {&value_a};

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_get_diagnostic_record(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
            .WillOnce(testing::Return(SQL_ERROR));

        level1_connector const connector(api);

        EXPECT_THROW( connector.get_diagnostic_record(handle), cpp_odbc::error );
    }

}

TEST(Level1ConnectorTest, GetStatementDiagnosticRecordFails)
{
    test_diagnostic_record_fails<level2::statement_handle>();
}

TEST(Level1ConnectorTest, GetConnectionDiagnosticRecordFails)
{
    test_diagnostic_record_fails<level2::connection_handle>();
}

TEST(Level1ConnectorTest, GetEnvironmentDiagnosticRecordFails)
{
    test_diagnostic_record_fails<level2::environment_handle>();
}

TEST(Level1ConnectorTest, SetEnvironmentAttributeCallsAPI)
{
    level2::environment_handle const handle = {&value_a};
    SQLINTEGER const attribute = SQL_ATTR_ODBC_VERSION;
    intptr_t const value = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_environment_attribute(handle.handle, attribute, reinterpret_cast<SQLPOINTER>(value), 0))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.set_environment_attribute(handle, attribute, value);
}

TEST(Level1ConnectorTest, SetEnvironmentAttributeFails)
{
    level2::environment_handle const handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_environment_attribute(testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.set_environment_attribute(handle, 0, 0), cpp_odbc::error );
}

TEST(Level1ConnectorTest, SetConnectionAttributeCallsAPI)
{
    level2::connection_handle const handle = {&value_a};
    SQLINTEGER const attribute = SQL_ATTR_AUTOCOMMIT;
    intptr_t const value = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_connection_attribute(handle.handle, attribute, reinterpret_cast<SQLPOINTER>(value), 0))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.set_connection_attribute(handle, attribute, value);
}

TEST(Level1ConnectorTest, SetConnectionAttributeFails)
{
    level2::connection_handle const handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_connection_attribute(testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.set_connection_attribute(handle, 0, 0), cpp_odbc::error );
}

TEST(Level1ConnectorTest, EstablishConnectionCallsAPI)
{
    level2::connection_handle handle = {&value_a};
    std::string const connection_string = "dummy connection string";

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_establish_connection(handle.handle, nullptr, testing::_, connection_string.length(), testing::_, 1024, testing::_, SQL_DRIVER_NOPROMPT))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.establish_connection(handle, connection_string);
}

TEST(Level1ConnectorTest, EstablishConnectionFails)
{
    level2::connection_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_establish_connection(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.establish_connection(handle, "dummy connection string"), cpp_odbc::error);
}

TEST(Level1ConnectorTest, DisconnectCallsAPI)
{
    level2::connection_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_disconnect(handle.handle))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.disconnect(handle);
}

TEST(Level1ConnectorTest, DisconnectFails)
{
    level2::connection_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_disconnect(testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.disconnect(handle), cpp_odbc::error);
}

TEST(Level1ConnectorTest, EndTransactionCallsAPI)
{
    level2::connection_handle handle = {&value_a};
    SQLSMALLINT const completion_type = SQL_COMMIT;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_end_transaction(SQL_HANDLE_DBC, handle.handle, completion_type))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.end_transaction(handle, completion_type);
}

TEST(Level1ConnectorTest, EndTransactionFails)
{
    level2::connection_handle handle = {&value_a};
    SQLSMALLINT const completion_type = SQL_COMMIT;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_end_transaction(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.end_transaction(handle, completion_type), cpp_odbc::error);
}

TEST(Level1ConnectorTest, GetStringConnectionInfoCallsAPI)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_ODBC_VER;
    std::string const expected_info = "test info";

    auto copy_string_to_void_pointer = [&expected_info](testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused) {
        memcpy(destination, expected_info.data(), expected_info.size());
    };

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_connection_info(handle.handle, info_type, testing::_, 1024, testing::_))
        .WillOnce(testing::DoAll(
                    testing::Invoke(copy_string_to_void_pointer),
                    testing::SetArgPointee<4>(expected_info.size()),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ(expected_info, connector.get_string_connection_info(handle, info_type));
}

TEST(Level1ConnectorTest, GetStringConnectionInfoFails)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_ODBC_VER;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_connection_info(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW(connector.get_string_connection_info(handle, info_type), cpp_odbc::error);
}

TEST(Level1ConnectorTest, GetIntegerConnectionInfoCallsAPI)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_ODBC_VER;
    SQLUINTEGER const expected_info = 42;

    auto copy_int_to_void_pointer = [&expected_info](testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused) {
        memcpy(destination, &expected_info, sizeof(expected_info));
    };

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_connection_info(handle.handle, info_type, testing::_, 0, nullptr))
        .WillOnce(testing::DoAll(
                    testing::Invoke(copy_int_to_void_pointer),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ(expected_info, connector.get_integer_connection_info(handle, info_type));
}

TEST(Level1ConnectorTest, GetIntegerConnectionInfoFails)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const info_type = SQL_ODBC_VER;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_connection_info(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW(connector.get_integer_connection_info(handle, info_type), cpp_odbc::error);
}

TEST(Level1ConnectorTest, BindColumnCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 42;
    SQLSMALLINT const column_type = 17;
    cpp_odbc::multi_value_buffer column_buffer(23, 2);

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_bind_column(handle.handle, column_id, column_type, column_buffer.data_pointer(), column_buffer.capacity_per_element(), column_buffer.indicator_pointer()))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.bind_column(handle, column_id, column_type, column_buffer);
}
TEST(Level1ConnectorTest, BindColumnFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 42;
    SQLSMALLINT const column_type = 17;
    cpp_odbc::multi_value_buffer column_buffer(23, 2);

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_bind_column(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW(connector.bind_column(handle, column_id, column_type, column_buffer), cpp_odbc::error);
}
TEST(Level1ConnectorTest, BindInputParameterCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const parameter_id = 42;
    SQLSMALLINT const value_type = 17;
    SQLSMALLINT const parameter_type = 51;
    SQLSMALLINT const digits = 5;
    cpp_odbc::multi_value_buffer buffer(23, 2);

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_bind_parameter(handle.handle, parameter_id, SQL_PARAM_INPUT, value_type, parameter_type, buffer.capacity_per_element(), digits, buffer.data_pointer(), buffer.capacity_per_element(), buffer.indicator_pointer()))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.bind_input_parameter(handle, parameter_id, value_type, parameter_type, digits, buffer);
}
TEST(Level1ConnectorTest, BindInputParameterFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const parameter_id = 42;
    SQLSMALLINT const value_type = 17;
    SQLSMALLINT const parameter_type = 51;
    SQLSMALLINT const digits = 5;
    cpp_odbc::multi_value_buffer buffer(23, 2);

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_bind_parameter(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.bind_input_parameter(handle, parameter_id, value_type, parameter_type, digits, buffer), cpp_odbc::error);
}

TEST(Level1ConnectorTest, ExecutePreparedStatementCallsAPI)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_execute_prepared_statement(handle.handle))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.execute_prepared_statement(handle);
}

TEST(Level1ConnectorTest, ExecutePreparedStatementFails)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_execute_prepared_statement(testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.execute_prepared_statement(handle), cpp_odbc::error);
}

namespace {

    // useful functor for comparing unsigned char * with strings
    struct matches_string {
        matches_string(std::string matchee) :
            matchee(std::move(matchee))
        {
        }

        bool operator()(unsigned char * pointer) const
        {
            return (memcmp(pointer, matchee.c_str(), matchee.size()) == 0);
        }

        std::string matchee;
    };

    // useful functor for comparing unsigned char * with strings
    struct matches_u16string {
        matches_u16string(std::u16string matchee) :
            matchee(std::move(matchee))
        {
        }

        bool operator()(SQLWCHAR * pointer) const
        {
            return (memcmp(pointer, matchee.c_str(), matchee.size() * 2) == 0);
        }

        std::u16string matchee;
    };

}

TEST(Level1ConnectorTest, ExecuteStatementCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    std::string const sql = "XXX";

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_execute_statement(handle.handle, testing::Truly(matches_string(sql)), sql.size()))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.execute_statement(handle, sql);
}

TEST(Level1ConnectorTest, ExecuteStatementFails)
{
    level2::statement_handle handle = {&value_a};
    std::string const sql = "XXX";

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_execute_statement(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.execute_statement(handle, sql), cpp_odbc::error );
}

TEST(Level1ConnectorTest, FetchScrollCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLSMALLINT const orientation = 42;
    SQLLEN const offset = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_fetch_scroll(handle.handle, orientation, offset))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    EXPECT_TRUE(connector.fetch_scroll(handle, orientation, offset));
}

TEST(Level1ConnectorTest, FetchScrollFails)
{
    level2::statement_handle handle = {&value_a};
    SQLSMALLINT const orientation = 42;
    SQLLEN const offset = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_fetch_scroll(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.fetch_scroll(handle, orientation, offset), cpp_odbc::error );
}

TEST(Level1ConnectorTest, FetchScrollHasNoMoreData)
{
    level2::statement_handle handle = {&value_a};
    SQLSMALLINT const orientation = 42;
    SQLLEN const offset = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_fetch_scroll(handle.handle, orientation, offset))
        .WillOnce(testing::Return(SQL_NO_DATA));

    level1_connector const connector(api);
    EXPECT_FALSE(connector.fetch_scroll(handle, orientation, offset));
}

TEST(Level1ConnectorTest, FreeStatementCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const option = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_free_statement(handle.handle, option))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.free_statement(handle, option);
}

TEST(Level1ConnectorTest, FreeStatementFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const option = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_free_statement(testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.free_statement(handle, option), cpp_odbc::error );
}

TEST(Level1ConnectorTest, GetIntegerColumnAttributeCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 23;
    intptr_t const expected = 12345;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_column_attribute(handle.handle, column_id, field_identifier, nullptr, 0, nullptr, testing::_))
        .WillOnce(testing::DoAll(
                    testing::SetArgPointee<6>(expected),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.get_integer_column_attribute(handle, column_id, field_identifier) );
}

TEST(Level1ConnectorTest, GetIntegerColumnAttributeFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_column_attribute(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.get_integer_column_attribute(handle, column_id, field_identifier), cpp_odbc::error );
}

TEST(Level1ConnectorTest, GetIntegerStatementAttributeCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 23;
    intptr_t const expected = 12345;

    auto copy_long_to_void_pointer = [&expected](testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused) {
        *reinterpret_cast<intptr_t *>(destination) = expected;
    };

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_statement_attribute(handle.handle, attribute, testing::_, 0, nullptr))
        .WillOnce(testing::DoAll(
                    testing::Invoke(copy_long_to_void_pointer),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.get_integer_statement_attribute(handle, attribute) );
}

TEST(Level1ConnectorTest, GetIntegerStatementAttributeFails)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_statement_attribute(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.get_integer_statement_attribute(handle, attribute), cpp_odbc::error );
}

TEST(Level1ConnectorTest, GetStringColumnAttributeCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 23;
    std::string const expected = "value";

    auto copy_string_to_void_pointer = [&expected](testing::Unused, testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused, testing::Unused) {
        memcpy(destination, expected.data(), expected.size());
    };

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_column_attribute(handle.handle, column_id, field_identifier, testing::_, 1024, testing::_, nullptr))
        .WillOnce(testing::DoAll(
                    testing::Invoke(copy_string_to_void_pointer),
                    testing::SetArgPointee<5>(expected.size()),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.get_string_column_attribute(handle, column_id, field_identifier) );
}

TEST(Level1ConnectorTest, GetStringColumnAttributeFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_column_attribute(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.get_string_column_attribute(handle, column_id, field_identifier), cpp_odbc::error );
}

TEST(Level1ConnectorTest, NumberOfResultColumnsCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    short int const expected = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_number_of_result_columns(handle.handle, testing::_))
        .WillOnce(testing::DoAll(
                    testing::SetArgPointee<1>(expected),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.number_of_result_columns(handle) );
}

TEST(Level1ConnectorTest, NumberOfResultColumnsFails)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_number_of_result_columns(testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.number_of_result_columns(handle), cpp_odbc::error );
}

TEST(Level1ConnectorTest, NumberOfParametersCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    short int const expected = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_number_of_parameters(handle.handle, testing::_))
        .WillOnce(testing::DoAll(
                    testing::SetArgPointee<1>(expected),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.number_of_parameters(handle) );
}

TEST(Level1ConnectorTest, NumberOfParametersFails)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_number_of_parameters(testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.number_of_parameters(handle), cpp_odbc::error );
}

TEST(Level1ConnectorTest, PrepareStatementCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    std::string const sql = "XXX";

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_prepare_statement(handle.handle, testing::Matcher<unsigned char *>(testing::Truly(matches_string(sql))), sql.size()))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.prepare_statement(handle, sql);
}

TEST(Level1ConnectorTest, PrepareStatementFails)
{
    level2::statement_handle handle = {&value_a};
    std::string const sql = "XXX";

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_prepare_statement(testing::_, testing::Matcher<unsigned char *>(testing::_), testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.prepare_statement(handle, sql), cpp_odbc::error );
}

TEST(Level1ConnectorTest, PrepareWideStatementCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    std::u16string const sql(u"XXX");

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_prepare_statement(handle.handle, testing::Matcher<SQLWCHAR *>(testing::Truly(matches_u16string(sql))), sql.size()))
    .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.prepare_statement(handle, sql);
}

TEST(Level1ConnectorTest, PrepareWideStatementFails)
{
    level2::statement_handle handle = {&value_a};
    std::u16string const sql(u"XXX");

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_prepare_statement(testing::_, testing::Matcher<SQLWCHAR *>(testing::_), testing::_))
    .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.prepare_statement(handle, sql), cpp_odbc::error );
}

TEST(Level1ConnectorTest, SetLongStatementAttributeCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 42;
    intptr_t const value = 23;

    auto matches_pointer_as_value = [&value](void * pointer) {
        return reinterpret_cast<intptr_t>(pointer) == value;
    };

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_statement_attribute(handle.handle, attribute, testing::Truly(matches_pointer_as_value), SQL_IS_INTEGER))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.set_statement_attribute(handle, attribute, value);
}

TEST(Level1ConnectorTest, SetLongStatementAttributeFails)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 42;
    intptr_t const value = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_statement_attribute(testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.set_statement_attribute(handle, attribute, value), cpp_odbc::error );
}

TEST(Level1ConnectorTest, SetPointerStatementAttributeCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 42;
    SQLULEN value = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_statement_attribute(handle.handle, attribute, &value, SQL_IS_POINTER))
        .WillOnce(testing::Return(SQL_SUCCESS));

    level1_connector const connector(api);
    connector.set_statement_attribute(handle, attribute, &value);
}

TEST(Level1ConnectorTest, SetPointerStatementAttributeFails)
{
    level2::statement_handle handle = {&value_a};
    SQLINTEGER const attribute = 42;
    SQLULEN value = 23;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_set_statement_attribute(testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.set_statement_attribute(handle, attribute, &value), cpp_odbc::error );
}

TEST(Level1ConnectorTest, RowCountCallsAPI)
{
    level2::statement_handle handle = {&value_a};
    SQLLEN const expected = 42;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_row_count(handle.handle, testing::_))
        .WillOnce(testing::DoAll(
                    testing::SetArgPointee<1>(expected),
                    testing::Return(SQL_SUCCESS)
                ));

    level1_connector const connector(api);
    EXPECT_EQ( expected, connector.row_count(handle) );
}

TEST(Level1ConnectorTest, RowCountFails)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_row_count(testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.row_count(handle), cpp_odbc::error );
}

namespace {

    void test_describe_column(bool expected_nullable, SQLSMALLINT sql_nullable, std::string const & message)
    {
        level2::statement_handle handle = {&value_a};
        SQLUSMALLINT const column_id = 17;

        cpp_odbc::column_description const expected = {"value", 123, 456, 666, expected_nullable};

        auto copy_string_to_void_pointer = [&expected](testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused, testing::Unused, testing::Unused, testing::Unused, testing::Unused) {
            memcpy(destination, expected.name.data(), expected.name.size());
        };

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_describe_column(handle.handle, column_id, testing::A<SQLCHAR *>(), 256, testing::_, testing::_, testing::_, testing::_, testing::_))
            .WillOnce(testing::DoAll(
                        testing::Invoke(copy_string_to_void_pointer),
                        testing::SetArgPointee<4>(expected.name.size()),
                        testing::SetArgPointee<5>(expected.data_type),
                        testing::SetArgPointee<6>(expected.size),
                        testing::SetArgPointee<7>(expected.decimal_digits),
                        testing::SetArgPointee<8>(sql_nullable),
                        testing::Return(SQL_SUCCESS)
                    ));

        level1_connector const connector(api);
        EXPECT_EQ(expected, connector.describe_column(handle, column_id)) << message;
    }

}

TEST(Level1ConnectorTest, DescribeColumnCallsAPI)
{
    test_describe_column(true, SQL_NULLABLE, "SQL_NULLABLE");
    test_describe_column(false, SQL_NO_NULLS, "SQL_NO_NULLS");
    test_describe_column(true, SQL_NULLABLE_UNKNOWN, "SQL_NULLABLE_UNKNOWN");
}

TEST(Level1ConnectorTest, DescribeColumnFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_describe_column(testing::_, testing::_, testing::A<SQLCHAR *>(), testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.describe_column(handle, column_id), cpp_odbc::error );
}


namespace {

    void test_describe_column_wide(bool expected_nullable, SQLSMALLINT sql_nullable, std::string const & message)
    {
        level2::statement_handle handle = {&value_a};
        SQLUSMALLINT const column_id = 17;

        std::u16string const unicode_name(u"I \u2665 Unicode");
        cpp_odbc::column_description const expected = {u8"I \u2665 Unicode", 123, 456, 666, expected_nullable};

        auto copy_unicode_to_void_pointer = [&unicode_name](testing::Unused, testing::Unused, void * destination, testing::Unused, testing::Unused, testing::Unused, testing::Unused, testing::Unused, testing::Unused) {
            memcpy(destination, unicode_name.data(), unicode_name.size() * 2);
        };

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_describe_column(handle.handle, column_id, testing::A<SQLWCHAR *>(), 256, testing::_, testing::_, testing::_, testing::_, testing::_))
            .WillOnce(testing::DoAll(
                testing::Invoke(copy_unicode_to_void_pointer),
                testing::SetArgPointee<4>(unicode_name.size()),
                testing::SetArgPointee<5>(expected.data_type),
                testing::SetArgPointee<6>(expected.size),
                testing::SetArgPointee<7>(expected.decimal_digits),
                testing::SetArgPointee<8>(sql_nullable),
                testing::Return(SQL_SUCCESS)
            ));

        level1_connector const connector(api);
        EXPECT_EQ(expected, connector.describe_column_wide(handle, column_id)) << message;
    }

}

TEST(Level1ConnectorTest, DescribeColumnWideCallsAPI)
{
    test_describe_column_wide(true, SQL_NULLABLE, "SQL_NULLABLE");
    test_describe_column_wide(false, SQL_NO_NULLS, "SQL_NO_NULLS");
    test_describe_column_wide(true, SQL_NULLABLE_UNKNOWN, "SQL_NULLABLE_UNKNOWN");
}

TEST(Level1ConnectorTest, DescribeColumnWideFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const column_id = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_describe_column(testing::_, testing::_, testing::A<SQLWCHAR *>(), testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.describe_column_wide(handle, column_id), cpp_odbc::error );
}


namespace {

    void test_describe_parameter(bool expected_nullable, SQLSMALLINT sql_nullable, std::string const & message)
    {
        level2::statement_handle handle = {&value_a};
        SQLUSMALLINT const parameter_id = 17;

        cpp_odbc::column_description const expected = {"parameter_17", 123, 456, 666, expected_nullable};

        auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
        EXPECT_CALL(*api, do_describe_parameter(handle.handle, parameter_id, testing::_, testing::_, testing::_, testing::_))
            .WillOnce(testing::DoAll(
                        testing::SetArgPointee<2>(expected.data_type),
                        testing::SetArgPointee<3>(expected.size),
                        testing::SetArgPointee<4>(expected.decimal_digits),
                        testing::SetArgPointee<5>(sql_nullable),
                        testing::Return(SQL_SUCCESS)
                    ));

        level1_connector const connector(api);
        EXPECT_EQ(expected, connector.describe_parameter(handle, parameter_id)) << message;
    }

}

TEST(Level1ConnectorTest, DescribeParameterCallsAPI)
{
    test_describe_parameter(true, SQL_NULLABLE, "SQL_NULLABLE");
    test_describe_parameter(false, SQL_NO_NULLS, "SQL_NO_NULLS");
    test_describe_parameter(true, SQL_NULLABLE_UNKNOWN, "SQL_NULLABLE_UNKNOWN");
}

TEST(Level1ConnectorTest, DescribeParameterFails)
{
    level2::statement_handle handle = {&value_a};
    SQLUSMALLINT const parameter_id = 17;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_describe_parameter(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.describe_parameter(handle, parameter_id), cpp_odbc::error );
}

TEST(Level1ConnectorTest, MoreResultsCallsAPI)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_more_results(handle.handle))
        .WillOnce(testing::Return(SQL_SUCCESS))
        .WillOnce(testing::Return(SQL_SUCCESS))
        .WillOnce(testing::Return(SQL_NO_DATA));

    level1_connector const connector(api);
    EXPECT_TRUE( connector.more_results(handle) );
    EXPECT_TRUE( connector.more_results(handle) );
    EXPECT_FALSE( connector.more_results(handle) );
}

TEST(Level1ConnectorTest, MoreResultsFails)
{
    level2::statement_handle handle = {&value_a};

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_more_results(testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW( connector.more_results(handle), cpp_odbc::error);
}


TEST(Level1ConnectorTest, SupportsFunctionCallsAPI)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const function_id = 5;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_functions(handle.handle, function_id, testing::_))
        .WillOnce(testing::DoAll(
            testing::SetArgPointee<2>(SQL_TRUE),
            testing::Return(SQL_SUCCESS)))
        .WillOnce(testing::DoAll(
            testing::SetArgPointee<2>(SQL_FALSE),
            testing::Return(SQL_SUCCESS)));

    level1_connector const connector(api);
    EXPECT_TRUE(connector.supports_function(handle, function_id));
    EXPECT_FALSE(connector.supports_function(handle, function_id));
}

TEST(Level1ConnectorTest, SupportsFunctionFails)
{
    level2::connection_handle handle = {&value_a};
    SQLUSMALLINT const function_id = 5;

    auto api = std::make_shared<cpp_odbc_test::level1_mock_api const>();
    EXPECT_CALL(*api, do_get_functions(handle.handle, function_id, testing::_))
        .WillOnce(testing::Return(SQL_ERROR));
    expect_diagnostic_record(*api, expected_error);

    level1_connector const connector(api);
    EXPECT_THROW(connector.supports_function(handle, function_id), cpp_odbc::error);
}
