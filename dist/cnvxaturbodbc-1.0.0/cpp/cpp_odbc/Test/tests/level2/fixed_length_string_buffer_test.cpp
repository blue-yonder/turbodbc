#include "cpp_odbc/level2/fixed_length_string_buffer.h"

#include <gtest/gtest.h>

#include <cstring>

using cpp_odbc::level2::fixed_length_string_buffer;

TEST(FixedLengthStringBufferTest, Capacity)
{
	std::size_t const expected = 5;
	fixed_length_string_buffer<expected> const buffer;
	EXPECT_EQ( expected, buffer.capacity() );
}

TEST(FixedLengthStringBufferTest, StringCast)
{
	fixed_length_string_buffer<5> buffer;

	std::string const expected = "dummy";
	memcpy(buffer.data_pointer(), expected.c_str(), expected.size());

	std::string const actual(buffer);
	EXPECT_EQ( expected, actual );
}
