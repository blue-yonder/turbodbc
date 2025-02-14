#include "cpp_odbc/level2/string_buffer.h"

#include <gtest/gtest.h>

#include <cstring>


TEST(StringBufferTest, Capacity)
{
    signed short int const expected_capacity = 1000;
    cpp_odbc::level2::string_buffer buffer(expected_capacity);

    EXPECT_EQ( expected_capacity, buffer.capacity() );
}

TEST(StringBufferTest, StringCast)
{
    cpp_odbc::level2::string_buffer buffer(1000);

    std::string const expected = "test message";

    memcpy(buffer.data_pointer(), expected.c_str(), expected.size());
    *buffer.size_pointer() = expected.size();

    std::string const actual(buffer);

    EXPECT_EQ( expected, actual );
}
