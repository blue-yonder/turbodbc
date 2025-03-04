#include <turbodbc/descriptions/date_description.h>

#include <sqlext.h>

#include <boost/variant/get.hpp>
#include <cstring>

namespace turbodbc {

date_description::date_description() = default;

date_description::date_description(std::string name, bool supports_null) :
	description(std::move(name), supports_null)
{
}

date_description::~date_description() = default;

std::size_t date_description::do_element_size() const
{
	return sizeof(SQL_DATE_STRUCT);
}

SQLSMALLINT date_description::do_column_c_type() const
{
	return SQL_C_TYPE_DATE;
}

SQLSMALLINT date_description::do_column_sql_type() const
{
	return SQL_TYPE_DATE;
}

SQLSMALLINT date_description::do_digits() const
{
	return 0;
}

type_code date_description::do_get_type_code() const
{
	return type_code::date;
}

}
