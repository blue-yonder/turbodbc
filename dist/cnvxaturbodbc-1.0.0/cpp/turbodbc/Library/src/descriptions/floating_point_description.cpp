#include <turbodbc/descriptions/floating_point_description.h>

#include <sqlext.h>

#include <boost/variant/get.hpp>
#include <cstring>

namespace turbodbc {

floating_point_description::floating_point_description() = default;

floating_point_description::floating_point_description(std::string name, bool supports_null) :
	description(std::move(name), supports_null)
{
}

floating_point_description::~floating_point_description() = default;

std::size_t floating_point_description::do_element_size() const
{
	return sizeof(double);
}

SQLSMALLINT floating_point_description::do_column_c_type() const
{
	return SQL_C_DOUBLE;
}

SQLSMALLINT floating_point_description::do_column_sql_type() const
{
	return SQL_DOUBLE;
}

SQLSMALLINT floating_point_description::do_digits() const
{
	return 0;
}

type_code floating_point_description::do_get_type_code() const
{
	return type_code::floating_point;
}

}
