#include <turbodbc/connection.h>

#include <gtest/gtest.h>
#include <cpp_odbc/connection.h>
#include "mock_classes.h"

#include <sqlext.h>


using turbodbc_test::mock_connection;
using turbodbc_test::mock_statement;


TEST(ConnectionTest, ConstructorDisablesAutoCommit)
{
    auto connection = std::make_shared<mock_connection>();
    EXPECT_CALL(*connection, do_set_attribute(SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF)).Times(1);

    turbodbc::connection test_connection(connection, turbodbc::options());
}

TEST(ConnectionTest, Commit)
{
    auto connection = std::make_shared<mock_connection>();
    EXPECT_CALL(*connection, do_commit()).Times(1);

    turbodbc::connection test_connection(connection, turbodbc::options());
    test_connection.commit();
}

TEST(ConnectionTest, Rollback)
{
    auto connection = std::make_shared<mock_connection>();
    EXPECT_CALL(*connection, do_rollback()).Times(1);

    turbodbc::connection test_connection(connection, turbodbc::options());
    test_connection.rollback();
}

TEST(ConnectionTest, MakeCursorForwardsSelf)
{
    auto connection = std::make_shared<mock_connection>();

    turbodbc::connection test_connection(connection, turbodbc::options());
    auto cursor = test_connection.make_cursor();
    EXPECT_EQ(connection, cursor.get_connection());
}
