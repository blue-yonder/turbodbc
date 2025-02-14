#include <turbodbc/description.h>

namespace turbodbc {

description::description() :
	name_("parameter"),
	supports_null_(true)
{
}

description::description(std::string name, bool supports_null) :
	name_(std::move(name)),
	supports_null_(supports_null)
{
}

description::~description() = default;

std::size_t description::element_size() const
{
	return do_element_size();
}

SQLSMALLINT description::column_c_type() const
{
	return do_column_c_type();
}

SQLSMALLINT description::column_sql_type() const
{
	return do_column_sql_type();
}

SQLSMALLINT description::digits() const
{
	return do_digits();
}

type_code description::get_type_code() const
{
	return do_get_type_code();
}

std::string const & description::name() const
{
	return name_;
}

bool description::supports_null_values() const
{
	return supports_null_;
}

}
