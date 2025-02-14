#include <turbodbc/descriptions/timestamp_description.h>

#include <sqlext.h>

#include <boost/variant/get.hpp>
#include <cstring>

namespace turbodbc {

timestamp_description::timestamp_description() = default;

timestamp_description::timestamp_description(std::string name, bool supports_null) :
	description(std::move(name), supports_null)
{
}

timestamp_description::~timestamp_description() = default;

std::size_t timestamp_description::do_element_size() const
{
	return sizeof(SQL_TIMESTAMP_STRUCT);
}

SQLSMALLINT timestamp_description::do_column_c_type() const
{
	return SQL_C_TYPE_TIMESTAMP;
}

SQLSMALLINT timestamp_description::do_column_sql_type() const
{
	return SQL_TYPE_TIMESTAMP;
}

SQLSMALLINT timestamp_description::do_digits() const
{
	return 6;
}

type_code timestamp_description::do_get_type_code() const
{
	return type_code::timestamp;
}

}
