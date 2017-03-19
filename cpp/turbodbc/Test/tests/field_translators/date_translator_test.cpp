#include "turbodbc/field_translators/date_translator.h"

#include <gtest/gtest.h>
#include <tests/mock_classes.h>


using turbodbc::field_translators::date_translator;

TEST(DateTranslatorTest, MakeField)
{
	cpp_odbc::multi_value_buffer buffer(1, 1);
	auto element = buffer[0];
	element.indicator = 1;
	auto const & as_const = buffer;

	date_translator const translator;

	*reinterpret_cast<SQL_DATE_STRUCT *>(element.data_pointer) = {2015, 12, 31};
	std::tm expected = std::tm {};
	expected.tm_year = 2015 - 1900;
	expected.tm_mon  = 12 - 1;
	expected.tm_mday = 7;
	EXPECT_EQ(turbodbc::field(expected), *(translator.make_field(as_const[0])));
}
