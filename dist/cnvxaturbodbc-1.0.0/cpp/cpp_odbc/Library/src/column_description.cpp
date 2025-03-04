#include "cpp_odbc/column_description.h"

#include <iostream>
#include <map>
#include <ciso646>

#include <sqlext.h>

namespace cpp_odbc {

namespace {
	std::map<SQLSMALLINT, std::string> const known_types = {
			{SQL_CHAR, "SQL_CHAR"},
			{SQL_VARCHAR, "SQL_VARCHAR"},
			{SQL_LONGVARCHAR, "SQL_LONGVARCHAR"},
			{SQL_WCHAR, "SQL_WCHAR"},
			{SQL_WVARCHAR, "SQL_WVARCHAR"},
			{SQL_WLONGVARCHAR, "SQL_WLONGVARCHAR"},
			{SQL_DECIMAL, "SQL_DECIMAL"},
			{SQL_NUMERIC, "SQL_NUMERIC"},
			{SQL_SMALLINT, "SQL_SMALLINT"},
			{SQL_INTEGER, "SQL_INTEGER"},
			{SQL_REAL, "SQL_REAL"},
			{SQL_FLOAT, "SQL_FLOAT"},
			{SQL_DOUBLE, "SQL_DOUBLE"},
			{SQL_BIT, "SQL_BIT"},
			{SQL_TINYINT, "SQL_TINYINT"},
			{SQL_BIGINT, "SQL_BIGINT"},
			{SQL_BINARY, "SQL_BINARY"},
			{SQL_VARBINARY, "SQL_VARBINARY"},
			{SQL_LONGVARBINARY, "SQL_LONGVARBINARY"},
			{SQL_TYPE_DATE, "SQL_TYPE_DATE"},
			{SQL_TYPE_TIME, "SQL_TYPE_TIME"},
			{SQL_TYPE_TIMESTAMP, "SQL_TYPE_TIMESTAMP"},
			// {SQL_TYPE_UTCDATETIME, "SQL_TYPE_UTCDATETIME"}, // not in unixodbc 2.2.14
			// {SQL_TYPE_UTCTIME, "SQL_TYPE_UTCTIME"},         // not in unixodbc 2.2.14
			{SQL_INTERVAL_MONTH, "SQL_INTERVAL_MONTH"},
			{SQL_INTERVAL_YEAR, "SQL_INTERVAL_YEAR"},
			{SQL_INTERVAL_YEAR_TO_MONTH, "SQL_INTERVAL_YEAR_TO_MONTH"},
			{SQL_INTERVAL_DAY, "SQL_INTERVAL_DAY"},
			{SQL_INTERVAL_HOUR, "SQL_INTERVAL_HOUR"},
			{SQL_INTERVAL_MINUTE, "SQL_INTERVAL_MINUTE"},
			{SQL_INTERVAL_SECOND, "SQL_INTERVAL_SECOND"},
			{SQL_INTERVAL_DAY_TO_HOUR, "SQL_INTERVAL_DAY_TO_HOUR"},
			{SQL_INTERVAL_DAY_TO_MINUTE, "SQL_INTERVAL_DAY_TO_MINUTE"},
			{SQL_INTERVAL_DAY_TO_SECOND, "SQL_INTERVAL_DAY_TO_SECOND"},
			{SQL_INTERVAL_HOUR_TO_MINUTE, "SQL_INTERVAL_HOUR_TO_MINUTE"},
			{SQL_INTERVAL_HOUR_TO_SECOND, "SQL_INTERVAL_HOUR_TO_SECOND"},
			{SQL_INTERVAL_MINUTE_TO_SECOND, "SQL_INTERVAL_MINUTE_TO_SECOND"},
			{SQL_GUID, "SQL_GUID"}
	};

	std::string get_typename(SQLSMALLINT type)
	{
		auto const it = known_types.find(type);
		if (it != known_types.end()) {
			return it->second;
		} else {
			return "UNKNOWN TYPE";
		}
	}
}

bool operator==(column_description const & lhs, column_description const & rhs)
{
	return (lhs.name == rhs.name) and (lhs.data_type == rhs.data_type)
			and (lhs.size == rhs.size) and (lhs.decimal_digits == rhs.decimal_digits)
			and (lhs.allows_null_values == rhs.allows_null_values);
}

std::ostream & operator<<(std::ostream & destination, column_description const & source)
{
	destination << source.name << " @ "
			<< (source.allows_null_values ? "NULLABLE " : "")
			<< get_typename(source.data_type) << " ("
			<< "precision " << source.size
			<< ", scale " << source.decimal_digits << ")";
	return destination;
}

}
