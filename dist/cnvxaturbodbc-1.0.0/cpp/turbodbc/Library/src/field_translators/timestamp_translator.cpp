#include <turbodbc/field_translators/timestamp_translator.h>

#include <boost/variant/get.hpp>

#include <sql.h>


namespace turbodbc { namespace field_translators {

timestamp_translator::timestamp_translator() = default;

timestamp_translator::~timestamp_translator() = default;

field timestamp_translator::do_make_field(char const * data_pointer) const
{
	auto const ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer);
	// map SQL nanosecond precision to posix_time microsecond precision
	int64_t const adjusted_fraction = ts->fraction / 1000;
	return {boost::posix_time::ptime{
										{static_cast<short unsigned int>(ts->year), ts->month, ts->day},
										{ts->hour, ts->minute, ts->second, adjusted_fraction}
									}
			};
}

} }
