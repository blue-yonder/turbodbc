#include "cpp_odbc/level2/input_string_buffer.h"

#include <gtest/gtest.h>


TEST(InputStringBufferTest, CopiesValue)
{
	std::string const data("dummy data");
	cpp_odbc::level2::input_string_buffer buffer(data);

	ASSERT_EQ(data.size(), buffer.size());

	for (std::size_t i = 0; i < data.size(); ++i) {
		EXPECT_EQ(data[i], buffer.data_pointer()[i]);
	}
}

TEST(InputStringBufferTest, HasTrailingSlashZero)
{
	std::string const data("dummy data");
	cpp_odbc::level2::input_string_buffer buffer(data);

	EXPECT_EQ(static_cast<unsigned char>('\0'), *(buffer.data_pointer() + buffer.size()));
}
