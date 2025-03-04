#include <turbodbc/descriptions/boolean_description.h>

#include <boost/variant/get.hpp>
#include <sqlext.h>

namespace turbodbc {

boolean_description::boolean_description() = default;

boolean_description::boolean_description(std::string name, bool supports_null) :
	description(std::move(name), supports_null)
{
}


boolean_description::~boolean_description() = default;

std::size_t boolean_description::do_element_size() const
{
	return sizeof(char);
}

SQLSMALLINT boolean_description::do_column_c_type() const
{
	return SQL_C_BIT;
}

SQLSMALLINT boolean_description::do_column_sql_type() const
{
	return SQL_BIT;
}

SQLSMALLINT boolean_description::do_digits() const
{
	return 0;
}

type_code boolean_description::do_get_type_code() const
{
	return type_code::boolean;
}



}
