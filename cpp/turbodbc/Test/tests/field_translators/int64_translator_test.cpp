#include "turbodbc/field_translators/int64_translator.h"

#include <gtest/gtest.h>


using turbodbc::field_translators::int64_translator;

TEST(Int64TranslatorTest, MakeField)
{
	cpp_odbc::multi_value_buffer buffer(8, 1);
	auto element = buffer[0];
	element.indicator = 8;
	auto const & as_const = buffer;

	int64_translator const translator;

	*reinterpret_cast<int64_t *>(element.data_pointer) = 42;
	EXPECT_EQ(turbodbc::field{int64_t(42)}, *(translator.make_field(as_const[0])));
}
