#include <turbodbc/make_description.h>

#include <gtest/gtest.h>
#include <turbodbc/descriptions.h>

#include <sqlext.h>
#include <sstream>
#include <stdexcept>
#include <chrono>

using turbodbc::make_description;
using turbodbc::field;


TEST(MakeDescriptionOfValueTest, FromInteger)
{
    field const value(int64_t(42));
    auto description = make_description(value);
    ASSERT_TRUE(dynamic_cast<turbodbc::integer_description const *>(description.get()));
}

TEST(MakeDescriptionOfValueTest, FromDouble)
{
    field const value(3.14);
    auto description = make_description(value);
    ASSERT_TRUE(dynamic_cast<turbodbc::floating_point_description const *>(description.get()));
}

TEST(MakeDescriptionOfValueTest, FromBool)
{
    field const value(true);
    auto description = make_description(value);
    ASSERT_TRUE(dynamic_cast<turbodbc::boolean_description const *>(description.get()));
}

TEST(MakeDescriptionOfValueTest, FromDate)
{
    using namespace std::chrono;
    field const value(year_month_day{year{2016}, month{1}, day{7}});
    auto description = make_description(value);
    ASSERT_TRUE(dynamic_cast<turbodbc::date_description const *>(description.get()));
}

TEST(MakeDescriptionOfValueTest, FromPtime)
{
    using namespace std::chrono;
    field const value(system_clock::time_point{sys_days{year{2016}/1/7} + hours{1} + minutes{2} + seconds{3} + microseconds{123456}});
    auto description = make_description(value);
    ASSERT_TRUE(dynamic_cast<turbodbc::timestamp_description const *>(description.get()));
}

TEST(MakeDescriptionOfValueTest, FromStringProvidesMinimumLength)
{
    std::string small_string("hi");
    field const value(small_string);
    auto description = make_description(value);
    auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
    ASSERT_TRUE(as_string_description != nullptr);

    std::size_t const minimum_length = 10;
    EXPECT_EQ(as_string_description->element_size(), minimum_length + 1);
}

TEST(MakeDescriptionOfValueTest, FromStringProvidesExtraSpaceForLargeStrings)
{
    std::string large_string("this is a relatively large string");
    field const value(large_string);
    auto description = make_description(value);
    auto as_string_description = dynamic_cast<turbodbc::string_description const *>(description.get());
    ASSERT_TRUE(as_string_description != nullptr);

    EXPECT_GT(as_string_description->element_size(), (large_string.size() + 1));
}
