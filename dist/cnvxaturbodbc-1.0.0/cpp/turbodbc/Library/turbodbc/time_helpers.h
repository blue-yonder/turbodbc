#pragma once

#include <cstdint>

namespace turbodbc {

/**
 * @brief Convert an SQL_TIMESTAMP_STRUCT stored at data_pointer to an
 *        integer describing the elapsed microseconds since the POSIX epoch
 */
int64_t timestamp_to_microseconds(char const * data_pointer);

/**
 * @brief Convert the number of microseconds since the POSIX epoch
 *        to a timestamp and store it in an SQL_TIMESTAMP_STRUCT located
 *        at the data_pointer
 */
void microseconds_to_timestamp(int64_t microseconds, char * data_pointer);


/**
 * @brief Convert the number of nanoseconds since the POSIX epoch
 *        to a timestamp and store it in an SQL_TIMESTAMP_STRUCT located
 *        at the data_pointer
 */
void nanoseconds_to_timestamp(int64_t nanoseconds, char * data_pointer);


/**
 * @brief Convert an SQL_DATE_STRUCT stored at data_pointer to an
 *        integer describing the elapsed days since the POSIX epoch
 */
int64_t date_to_days(char const * data_pointer);

/**
 * @brief Convert the number of days since the POSIX epoch
 *        to a date and store it in an SQL_DATE_STRUCT located
 *        at the data_pointer
 */
void days_to_date(int64_t days, char * data_pointer);

}
