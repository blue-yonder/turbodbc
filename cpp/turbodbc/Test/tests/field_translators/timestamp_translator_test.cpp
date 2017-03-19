#include "turbodbc/field_translators/timestamp_translator.h"

#include <gtest/gtest.h>
#include <tests/mock_classes.h>


using turbodbc::field_translators::timestamp_translator;

TEST(TimestampTranslatorTest, MakeField)
{
	cpp_odbc::multi_value_buffer buffer(1, 1);
	auto element = buffer[0];
	element.indicator = 1;
	auto const & as_const = buffer;

	timestamp_translator const translator;

	*reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(element.data_pointer) = {2015, 12, 31, 1, 2, 3, 123456000};
	boost::posix_time::ptime const expected{{2015, 12, 31}, {1, 2, 3, 123456}};
	EXPECT_EQ(turbodbc::field(expected), *(translator.make_field(as_const[0])));
}
