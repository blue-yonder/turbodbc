#include <turbodbc/field_translators/date_translator.h>

#include <boost/variant/get.hpp>

#include <sql.h>


namespace turbodbc { namespace field_translators {

date_translator::date_translator() = default;

date_translator::~date_translator() = default;

field date_translator::do_make_field(char const * data_pointer) const
{
	auto const date = reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer);
	return {boost::gregorian::date(date->year, date->month, date->day)};
}


} }
