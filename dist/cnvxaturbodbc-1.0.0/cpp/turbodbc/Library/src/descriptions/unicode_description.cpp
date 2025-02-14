#include <turbodbc/descriptions/unicode_description.h>

#include <boost/variant/get.hpp>

#ifdef _WIN32
#include <windows.h>
#endif
#include <sqlext.h>
#include <cstring>

namespace turbodbc {

unicode_description::unicode_description(std::size_t maximum_length) :
	maximum_length_(maximum_length)
{
}

unicode_description::unicode_description(std::string name, bool supports_null, std::size_t maximum_length) :
	description(std::move(name), supports_null),
	maximum_length_(maximum_length)
{
}

unicode_description::~unicode_description() = default;

std::size_t unicode_description::do_element_size() const
{
	return sizeof(char16_t) * (maximum_length_ + 1);
}

SQLSMALLINT unicode_description::do_column_c_type() const
{
	return SQL_C_WCHAR;
}

SQLSMALLINT unicode_description::do_column_sql_type() const
{
	return SQL_WVARCHAR;
}

SQLSMALLINT unicode_description::do_digits() const
{
	return 0;
}

type_code unicode_description::do_get_type_code() const
{
	return type_code::unicode;
}

}
