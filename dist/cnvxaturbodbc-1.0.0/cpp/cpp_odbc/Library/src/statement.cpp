#include "cpp_odbc/statement.h"

namespace cpp_odbc {


statement::statement() = default;
statement::~statement() = default;

intptr_t statement::get_integer_attribute(SQLINTEGER attribute) const
{
    return do_get_integer_attribute(attribute);
}

void statement::set_attribute(SQLINTEGER attribute, intptr_t value) const
{
    do_set_attribute(attribute, value);
}

void statement::set_attribute(SQLINTEGER attribute, SQLULEN * pointer) const
{
    do_set_attribute(attribute, pointer);
}

void statement::execute(std::string const & sql) const
{
    do_execute(sql);
}

void statement::prepare(std::string const & sql) const
{
    do_prepare(sql);
}

void statement::prepare(std::u16string const & sql) const
{
    do_prepare(sql);
}

void statement::bind_input_parameter(SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, cpp_odbc::multi_value_buffer & parameter_values) const
{
    do_bind_input_parameter(parameter_id, value_type, parameter_type, digits, parameter_values);
}

void statement::unbind_all_parameters() const
{
    do_unbind_all_parameters();
}

void statement::execute_prepared() const
{
    do_execute_prepared();
}

short int statement::number_of_columns() const
{
    return do_number_of_columns();
}

short int statement::number_of_parameters() const
{
    return do_number_of_parameters();
}

void statement::bind_column(SQLUSMALLINT column_id, SQLSMALLINT column_type, cpp_odbc::multi_value_buffer & column_buffer) const
{
    do_bind_column(column_id, column_type, column_buffer);
}

void statement::unbind_all_columns() const
{
    do_unbind_all_columns();
}

bool statement::fetch_next() const
{
    return do_fetch_next();
}

void statement::close_cursor() const
{
    do_close_cursor();
}

intptr_t statement::get_integer_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return do_get_integer_column_attribute(column_id, field_identifier);
}

std::string statement::get_string_column_attribute(SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return do_get_string_column_attribute(column_id, field_identifier);
}

SQLLEN statement::row_count() const
{
    return do_row_count();
}

column_description statement::describe_column(SQLUSMALLINT column_id) const
{
    return do_describe_column(column_id);
}

column_description statement::describe_column_wide(SQLUSMALLINT column_id) const
{
    return do_describe_column_wide(column_id);
}

column_description statement::describe_parameter(SQLUSMALLINT parameter_id) const
{
    return do_describe_parameter(parameter_id);
}

bool statement::more_results() const
{
    return do_more_results();
}

void statement::finalize()
{
    if(!finalized){
        finalized = true;
        do_finalize();
    }
}

}
