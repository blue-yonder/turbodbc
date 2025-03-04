#include <turbodbc/result_sets/field_result_set.h>

#include <boost/variant/get.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using turbodbc::result_sets::field_result_set;


struct mock_result_set : public turbodbc::result_sets::result_set
{
    MOCK_METHOD0(do_fetch_next_batch, std::size_t());
    MOCK_CONST_METHOD0(do_get_column_info, std::vector<turbodbc::column_info>());
    MOCK_CONST_METHOD0(do_get_buffers, std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>>());
};


TEST(FieldResultSetTest, GetColumnInfoForwards)
{
    testing::NiceMock<mock_result_set> base;
    std::vector<turbodbc::column_info> const info = {{"my column", turbodbc::type_code::integer, 8, true}};
    ON_CALL(base, do_get_column_info()).WillByDefault(testing::Return(info));

    field_result_set rs(base);
    auto const actual = rs.get_column_info();
    ASSERT_EQ(1, actual.size());
    EXPECT_EQ("my column", actual[0].name);
}

TEST(FieldResultSetTest, FetchRow)
{
    int64_t const expected_int = 42;
    double const expected_float = 3.14;
    cpp_odbc::multi_value_buffer buffer_int(8, 3);
    cpp_odbc::multi_value_buffer buffer_float(8, 3);
    std::vector<turbodbc::column_info> const infos = {{"my column", turbodbc::type_code::integer, 8, true},
                                                      {"my column", turbodbc::type_code::floating_point, 8, true}};

    *reinterpret_cast<intptr_t *>(buffer_int[0].data_pointer) = expected_int;
    buffer_int[0].indicator = 8;
    *reinterpret_cast<double *>(buffer_float[0].data_pointer) = expected_float;
    buffer_int[0].indicator = 8;
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers = {std::cref(buffer_int),
                                                                                       std::cref(buffer_float)};

    testing::NiceMock<mock_result_set> base;
    ON_CALL(base, do_get_buffers()).WillByDefault(testing::Return(buffers));
    ON_CALL(base, do_get_column_info()).WillByDefault(testing::Return(infos));
    field_result_set rs(base);

    EXPECT_CALL(base, do_fetch_next_batch()).WillOnce(testing::Return(1));
    auto const row = rs.fetch_row();
    ASSERT_EQ(2, row.size());
    EXPECT_EQ(expected_int, boost::get<int64_t>(*row[0]));
    EXPECT_EQ(expected_float, boost::get<double>(*row[1]));
}

TEST(FieldResultSetTest, FetchRowEmptyResultSet)
{
    cpp_odbc::multi_value_buffer buffer(8, 3);
    std::vector<turbodbc::column_info> const infos = {{"my column", turbodbc::type_code::integer, 8, true}};

    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers = {std::cref(buffer)};

    testing::NiceMock<mock_result_set> base;
    ON_CALL(base, do_get_buffers()).WillByDefault(testing::Return(buffers));
    ON_CALL(base, do_get_column_info()).WillByDefault(testing::Return(infos));
    field_result_set rs(base);

    EXPECT_CALL(base, do_fetch_next_batch()).WillOnce(testing::Return(0));
    auto const row = rs.fetch_row();
    ASSERT_TRUE(row.empty());
}
