#pragma once

namespace turbodbc { namespace datetime_utils {

/**
 * @brief Translates an SQL timestamp to microseconds since UNIX Epoch
 */
long timestamp_to_microseconds(char const * sql_timestamp);

/**
 * @brief Translates an SQL date to days since UNIX Epoch
 */
long date_to_days(char const * sql_date);

} }
