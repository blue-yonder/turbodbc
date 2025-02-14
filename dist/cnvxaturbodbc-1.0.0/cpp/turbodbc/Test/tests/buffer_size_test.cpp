#include <turbodbc/buffer_size.h>
#include <turbodbc/descriptions/integer_description.h>

#include <boost/variant.hpp>
#include <gtest/gtest.h>

using turbodbc::description;
using turbodbc::integer_description;


TEST(BufferSizeTest, RowsIsDefaultConstructible)
{
    turbodbc::rows one_row;
    EXPECT_EQ(1, one_row.value);
}

TEST(BufferSizeTest, RowsConstructor)
{
    turbodbc::rows r(42);
    EXPECT_EQ(42, r.value);
}

TEST(BufferSizeTest, MegabytesConstructor)
{
    turbodbc::megabytes mb(42);
    EXPECT_EQ(42, mb.value);
}

TEST(BufferSizeTest, BufferSizeIsDefaultConstructible)
{
    turbodbc::buffer_size one_row;
    EXPECT_EQ(1, boost::get<turbodbc::rows>(one_row).value);
}

TEST(BufferSizeTest, DetermineBufferSizeWithRows)
{
    turbodbc::buffer_size ten_rows(turbodbc::rows(10));
    std::vector<std::unique_ptr<turbodbc::description const>> descriptions;

    std::size_t rows_to_buffer = boost::apply_visitor(turbodbc::determine_rows_to_buffer(descriptions), ten_rows);
    EXPECT_EQ(10, rows_to_buffer);
}

TEST(BufferSizeTest, DetermineBufferSizeWithZeroRows)
{
    turbodbc::buffer_size ten_rows(turbodbc::rows(0));
    std::vector<std::unique_ptr<turbodbc::description const>> descriptions;

    std::size_t rows_to_buffer = boost::apply_visitor(turbodbc::determine_rows_to_buffer(descriptions), ten_rows);
    EXPECT_EQ(1, rows_to_buffer);
}


TEST(BufferSizeTest, DetermineBufferSizeWithMegabytes)
{
    turbodbc::buffer_size ten_mb(turbodbc::megabytes(10));
    std::vector<std::unique_ptr<turbodbc::description const>> descriptions;
    descriptions.push_back(std::unique_ptr<description>(new integer_description("", true)));
    descriptions.push_back(std::unique_ptr<description>(new integer_description("", true)));

    std::size_t rows_to_buffer = boost::apply_visitor(turbodbc::determine_rows_to_buffer(descriptions), ten_mb);
    EXPECT_EQ(10 * 1024 * 1024 / (8 + 8), rows_to_buffer);
}

TEST(BufferSizeTest, DetermineBufferSizeWithMegabytesWhereRowLargerThanDesiredBuffer)
{
    std::vector<std::unique_ptr<turbodbc::description const>> descriptions;
    std::size_t const columns_for_row_larger_than_one_mb = 200000;
    for (std::size_t i = 0; i != columns_for_row_larger_than_one_mb; ++i) {
        descriptions.push_back(std::unique_ptr<description>(new integer_description("", true)));
    }

    turbodbc::buffer_size one_mb(turbodbc::megabytes(1));
    std::size_t rows_to_buffer = boost::apply_visitor(turbodbc::determine_rows_to_buffer(descriptions), one_mb);
    EXPECT_EQ(1, rows_to_buffer);
}

TEST(BufferSizeTest, HalveBufferSizeWithRows)
{
    turbodbc::buffer_size odd(turbodbc::rows(31));
    turbodbc::buffer_size even(turbodbc::rows(42));

    EXPECT_EQ(15 + 1, boost::get<turbodbc::rows>(boost::apply_visitor(turbodbc::halve_buffer_size(), odd)).value);
    EXPECT_EQ(21, boost::get<turbodbc::rows>(boost::apply_visitor(turbodbc::halve_buffer_size(), even)).value);
}

TEST(BufferSizeTest, HalveBufferSizeWithMegabytes)
{
    turbodbc::buffer_size odd(turbodbc::megabytes(31));
    turbodbc::buffer_size even(turbodbc::megabytes(42));

    EXPECT_EQ(15 + 1, boost::get<turbodbc::megabytes>(boost::apply_visitor(turbodbc::halve_buffer_size(), odd)).value);
    EXPECT_EQ(21, boost::get<turbodbc::megabytes>(boost::apply_visitor(turbodbc::halve_buffer_size(), even)).value);
}