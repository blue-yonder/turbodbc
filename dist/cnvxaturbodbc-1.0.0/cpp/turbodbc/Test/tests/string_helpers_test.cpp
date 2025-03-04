#include <turbodbc/string_helpers.h>

#include <gtest/gtest.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sqlext.h>


using turbodbc::buffered_string_size;

TEST(StringHelpersTest, BufferedStringSizeForSmallerString)
{
    std::intptr_t const reported_string_size = 10;
    std::size_t const maximum_string_size = 42;
    EXPECT_EQ(10, buffered_string_size(reported_string_size, maximum_string_size));
}


TEST(StringHelpersTest, BufferedStringSizeForFittingString)
{
    std::intptr_t const reported_string_size = 42;
    std::size_t const maximum_string_size = 42;
    EXPECT_EQ(42, buffered_string_size(reported_string_size, maximum_string_size));
}


TEST(StringHelpersTest, BufferedStringSizeForOversizedString)
{
    std::intptr_t const reported_string_size = 43;
    std::size_t const maximum_string_size = 42;
    EXPECT_EQ(42, buffered_string_size(reported_string_size, maximum_string_size));
}


TEST(StringHelpersTest, BufferedStringSizeForSQLNoTotal)
{
    std::intptr_t const reported_string_size = SQL_NO_TOTAL;
    std::size_t const maximum_string_size = 42;
    EXPECT_EQ(42, buffered_string_size(reported_string_size, maximum_string_size));
}