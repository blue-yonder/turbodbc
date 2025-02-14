#include <turbodbc/time_helpers.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <cstring>
#include <sql.h>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace turbodbc {

namespace {
    boost::posix_time::ptime const timestamp_epoch({1970, 1, 1}, {0, 0, 0, 0});
    boost::gregorian::date const date_epoch(1970, 1, 1);
}


int64_t timestamp_to_microseconds(char const * data_pointer)
{
    auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer);
    intptr_t const microseconds = sql_ts.fraction / 1000;
    boost::posix_time::ptime const ts({static_cast<unsigned short>(sql_ts.year), sql_ts.month, sql_ts.day},
            {sql_ts.hour, sql_ts.minute, sql_ts.second, microseconds});
    return (ts - timestamp_epoch).total_microseconds();
}


void microseconds_to_timestamp(int64_t microseconds, char * data_pointer)
{
    // use time duration constructor instead of boost::posix_time::microseconds here
    // because some older version of boost cause an overflow, see https://svn.boost.org/trac10/ticket/3471
    // Debian 7's boost 1.49 seems to be affected, though the fix claims to be done with boost 1.43.
    auto const ptime = timestamp_epoch + boost::posix_time::time_duration(0, 0, 0, microseconds);
    auto const date = ptime.date();
    auto const time = ptime.time_of_day();
    auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(data_pointer);
    sql_ts.year = date.year();
    sql_ts.month = date.month();
    sql_ts.day = date.day();
    sql_ts.hour = time.hours();
    sql_ts.minute = time.minutes();
    sql_ts.second = time.seconds();
    sql_ts.fraction = time.fractional_seconds() * 1000;
}


void nanoseconds_to_timestamp(int64_t nanoseconds, char * data_pointer)
{
    // use time duration constructor instead of boost::posix_time::microseconds here
    // because some older version of boost cause an overflow, see https://svn.boost.org/trac10/ticket/3471
    // Debian 7's boost 1.49 seems to be affected, though the fix claims to be done with boost 1.43.
    auto const ptime = timestamp_epoch + boost::posix_time::time_duration(0, 0, 0, nanoseconds / 1000);
    auto const date = ptime.date();
    auto const time = ptime.time_of_day();
    auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(data_pointer);
    sql_ts.year = date.year();
    sql_ts.month = date.month();
    sql_ts.day = date.day();
    sql_ts.hour = time.hours();
    sql_ts.minute = time.minutes();
    sql_ts.second = time.seconds();
    sql_ts.fraction = time.fractional_seconds() * 1000 + nanoseconds % 1000;
}


int64_t date_to_days(char const * data_pointer)
{
    auto & sql_date = *reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer);
    boost::gregorian::date const date(sql_date.year, sql_date.month, sql_date.day);
    return (date - date_epoch).days();
}


void days_to_date(int64_t days, char * data_pointer) {
    auto const date = date_epoch + boost::gregorian::date_duration(days);
    auto &sql_date = *reinterpret_cast<SQL_DATE_STRUCT *>(data_pointer);
    sql_date.year = date.year();
    sql_date.month = date.month();
    sql_date.day = date.day();
}


}
