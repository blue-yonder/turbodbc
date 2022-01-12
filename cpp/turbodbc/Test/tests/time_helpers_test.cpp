#include "turbodbc/time_helpers.h"

#include <gtest/gtest.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>

using turbodbc::timestamp_to_microseconds;
using turbodbc::timestamp_to_microseconds_truncated;
using turbodbc::microseconds_to_timestamp;
using turbodbc::nanoseconds_to_timestamp;
using turbodbc::date_to_days;
using turbodbc::days_to_date;

TEST(TimeHelpersTest, TimestampToMicrosecondsForEpoch)
{
    SQL_TIMESTAMP_STRUCT data = {1970, 01, 01, 0, 0, 0, 0};
    EXPECT_EQ(0, timestamp_to_microseconds(reinterpret_cast<char const *>(&data)));
}

TEST(TimeHelpersTest, TimestampToMicrosecondsForYear4000)
{
    SQL_TIMESTAMP_STRUCT data = {4000, 01, 02, 3, 4, 5, 123456000};
    // expectation generated by unixtimestamp.com
    std::int64_t expected = 64060686245 * 1000000 + 123456;
    EXPECT_EQ(expected, timestamp_to_microseconds(reinterpret_cast<char const *>(&data)));
}


TEST(TimeHelpersTest, MicrosecondsToTimestampForEpoch)
{
    std::int64_t const microseconds = 0;
    SQL_TIMESTAMP_STRUCT ts;
    microseconds_to_timestamp(microseconds, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(1970, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(1, ts.day);
    EXPECT_EQ(0, ts.hour);
    EXPECT_EQ(0, ts.minute);
    EXPECT_EQ(0, ts.second);
    EXPECT_EQ(0, ts.fraction);
}


TEST(TimeHelpersTest, MicrosecondsToTimestampForYear4000)
{
    // generated by unixtimestamp.com
    std::int64_t const microseconds = 64060686245 * 1000000 + 123456;
    SQL_TIMESTAMP_STRUCT ts;
    microseconds_to_timestamp(microseconds, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(4000, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(2, ts.day);
    EXPECT_EQ(3, ts.hour);
    EXPECT_EQ(4, ts.minute);
    EXPECT_EQ(5, ts.second);
    EXPECT_EQ(123456000, ts.fraction);
}


TEST(TimeHelpersTest, TimestampToMicrosecondsTruncatedForYear10000)
{
    SQL_TIMESTAMP_STRUCT data = {10000, 01, 02, 3, 4, 5, 123456000};
    // expectation one microsecond before year 10000
    std::int64_t expected = 253402300799999999;
    EXPECT_EQ(expected, timestamp_to_microseconds_truncated(reinterpret_cast<char const *>(&data)));
}


TEST(TimeHelpersTest, TimestampToMicrosecondsTruncatedForYear100)
{
    SQL_TIMESTAMP_STRUCT data = {100, 01, 02, 3, 4, 5, 0};
    // expectation january first 1400
    std::int64_t expected = -17987443200000000;
    EXPECT_EQ(expected, timestamp_to_microseconds_truncated(reinterpret_cast<char const *>(&data)));
}


TEST(TimeHelpersTest, NanosecondsToTimestampForEpoch)
{
    std::int64_t const nanoseconds = 0;
    SQL_TIMESTAMP_STRUCT ts;
    nanoseconds_to_timestamp(nanoseconds, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(1970, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(1, ts.day);
    EXPECT_EQ(0, ts.hour);
    EXPECT_EQ(0, ts.minute);
    EXPECT_EQ(0, ts.second);
    EXPECT_EQ(0, ts.fraction);
}


TEST(TimeHelpersTest, NanosecondsToTimestampForYear2200)
{
    // generated by unixtimestamp.com
    std::int64_t const nanoseconds = 7258215845 * 1000000000 + 123456789;
    SQL_TIMESTAMP_STRUCT ts;
    nanoseconds_to_timestamp(nanoseconds, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(2200, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(2, ts.day);
    EXPECT_EQ(3, ts.hour);
    EXPECT_EQ(4, ts.minute);
    EXPECT_EQ(5, ts.second);
    EXPECT_EQ(123456789, ts.fraction);
}


TEST(TimeHelpersTest, DateToDaysForEpoch)
{
    SQL_DATE_STRUCT data = {1970, 01, 01};
    EXPECT_EQ(0, date_to_days(reinterpret_cast<char const *>(&data)));
}


TEST(TimeHelpersTest, DateToDaysForYear4000)
{
    SQL_DATE_STRUCT data = {4000, 01, 02};
    // expectation generated by http://www.convertunits.com/dates/from/Jan+1,+1970/to/Jan+2,+4000
    std::int64_t expected = 741443;
    EXPECT_EQ(expected, date_to_days(reinterpret_cast<char const *>(&data)));
}


TEST(TimeHelpersTest, DaysToDateForEpoch)
{
    std::int64_t const days = 0;
    SQL_TIMESTAMP_STRUCT ts;
    days_to_date(days, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(1970, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(1, ts.day);
}


TEST(TimeHelpersTest, DaysToDateForYear4000)
{
    // generated by http://www.convertunits.com/dates/from/Jan+1,+1970/to/Jan+2,+4000
    std::int64_t const days = 741443;
    SQL_TIMESTAMP_STRUCT ts;
    days_to_date(days, reinterpret_cast<char *>(&ts));
    EXPECT_EQ(4000, ts.year);
    EXPECT_EQ(1, ts.month);
    EXPECT_EQ(2, ts.day);
}