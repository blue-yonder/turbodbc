#include "cpp_odbc/statement.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "cpp_odbc_test/mock_statement.h"


using cpp_odbc_test::mock_statement;


TEST(StatementTest, GetIntegerAttributeForwards)
{
    SQLINTEGER const attribute = 23;
    intptr_t const expected = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_get_integer_attribute(attribute))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.get_integer_attribute(attribute));
}

TEST(StatementTest, SetIntegerAttributeForwards)
{
    SQLINTEGER const attribute = 23;
    intptr_t const value = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_set_attribute(attribute, value)).Times(1);

    statement.set_attribute(attribute, value);
}

TEST(StatementTest, SetPointerAttributeForwards)
{
    SQLINTEGER const attribute = 23;
    SQLULEN value = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_set_attribute(attribute, &value)).Times(1);

    statement.set_attribute(attribute, &value);
}

TEST(StatementTest, ExecuteForwards)
{
    std::string const query = "SELECT * FROM dummy";

    mock_statement statement;
    EXPECT_CALL( statement, do_execute(query)).Times(1);

    statement.execute(query);
}

TEST(StatementTest, PrepareForwards)
{
    std::string const query = "SELECT * FROM dummy";

    mock_statement statement;
    EXPECT_CALL( statement, do_prepare(query)).Times(1);

    statement.prepare(query);
}

TEST(StatementTest, PrepareWideForwards)
{
    std::u16string const query(u"SELECT * FROM dummy");

    mock_statement statement;
    EXPECT_CALL(statement, do_prepare(query)).Times(1);

    statement.prepare(query);
}

TEST(StatementTest, BindInputParameterForwards)
{
    SQLUSMALLINT const parameter = 17;
    SQLSMALLINT const value_type = 23;
    SQLSMALLINT const parameter_type = 42;
    SQLSMALLINT const digits = 5;
    cpp_odbc::multi_value_buffer values(2,3);

    mock_statement statement;
    EXPECT_CALL( statement, do_bind_input_parameter(parameter, value_type, parameter_type, digits, testing::Ref(values))).Times(1);

    statement.bind_input_parameter(parameter, value_type, parameter_type, digits, values);
}

TEST(StatementTest, UnbindAllParametersForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_unbind_all_parameters() ).Times(1);

    statement.unbind_all_parameters();
}


TEST(StatementTest, ExecutePreparedForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_execute_prepared()).Times(1);

    statement.execute_prepared();
}

TEST(StatementTest, NumberOfColumnsForwards)
{
    short int const expected = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_number_of_columns())
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.number_of_columns() );
}

TEST(StatementTest, NumberOfParametersForwards)
{
    short int const expected = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_number_of_parameters())
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.number_of_parameters() );
}

TEST(StatementTest, BindColumnForwards)
{
    SQLUSMALLINT const column = 17;
    SQLSMALLINT const column_type = 23;
    cpp_odbc::multi_value_buffer values(2,3);

    mock_statement statement;
    EXPECT_CALL( statement, do_bind_column(column, column_type, testing::Ref(values))).Times(1);

    statement.bind_column(column, column_type, values);
}

TEST(StatementTest, UnbindAllColumnsForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_unbind_all_columns() ).Times(1);

    statement.unbind_all_columns();
}

TEST(StatementTest, FetchNextForwards)
{
    bool const expected = false;
    mock_statement statement;
    EXPECT_CALL( statement, do_fetch_next()).WillOnce(testing::Return(expected));

    EXPECT_EQ(expected, statement.fetch_next());
}

TEST(StatementTest, CloseCursorForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_close_cursor()).Times(1);

    statement.close_cursor();
}

TEST(StatementTest, GetIntegerColumnAttributeForwards)
{
    SQLUSMALLINT const column = 23;
    SQLUSMALLINT const field_identifier = 42;
    intptr_t const expected = 17;

    mock_statement statement;
    EXPECT_CALL( statement, do_get_integer_column_attribute(column, field_identifier))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.get_integer_column_attribute(column, field_identifier));
}

TEST(StatementTest, GetStringColumnAttributeForwards)
{
    SQLUSMALLINT const column = 23;
    SQLUSMALLINT const field_identifier = 42;
    std::string const expected = "test value";

    mock_statement statement;
    EXPECT_CALL( statement, do_get_string_column_attribute(column, field_identifier))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.get_string_column_attribute(column, field_identifier));
}

TEST(StatementTest, RowCountForwards)
{
    SQLLEN const expected = 42;

    mock_statement statement;
    EXPECT_CALL( statement, do_row_count())
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.row_count() );
}

TEST(StatementTest, DescribeColumnForwards)
{
    SQLUSMALLINT const column_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    mock_statement statement;
    EXPECT_CALL( statement, do_describe_column(column_id))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.describe_column(column_id) );
}

TEST(StatementTest, DescribeColumnWideForwards)
{
    SQLUSMALLINT const column_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};
    
    mock_statement statement;
    EXPECT_CALL( statement, do_describe_column_wide(column_id))
        .WillOnce(testing::Return(expected));
    
    EXPECT_EQ( expected, statement.describe_column_wide(column_id) );
}

TEST(StatementTest, DescribeParameterForwards)
{
    SQLUSMALLINT const parameter_id = 23;
    cpp_odbc::column_description const expected = {"dummy", 1, 2, 3, false};

    mock_statement statement;
    EXPECT_CALL( statement, do_describe_parameter(parameter_id))
        .WillOnce(testing::Return(expected));

    EXPECT_EQ( expected, statement.describe_parameter(parameter_id) );
}

TEST(StatementTest, MoreResultsForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_more_results()).WillOnce(testing::Return(false));

    EXPECT_FALSE(statement.more_results());
}

TEST(StatementTest, FinalizeForwards)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_finalize() ).Times(1);

    statement.finalize();
}


TEST(StatementTest, FinalizeForwardsOnlyOnce)
{
    mock_statement statement;
    EXPECT_CALL( statement, do_finalize() ).Times(1);

    // do_finalize should only be called once, even if finalize is called twice
    statement.finalize();
    statement.finalize();
}
