#include <turbodbc/result_sets/bound_result_set.h>

#include <tests/mock_classes.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <type_traits>
#include <memory>

#include <sqlext.h>

using turbodbc::result_sets::bound_result_set;
using turbodbc::column_info;
using turbodbc_test::mock_statement;


namespace {

	bool const prefer_string = false;
	bool const prefer_unicode = true;

	std::shared_ptr<mock_statement> prepare_mock_with_columns(std::vector<SQLSMALLINT> const & column_types)
	{
		auto statement = std::make_shared<mock_statement>();

		ON_CALL(*statement, do_number_of_columns())
			.WillByDefault(testing::Return(column_types.size()));

		for (std::size_t i = 0; i != column_types.size(); ++i) {
			cpp_odbc::column_description const value = {"dummy_name", column_types[i], 42, 17, true};
			ON_CALL(*statement, do_describe_column(i + 1))
				.WillByDefault(testing::Return(value));
		}
		return statement;
	}

	/**
	 * Set the mock statement to expect calls to bind_buffer.
	 */
	void expect_calls_to_bind_buffer(mock_statement & statement, std::vector<SQLSMALLINT> const & expected_bind_types)
	{
		for (std::size_t i = 0; i != expected_bind_types.size(); ++i) {
			EXPECT_CALL(statement, do_bind_column(i + 1, expected_bind_types[i], testing::_));
		}
	}

}


TEST(BoundResultSetTest, IsResultSet)
{
	bool const is_result_set = std::is_base_of<turbodbc::result_sets::result_set, bound_result_set>::value;
	EXPECT_TRUE(is_result_set);
}


TEST(BoundResultSetTest, BindColumnsWithFixedRowsInConstructor)
{
	std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER, SQL_VARCHAR};
	std::vector<SQLSMALLINT> const c_column_types = {SQL_C_SBIGINT, SQL_CHAR};
	std::size_t const buffered_rows = 1234;

	auto statement = prepare_mock_with_columns(sql_column_types);
	expect_calls_to_bind_buffer(*statement, c_column_types);
	EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, buffered_rows));

	bound_result_set rs(statement, turbodbc::rows(buffered_rows), prefer_string);
}

TEST(BoundResultSetTest, BindColumnsWithFixedMegabytesInConstructor)
{
	std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER};
	std::vector<SQLSMALLINT> const c_column_types = {SQL_C_SBIGINT};

    std::size_t const expected_rows = 1024 * 1024 / 8;

	auto statement = prepare_mock_with_columns(sql_column_types);
	expect_calls_to_bind_buffer(*statement, c_column_types);
	EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, expected_rows));

	bound_result_set rs(statement, turbodbc::megabytes(1), prefer_string);
}


namespace {

	/**
	* Change the address of the given target_pointer to point to the second argument of the mocked function
	*/
	ACTION_P(store_length_buffer_address_in, target_pointer) {
		*target_pointer = arg1;
	}

	void expect_rows_fetched_pointer_set(mock_statement & statement, SQLULEN * & rows_fetched)
	{
		EXPECT_CALL(statement, do_set_attribute(SQL_ATTR_ROWS_FETCHED_PTR, testing::An<SQLULEN *>()))
				.WillOnce(store_length_buffer_address_in(&rows_fetched));
	}

}


TEST(BoundResultSetTest, FetchNextBatch)
{
	std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER};
	SQLULEN * rows_fetched = nullptr;
	auto statement = prepare_mock_with_columns(sql_column_types);
	expect_rows_fetched_pointer_set(*statement, rows_fetched);

	bound_result_set rs(statement, turbodbc::rows(1000), prefer_string);
	ASSERT_TRUE(rows_fetched != nullptr);

	*rows_fetched = 123;
	EXPECT_CALL(*statement, do_fetch_next());

	EXPECT_EQ(*rows_fetched, rs.fetch_next_batch());
}


TEST(BoundResultSetTest, GetColumnInfo)
{
	auto statement = prepare_mock_with_columns({SQL_INTEGER, SQL_VARCHAR});

	bound_result_set rs(statement, turbodbc::rows(123), prefer_string);

	ASSERT_EQ(2, rs.get_column_info().size());
	EXPECT_EQ(turbodbc::type_code::integer, rs.get_column_info()[0].type);
	EXPECT_EQ(turbodbc::type_code::string, rs.get_column_info()[1].type);
}


TEST(BoundResultSetTest, GetBuffers)
{
	auto statement = prepare_mock_with_columns({SQL_INTEGER, SQL_VARCHAR});
	std::size_t const buffered_rows = 1234;

	bound_result_set rs(statement, turbodbc::rows(buffered_rows), prefer_string);
	auto const buffers = rs.get_buffers();
	ASSERT_EQ(2, buffers.size());

	// make sure we can read the last elements for both columns
	auto last_of_col_a = buffers[0].get()[buffered_rows - 1];
	auto last_of_col_b = buffers[1].get()[buffered_rows - 1];

	EXPECT_EQ(last_of_col_a.indicator, last_of_col_a.indicator);
	EXPECT_EQ(last_of_col_b.indicator, last_of_col_b.indicator);
}


TEST(BoundResultSetTest, Rebind)
{
	std::vector<SQLSMALLINT> const sql_column_types = {SQL_INTEGER, SQL_VARCHAR};
	std::vector<SQLSMALLINT> const c_column_types = {SQL_C_SBIGINT, SQL_CHAR};
	std::size_t const buffered_rows = 1234;

	auto statement = prepare_mock_with_columns(sql_column_types);
	EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, buffered_rows));

	bound_result_set rs(statement, turbodbc::rows(buffered_rows), prefer_string);

	expect_calls_to_bind_buffer(*statement, c_column_types);
	EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROWS_FETCHED_PTR, testing::An<SQLULEN *>()));
	rs.rebind();
}


TEST(BoundResultSetTest, MoveConstructorRebinds)
{
	auto statement = prepare_mock_with_columns({SQL_INTEGER, SQL_VARCHAR});

	bound_result_set moved(statement, turbodbc::rows(123), prefer_string);

	// rebind includes setting fetched pointer
	EXPECT_CALL(*statement, do_set_attribute(SQL_ATTR_ROWS_FETCHED_PTR, testing::An<SQLULEN *>()));

	bound_result_set rs(std::move(moved));
	ASSERT_EQ(2, rs.get_column_info().size());
	EXPECT_EQ(turbodbc::type_code::integer, rs.get_column_info()[0].type);
	EXPECT_EQ(turbodbc::type_code::string, rs.get_column_info()[1].type);
}


namespace {

    void test_preference(bool prefer_unicode, SQLSMALLINT expected_c_column_type)
    {
        std::vector<SQLSMALLINT> const sql_column_types = {SQL_VARCHAR}; // get character
        std::vector<SQLSMALLINT> const c_column_types = {expected_c_column_type};

        auto statement = prepare_mock_with_columns(sql_column_types);
        expect_calls_to_bind_buffer(*statement, c_column_types);

        bound_result_set rs(statement, turbodbc::rows(1), prefer_unicode);
    }

}

TEST(BoundResultSetTest, ConstructorRespectsStringOrUnicodePreference)
{
    test_preference(prefer_string, SQL_CHAR);
    test_preference(prefer_unicode, SQL_WCHAR);
}
