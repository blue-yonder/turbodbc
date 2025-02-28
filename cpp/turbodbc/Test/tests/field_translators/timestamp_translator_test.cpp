#include "turbodbc/field_translators/timestamp_translator.h"

#include <gtest/gtest.h>
#include <chrono>

using turbodbc::field_translators::timestamp_translator;

TEST(TimestampTranslatorTest, MakeField)
{
    cpp_odbc::multi_value_buffer buffer(sizeof(SQL_TIMESTAMP_STRUCT), 1);
    auto element = buffer[0];
    element.indicator = 1;
    auto const & as_const = buffer;

    timestamp_translator const translator;

    *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(element.data_pointer) = {2015, 12, 31, 1, 2, 3, 123456000};
    
    std::chrono::system_clock::time_point const expected =
        std::chrono::sys_days{std::chrono::year{2015}/std::chrono::month{12}/std::chrono::day{31}}
      + std::chrono::hours{1}
      + std::chrono::minutes{2}
      + std::chrono::seconds{3}
      + std::chrono::microseconds{123456};

    EXPECT_EQ(turbodbc::field(expected), *(translator.make_field(as_const[0])));
}
