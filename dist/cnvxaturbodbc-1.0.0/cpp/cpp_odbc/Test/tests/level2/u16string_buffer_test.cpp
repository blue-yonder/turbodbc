#include "cpp_odbc/level2/u16string_buffer.h"

#include <gtest/gtest.h>

#include <cstring>


TEST(U16StringBufferTest, Capacity)
{
    signed short int const expected_capacity = 1000;
    cpp_odbc::level2::u16string_buffer buffer(expected_capacity);

    EXPECT_EQ( expected_capacity, buffer.capacity() );
}

TEST(U16StringBufferTest, StringCast)
{
    cpp_odbc::level2::u16string_buffer buffer(1000);

    std::u16string const expected = u"test message";

    memcpy(buffer.data_pointer(), expected.c_str(), expected.size() * 2);
    *buffer.size_pointer() = expected.size();

    std::u16string const actual(buffer);

    EXPECT_EQ( expected, actual );
}
