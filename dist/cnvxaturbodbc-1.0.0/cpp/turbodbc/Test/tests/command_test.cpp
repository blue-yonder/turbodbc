#include <turbodbc/command.h>

#include <turbodbc/result_sets/bound_result_set.h>
#include <turbodbc/result_sets/double_buffered_result_set.h>

#include <gtest/gtest.h>
#include <cpp_odbc/connection.h>

#include "mock_classes.h"
#include <sqlext.h>


using turbodbc_test::mock_connection;
using turbodbc_test::mock_statement;


namespace {

    bool const disable_async_io = false;
    bool const enable_async_io = true;

    turbodbc::configuration make_config() {
        turbodbc::options options;
        options.read_buffer_size = turbodbc::rows(1);
        return {options, turbodbc::capabilities(*std::make_shared<mock_connection>())};
    }

    turbodbc::configuration make_config_with_async_io(bool use_async_io) {
        auto configuration = make_config();
        configuration.options.use_async_io = use_async_io;
        return configuration;
    }

}

TEST(CommandTest, GetRowCountBeforeExecuted)
{
    auto statement = std::make_shared<mock_statement>();

    turbodbc::command command(statement, make_config());
    EXPECT_EQ(0, command.get_row_count());
}

namespace {

    void prepare_single_column_result_set(mock_statement & statement)
    {
        ON_CALL( statement, do_number_of_columns())
                .WillByDefault(testing::Return(1));
        ON_CALL( statement, do_describe_column(testing::_))
                .WillByDefault(testing::Return(cpp_odbc::column_description{"", SQL_BIGINT, 8, 0, false}));
    }

}

TEST(CommandTest, GetRowCountAfterQueryWithResultSet)
{
    int64_t const expected = 17;
    auto statement = std::make_shared<mock_statement>();

    prepare_single_column_result_set(*statement);
    EXPECT_CALL( *statement, do_row_count())
            .WillOnce(testing::Return(expected));

    turbodbc::command command(statement, make_config());
    command.execute();
    EXPECT_EQ(expected, command.get_row_count());
}


TEST(CommandTest, GetRowCountAfterInsertWithoutParameters)
{
    int64_t const expected = 17;
    auto statement = std::make_shared<mock_statement>();

    ON_CALL(*statement, do_number_of_parameters())
        .WillByDefault(testing::Return(0));

    EXPECT_CALL( *statement, do_row_count())
        .WillOnce(testing::Return(expected));

    turbodbc::command command(statement, make_config());
    command.execute();
    EXPECT_EQ(expected, command.get_row_count());
}


TEST(CommandTest, GetRowCountAfterInsertWithParameters)
{
    auto statement = std::make_shared<mock_statement>();

    ON_CALL(*statement, do_number_of_parameters())
        .WillByDefault(testing::Return(1));
    ON_CALL(*statement, do_describe_parameter(1))
        .WillByDefault(testing::Return(cpp_odbc::column_description{"dummy", SQL_BIGINT, 0, 0, true}));

    EXPECT_CALL( *statement, do_row_count()).Times(0);

    turbodbc::command command(statement, make_config());
    command.execute(); // execute without setting any parameters before equals 0 transferred rows
    EXPECT_EQ(0, command.get_row_count());
}


namespace {

    template <typename ExpectedResultSetType>
    void test_async_io(bool double_buffering)
    {
        auto statement = std::make_shared<mock_statement>();
        prepare_single_column_result_set(*statement);

        turbodbc::command command(statement, make_config_with_async_io(double_buffering));
        command.execute();
        EXPECT_TRUE(std::dynamic_pointer_cast<ExpectedResultSetType>(command.get_results()));
    }

}

TEST(CommandTest, UseAsyncIOAffectsResultSet)
{
    test_async_io<turbodbc::result_sets::bound_result_set>(disable_async_io);
    test_async_io<turbodbc::result_sets::double_buffered_result_set>(enable_async_io);
}

TEST(CommandTest, GetParameters)
{
    auto statement = std::make_shared<mock_statement>();

    turbodbc::command command(statement, make_config());
    EXPECT_EQ(0, command.get_parameters().number_of_parameters());
}
