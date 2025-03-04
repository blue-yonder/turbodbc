#include "cpp_odbc/connection.h"

namespace cpp_odbc {

connection::connection() = default;
connection::~connection() = default;

std::shared_ptr<statement const> connection::make_statement() const
{
	return do_make_statement();
}

void connection::set_attribute(SQLINTEGER attribute, intptr_t value) const
{
	do_set_attribute(attribute, value);
}

void connection::commit() const
{
	do_commit();
}

void connection::rollback() const
{
	do_rollback();
}

std::string connection::get_string_info(SQLUSMALLINT info_type) const
{
	return do_get_string_info(info_type);
}

SQLUINTEGER connection::get_integer_info(SQLUSMALLINT info_type) const
{
	return do_get_integer_info(info_type);
}

bool connection::supports_function(SQLUSMALLINT function_id) const
{
	return do_supports_function(function_id);
}

}
