#include <turbodbc/result_sets/result_set.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


struct mock_result_set : public turbodbc::result_sets::result_set
{
    MOCK_METHOD0(do_fetch_next_batch, std::size_t());
    MOCK_CONST_METHOD0(do_get_column_info, std::vector<turbodbc::column_info>());
    MOCK_CONST_METHOD0(do_get_buffers, std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>>());
};


TEST(BaseResultSetTest, FetchNextBatchForwards)
{
    mock_result_set rs;
    EXPECT_CALL(rs, do_fetch_next_batch()).WillOnce(testing::Return(42));
    EXPECT_EQ(42, rs.fetch_next_batch());
}

TEST(BaseResultSetTest, GetColumnInfoForwards)
{
    mock_result_set rs;
    std::vector<turbodbc::column_info> expected = {{"column name",
                                                    turbodbc::type_code::integer,
                                                    10,
                                                    true}};

    EXPECT_CALL(rs, do_get_column_info()).WillOnce(testing::Return(expected));
    EXPECT_EQ(expected[0].name, rs.get_column_info()[0].name);
}

TEST(BaseResultSetTest, GetColumnsForwards)
{
    mock_result_set rs;
    cpp_odbc::multi_value_buffer buffer(10, 10);
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> expected = {buffer};

    EXPECT_CALL(rs, do_get_buffers()).WillOnce(testing::Return(expected));
    EXPECT_EQ(expected[0].get().data_pointer(), rs.get_buffers()[0].get().data_pointer());
}
