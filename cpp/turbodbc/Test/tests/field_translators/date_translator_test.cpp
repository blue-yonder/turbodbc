#include "turbodbc/field_translators/date_translator.h"

#include <gtest/gtest.h>
#include <chrono>

using turbodbc::field_translators::date_translator;

TEST(DateTranslatorTest, MakeField)
{
    cpp_odbc::multi_value_buffer buffer(sizeof(SQL_DATE_STRUCT), 1);
    auto element = buffer[0];
    element.indicator = 1;
    auto const & as_const = buffer;

    date_translator const translator;

    *reinterpret_cast<SQL_DATE_STRUCT *>(element.data_pointer) = {2015, 12, 31};
    std::chrono::year_month_day const expected{std::chrono::year{2015}, std::chrono::month{12}, std::chrono::day{31}};
    EXPECT_EQ(turbodbc::field(expected), *(translator.make_field(as_const[0])));
}
