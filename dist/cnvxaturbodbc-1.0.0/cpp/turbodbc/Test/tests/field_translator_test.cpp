#include "turbodbc/field_translator.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sql.h>


namespace {

	struct mock_translator : public turbodbc::field_translator {
		MOCK_CONST_METHOD1(do_make_field, turbodbc::field(char const *));
		MOCK_CONST_METHOD2(do_set_field, void(cpp_odbc::writable_buffer_element &, turbodbc::field const &));
	};

}


TEST(FieldTranslatorTest, MakeFieldForwards)
{
	turbodbc::nullable_field const expected(turbodbc::field(int64_t(42)));
	cpp_odbc::multi_value_buffer buffer(8, 3);
	auto element = buffer[0];
	element.indicator = 1;

	mock_translator translator;
	EXPECT_CALL(translator, do_make_field(element.data_pointer))
		.WillOnce(testing::Return(*expected));

	cpp_odbc::multi_value_buffer const & as_const = buffer;
	EXPECT_EQ(expected, translator.make_field(as_const[0]));
}

TEST(FieldTranslatorTest, MakeFieldHandlesNull)
{
	turbodbc::nullable_field const expected;
	cpp_odbc::multi_value_buffer buffer(8, 3);
	auto element = buffer[0];
	element.indicator = SQL_NULL_DATA;

	mock_translator translator;

	cpp_odbc::multi_value_buffer const & as_const = buffer;
	EXPECT_EQ(expected, translator.make_field(as_const[0]));
}
