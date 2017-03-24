#include "cpp_odbc/level2/input_u16string_buffer.h"

#include <gtest/gtest.h>


TEST(InputU16StringBufferTest, CopiesValue)
{
	std::u16string const data(u"dummy data");
	cpp_odbc::level2::input_u16string_buffer buffer(data);

	ASSERT_EQ(data.size(), buffer.size());

	for (std::size_t i = 0; i < data.size(); ++i) {
		EXPECT_EQ(data[i], buffer.data_pointer()[i]);
	}
}

TEST(InputU16StringBufferTest, HasTrailingSlashZero)
{
	std::u16string const data(u"dummy data");
	cpp_odbc::level2::input_u16string_buffer buffer(data);

	EXPECT_EQ(static_cast<unsigned short>(u'\0'), *(buffer.data_pointer() + buffer.size()));
}
