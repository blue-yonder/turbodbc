#include "turbodbc/field_translators/string_translator.h"

#include <gtest/gtest.h>


using turbodbc::field_translators::string_translator;

TEST(StringTranslatorTest, MakeField)
{
	cpp_odbc::multi_value_buffer buffer(10, 1);
	auto element = buffer[0];
	std::string const expected("hi there");
	std::strcpy(element.data_pointer, expected.c_str());
	element.indicator = expected.size();

	auto const & as_const = buffer;
	string_translator const translator;
	EXPECT_EQ(turbodbc::field{expected}, *(translator.make_field(as_const[0])));
}
