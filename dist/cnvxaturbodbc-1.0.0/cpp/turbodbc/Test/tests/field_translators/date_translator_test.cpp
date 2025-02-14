#include "turbodbc/field_translators/date_translator.h"

#include <gtest/gtest.h>


using turbodbc::field_translators::date_translator;

TEST(DateTranslatorTest, MakeField)
{
    cpp_odbc::multi_value_buffer buffer(sizeof(SQL_DATE_STRUCT), 1);
    auto element = buffer[0];
    element.indicator = 1;
    auto const & as_const = buffer;

    date_translator const translator;

    *reinterpret_cast<SQL_DATE_STRUCT *>(element.data_pointer) = {2015, 12, 31};
    boost::gregorian::date const expected{2015, 12, 31};
    EXPECT_EQ(turbodbc::field(expected), *(translator.make_field(as_const[0])));
}
