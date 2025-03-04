#include "cpp_odbc/level3/raii_statement.h"

#include <gtest/gtest.h>

#include "cpp_odbc/level3/raii_environment.h"
#include "cpp_odbc/level3/raii_connection.h"
#include "cpp_odbc/error.h"
#include "cpp_odbc_test/level2_mock_api.h"

#include <type_traits>
#include <ciso646>


using cpp_odbc::level3::raii_connection;
using cpp_odbc::level3::raii_environment;
using cpp_odbc::level3::raii_statement;
using cpp_odbc_test::level2_mock_api;
using cpp_odbc::level2::environment_handle;
using cpp_odbc::level2::connection_handle;
using cpp_odbc::level2::statement_handle;

namespace {

    // destinations for pointers, values irrelevant
    int value_a = 17;
    int value_b = 23;
    int value_c = 17;

    environment_handle const default_e_handle = {&value_a};
    connection_handle const default_c_handle = {&value_b};
    statement_handle const default_s_handle = {&value_c};

    std::shared_ptr<testing::NiceMock<level2_mock_api> const> make_default_api()
    {
        auto api = std::make_shared<testing::NiceMock<level2_mock_api>>();

        ON_CALL(*api, do_allocate_environment_handle())
            .WillByDefault(testing::Return(default_e_handle));
        ON_CALL(*api, do_allocate_connection_handle(testing::_))
            .WillByDefault(testing::Return(default_c_handle));
        ON_CALL(*api, do_allocate_statement_handle(testing::_))
            .WillByDefault(testing::Return(default_s_handle));

        return api;
    }

}

TEST(RaiiStatementTest, IsStatement)
{
    bool const derived_from_statement = std::is_base_of<cpp_odbc::statement, raii_statement>::value;
    EXPECT_TRUE( derived_from_statement );
}

TEST(RaiiStatementTest, ResourceManagement)
{
    auto api = std::make_shared<testing::NiceMock<level2_mock_api> const>();
    ON_CALL(*api, do_allocate_environment_handle())
        .WillByDefault(testing::Return(default_e_handle));
    ON_CALL(*api, do_allocate_connection_handle(testing::_))
        .WillByDefault(testing::Return(default_c_handle));

    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");

    statement_handle s_handle = {&value_b};

    EXPECT_CALL(*api, do_allocate_statement_handle(default_c_handle))
        .WillOnce(testing::Return(s_handle));

    {
        raii_statement statement(connection);

        // free handle on destruction
        EXPECT_CALL(*api, do_free_handle(s_handle)).Times(1);
    }
}

TEST(RaiiStatementTest, DestructorHandlesFreeHandleErrors)
{
    auto api = make_default_api();

    statement_handle s_handle = {&value_b};
    EXPECT_CALL(*api, do_allocate_statement_handle(default_c_handle))
        .WillOnce(testing::Return(s_handle));

    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");

    ON_CALL(*api, do_free_handle(s_handle))
        .WillByDefault(testing::Throw(cpp_odbc::error("")));


    raii_statement statement(connection);
}

TEST(RaiiStatementTest, KeepsConnectionAlive)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");

    auto const use_count_before = connection.use_count();
    raii_statement statement(connection);
    auto const use_count_after = connection.use_count();

    EXPECT_EQ(1, use_count_after - use_count_before);
}

TEST(RaiiStatementTest, GetIntegerAttribute)
{
    SQLINTEGER const attribute = 42;
    intptr_t const expected = 12345;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_get_integer_statement_attribute(default_s_handle, attribute))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.get_integer_attribute(attribute));
}

TEST(RaiiStatementTest, SetIntegerAttribute)
{
    SQLINTEGER const attribute = 42;
    intptr_t const value = 12345;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_set_statement_attribute(default_s_handle, attribute, value)).Times(1);

    raii_statement statement(connection);
    statement.set_attribute(attribute, value);
}

TEST(RaiiStatementTest, SetPointerAttribute)
{
    SQLINTEGER const attribute = 42;
    SQLULEN value = 12345;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_set_statement_attribute(default_s_handle, attribute, &value)).Times(1);

    raii_statement statement(connection);
    statement.set_attribute(attribute, &value);
}

TEST(RaiiStatementTest, Execute)
{
    std::string const sql = "SELECT dummy FROM test";

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_execute_statement(default_s_handle, sql)).Times(1);

    raii_statement statement(connection);
    statement.execute(sql);
}

TEST(RaiiStatementTest, Prepare)
{
    std::string const sql = "SELECT dummy FROM test";

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_prepare_statement(default_s_handle, sql)).Times(1);

    raii_statement statement(connection);
    statement.prepare(sql);
}

TEST(RaiiStatementTest, PrepareWide)
{
    std::u16string const sql(u"SELECT dummy FROM test");

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_prepare_statement(default_s_handle, sql)).Times(1);

    raii_statement statement(connection);
    statement.prepare(sql);
}

TEST(RaiiStatementTest, BindInputParameter)
{
    SQLUSMALLINT const parameter_id = 17;
    SQLSMALLINT const value_type = 23;
    SQLSMALLINT const parameter_type = 42;
    SQLSMALLINT const digits = 5;
    cpp_odbc::multi_value_buffer parameter_values(3, 4);

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_bind_input_parameter(default_s_handle, parameter_id, value_type, parameter_type, digits, testing::Ref(parameter_values))).Times(1);

    raii_statement statement(connection);
    statement.bind_input_parameter(parameter_id, value_type, parameter_type, digits, parameter_values);
}

TEST(RaiiStatementTest, UnbindAllParameters)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_free_statement(default_s_handle, SQL_RESET_PARAMS)).Times(1);

    raii_statement statement(connection);
    statement.unbind_all_parameters();
}

TEST(RaiiStatementTest, ExecutePrepared)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_execute_prepared_statement(default_s_handle)).Times(1);

    raii_statement statement(connection);
    statement.execute_prepared();
}

TEST(RaiiStatementTest, NumberOfColumns)
{
    short int const expected = 23;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_number_of_result_columns(default_s_handle))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.number_of_columns());
}

TEST(RaiiStatementTest, NumberOfParameters)
{
    short int const expected = 23;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_number_of_parameters(default_s_handle))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.number_of_parameters());
}

TEST(RaiiStatementTest, BindColumn)
{
    SQLUSMALLINT const column_id = 17;
    SQLSMALLINT const column_type = 23;
    cpp_odbc::multi_value_buffer column_buffer(3, 4);

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_bind_column(default_s_handle, column_id, column_type, testing::Ref(column_buffer))).Times(1);

    raii_statement statement(connection);
    statement.bind_column(column_id, column_type, column_buffer);
}

TEST(RaiiStatementTest, UnbindAllColumns)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_free_statement(default_s_handle, SQL_UNBIND)).Times(1);

    raii_statement statement(connection);
    statement.unbind_all_columns();
}

TEST(RaiiStatementTest, FetchNext)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_fetch_scroll(default_s_handle, SQL_FETCH_NEXT, 0)).Times(1);

    raii_statement statement(connection);
    statement.fetch_next();
}

TEST(RaiiStatementTest, CloseCursor)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_free_statement(default_s_handle, SQL_CLOSE)).Times(1);

    raii_statement statement(connection);
    statement.close_cursor();
}

TEST(RaiiStatementTest, GetIntegerColumnAttribute)
{
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 42;
    intptr_t const expected = 23;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_get_integer_column_attribute(default_s_handle, column_id, field_identifier))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.get_integer_column_attribute(column_id, field_identifier));
}

TEST(RaiiStatementTest, GetStringColumnAttribute)
{
    SQLUSMALLINT const column_id = 17;
    SQLUSMALLINT const field_identifier = 42;
    std::string const expected = "test value";

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_get_string_column_attribute(default_s_handle, column_id, field_identifier))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.get_string_column_attribute(column_id, field_identifier));
}

TEST(RaiiStatementTest, RowCount)
{
    SQLLEN const expected = 23;

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_row_count(default_s_handle))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.row_count());
}

TEST(RaiiStatementTest, DescribeColumn)
{
    SQLUSMALLINT const column_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_describe_column(default_s_handle, column_id))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.describe_column(column_id));
}

TEST(RaiiStatementTest, DescribeColumnWide)
{
    SQLUSMALLINT const column_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_describe_column_wide(default_s_handle, column_id))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.describe_column_wide(column_id));
}

TEST(RaiiStatementTest, DescribeParameter)
{
    SQLUSMALLINT const parameter_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_describe_parameter(default_s_handle, parameter_id))
        .WillOnce(testing::Return(expected));

    raii_statement statement(connection);
    EXPECT_EQ( expected, statement.describe_parameter(parameter_id));
}

TEST(RaiiStatementTest, MoreResults)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");
    EXPECT_CALL(*api, do_more_results(default_s_handle)).WillOnce(testing::Return(false));

    raii_statement statement(connection);
    EXPECT_TRUE( not statement.more_results() );
}


TEST(RaiiStatementTest, DoFinalize)
{
    auto api = make_default_api();
    auto environment = std::make_shared<raii_environment>(api);
    auto connection = std::make_shared<raii_connection>(environment, "dummy");

    statement_handle s_handle = {&value_c};

    EXPECT_CALL(*api, do_free_handle(s_handle)).Times(1);

    raii_statement statement(connection);
    statement.finalize();
}
