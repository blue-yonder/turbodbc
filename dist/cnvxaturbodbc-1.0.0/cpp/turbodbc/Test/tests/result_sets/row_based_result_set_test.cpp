#include <turbodbc/result_sets/row_based_result_set.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using turbodbc::result_sets::row_based_result_set;


struct mock_result_set : public turbodbc::result_sets::result_set
{
	MOCK_METHOD0(do_fetch_next_batch, std::size_t());
	MOCK_CONST_METHOD0(do_get_column_info, std::vector<turbodbc::column_info>());
	MOCK_CONST_METHOD0(do_get_buffers, std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>>());
};


TEST(RowBasedResultSetTest, GetColumnInfoForwards)
{
	testing::NiceMock<mock_result_set> base;
	row_based_result_set rs(base);
	std::vector<turbodbc::column_info> const info = {{"my column", turbodbc::type_code::integer, 8, true}};

	ON_CALL(base, do_get_column_info()).WillByDefault(testing::Return(info));
	auto const actual = rs.get_column_info();
	ASSERT_EQ(1, actual.size());
	EXPECT_EQ("my column", actual[0].name);
}

TEST(RowBasedResultSetTest, FetchRowBatchHandling)
{
	cpp_odbc::multi_value_buffer buffer(8, 3);
	std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers = {std::cref(buffer)};

	testing::NiceMock<mock_result_set> base;
	row_based_result_set rs(base);

	// first batch: 3 rows
	EXPECT_CALL(base, do_fetch_next_batch()).WillOnce(testing::Return(3));
	EXPECT_CALL(base, do_get_buffers()).WillOnce(testing::Return(buffers));
	buffer[0].indicator = 0;
	buffer[1].indicator = 1;
	buffer[2].indicator = 2;

	EXPECT_EQ(0, rs.fetch_row().at(0).indicator);
	EXPECT_EQ(1, rs.fetch_row().at(0).indicator);
	EXPECT_EQ(2, rs.fetch_row().at(0).indicator);

	// second batch: 1 row
	EXPECT_CALL(base, do_fetch_next_batch()).WillOnce(testing::Return(1));
	EXPECT_CALL(base, do_get_buffers()).WillOnce(testing::Return(buffers));
	buffer[0].indicator = 3;

	EXPECT_EQ(3, rs.fetch_row().at(0).indicator);

	// third batch: empty
	EXPECT_CALL(base, do_fetch_next_batch()).WillOnce(testing::Return(0));
	EXPECT_CALL(base, do_get_buffers()).WillOnce(testing::Return(buffers));

	EXPECT_TRUE(rs.fetch_row().empty());
}

TEST(RowBasedResultSetTest, FetchRowWithMultipleColumns)
{
	cpp_odbc::multi_value_buffer buffer_a(8, 3);
	cpp_odbc::multi_value_buffer buffer_b(8, 3);
	std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers = {std::cref(buffer_a),
	                                                                                   std::cref(buffer_b)};

	testing::NiceMock<mock_result_set> base;
	ON_CALL(base, do_get_buffers()).WillByDefault(testing::Return(buffers));
	row_based_result_set rs(base);

	// first batch: 3 rows
	buffer_a[0].indicator = 0;
	buffer_b[0].indicator = 42;

	ON_CALL(base, do_fetch_next_batch()).WillByDefault(testing::Return(1));
	auto const row = rs.fetch_row();
	EXPECT_EQ(0, row.at(0).indicator);
	EXPECT_EQ(42, row.at(1).indicator);
}
