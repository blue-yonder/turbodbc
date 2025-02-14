#include <turbodbc/descriptions/integer_description.h>

#include <sqlext.h>

#include <boost/variant/get.hpp>
#include <cstring>

namespace turbodbc {

integer_description::integer_description() = default;

integer_description::integer_description(std::string name, bool supports_null) :
	description(std::move(name), supports_null)
{
}

integer_description::~integer_description() = default;

std::size_t integer_description::do_element_size() const
{
	return sizeof(int64_t);
}

SQLSMALLINT integer_description::do_column_c_type() const
{
	return SQL_C_SBIGINT;
}

SQLSMALLINT integer_description::do_column_sql_type() const
{
	return SQL_BIGINT;
}

SQLSMALLINT integer_description::do_digits() const
{
	return 0;
}

type_code integer_description::do_get_type_code() const
{
	return type_code::integer;
}

}
