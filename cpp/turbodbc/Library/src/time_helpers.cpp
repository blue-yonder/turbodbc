#include <turbodbc/time_helpers.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <cstring>
#include <sql.h>
#include <chrono>

namespace turbodbc {

// Alias a time_point with microsecond precision.
using sys_time_us = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

// Define the epoch for timestamps and dates.
constexpr sys_time_us timestamp_epoch{};
constexpr std::chrono::sys_days date_epoch{};

int64_t timestamp_to_microseconds(char const * data_pointer)
{
    auto const & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer);

    // Build the date component using C++20 calendar types.
    std::chrono::year_month_day ymd{
        std::chrono::year{sql_ts.year},
        std::chrono::month{sql_ts.month},
        std::chrono::day{sql_ts.day}
    };
    std::chrono::sys_days date_part{ymd};

    // Build the time-of-day portion.
    // Note: sql_ts.fraction is assumed to be in nanoseconds.
    auto micros_from_fraction = std::chrono::microseconds(sql_ts.fraction / 1000);
    auto time_of_day = std::chrono::hours(sql_ts.hour)
                     + std::chrono::minutes(sql_ts.minute)
                     + std::chrono::seconds(sql_ts.second)
                     + micros_from_fraction;

    // Combine date and time.
    sys_time_us tp = std::chrono::time_point_cast<std::chrono::microseconds>(date_part) + time_of_day;

    // Return microseconds elapsed since the epoch.
    auto duration_since_epoch = tp - timestamp_epoch;
    return duration_since_epoch.count();
}

void microseconds_to_timestamp(int64_t microseconds, char * data_pointer)
{
    // Compute the time point from the epoch.
    sys_time_us tp = timestamp_epoch + std::chrono::microseconds(microseconds);

    // Separate date and time components.
    auto dp = std::chrono::floor<std::chrono::days>(tp);
    std::chrono::year_month_day ymd{dp};
    auto time_of_day = tp - dp;

    // Extract hours, minutes, seconds, and microseconds.
    auto hrs   = std::chrono::duration_cast<std::chrono::hours>(time_of_day);
    time_of_day -= hrs;
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(time_of_day);
    time_of_day -= mins;
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(time_of_day);
    time_of_day -= secs;
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(time_of_day);

    // Fill the SQL_TIMESTAMP_STRUCT.
    auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(data_pointer);
    sql_ts.year   = static_cast<SQLSMALLINT>(static_cast<int>(ymd.year()));
    sql_ts.month  = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.month()));
    sql_ts.day    = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.day()));
    sql_ts.hour   = static_cast<SQLUSMALLINT>(hrs.count());
    sql_ts.minute = static_cast<SQLUSMALLINT>(mins.count());
    sql_ts.second = static_cast<SQLUSMALLINT>(secs.count());
    sql_ts.fraction = static_cast<SQLUINTEGER>(micros.count() * 1000);
}


void nanoseconds_to_timestamp(int64_t nanoseconds, char * data_pointer)
{
    // Create a nanoseconds duration from the input.
    auto ns = std::chrono::nanoseconds(nanoseconds);
    sys_time_us tp = timestamp_epoch + std::chrono::duration_cast<std::chrono::microseconds>(ns);

    auto dp = std::chrono::floor<std::chrono::days>(tp);
    std::chrono::year_month_day ymd{dp};
    auto time_of_day = tp - dp;
    auto hrs   = std::chrono::duration_cast<std::chrono::hours>(time_of_day);
    time_of_day -= hrs;
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(time_of_day);
    time_of_day -= mins;
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(time_of_day);
    time_of_day -= secs;
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(time_of_day);

    auto remainder = ns % std::chrono::nanoseconds(1000);

    auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(data_pointer);
    sql_ts.year   = static_cast<SQLSMALLINT>(static_cast<int>(ymd.year()));
    sql_ts.month  = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.month()));
    sql_ts.day    = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.day()));
    sql_ts.hour   = static_cast<SQLUSMALLINT>(hrs.count());
    sql_ts.minute = static_cast<SQLUSMALLINT>(mins.count());
    sql_ts.second = static_cast<SQLUSMALLINT>(secs.count());
    // Combine the microsecond part (converted back to nanoseconds) with the remainder.
    sql_ts.fraction = static_cast<SQLUINTEGER>(micros.count() * 1000 + remainder.count());
}


int64_t date_to_days(char const * data_pointer)
{
    auto const & sql_date = *reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer);
    std::chrono::year_month_day ymd{
        std::chrono::year{sql_date.year},
        std::chrono::month{sql_date.month},
        std::chrono::day{sql_date.day}
    };
    std::chrono::sys_days date_val{ymd};
    return (date_val - date_epoch).count();
}

void days_to_date(int64_t days, char * data_pointer)
{
    std::chrono::sys_days dp = date_epoch + std::chrono::days{days};
    std::chrono::year_month_day ymd{dp};

    auto & sql_date = *reinterpret_cast<SQL_DATE_STRUCT *>(data_pointer);
    sql_date.year  = static_cast<SQLSMALLINT>(static_cast<int>(ymd.year()));
    sql_date.month = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.month()));
    sql_date.day   = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.day()));
}


}
