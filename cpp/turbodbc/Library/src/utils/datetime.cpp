#include <ctime>
#include <sql.h>

namespace turbodbc { namespace datetime_utils {

long timestamp_to_microseconds(char const * data_pointer)
{
	auto & sql_ts = *reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer);
	std::tm epoch = {};
	epoch.tm_year = 70;
	epoch.tm_mon  = 0;
	epoch.tm_mday = 1;

	std::tm timestamp = {};
	timestamp.tm_year = sql_ts.year - 1900;
	timestamp.tm_mon  = sql_ts.month - 1;
	timestamp.tm_mday = sql_ts.day;
	timestamp.tm_hour = sql_ts.hour;
	timestamp.tm_min  = sql_ts.minute;
	timestamp.tm_sec  = sql_ts.second;

	return std::difftime(std::mktime(&timestamp), std::mktime(&epoch)) * (1000*1000) + (sql_ts.fraction/1000);
}

long date_to_days(char const * data_pointer)
{
	auto & sql_date = *reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer);
	std::tm epoch   = {};
	epoch.tm_year   = 70;
	epoch.tm_mon    = 0;
	epoch.tm_mday   = 1;

	std::tm date = {};
	date.tm_year = sql_date.year - 1900;
	date.tm_mon  = sql_date.month - 1;
	date.tm_mday = sql_date.day;

	return std::difftime(std::mktime(&date), std::mktime(&epoch))/(60*60*24);
}

} }
