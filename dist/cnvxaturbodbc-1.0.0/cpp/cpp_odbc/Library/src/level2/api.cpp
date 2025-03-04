#include "cpp_odbc/level2/api.h"

namespace cpp_odbc { namespace level2 {

api::~api() = default;
api::api() = default;

statement_handle api::allocate_statement_handle(connection_handle const & input_handle) const
{
    return do_allocate_statement_handle(input_handle);
}

connection_handle api::allocate_connection_handle(environment_handle const & input_handle) const
{
    return do_allocate_connection_handle(input_handle);
}

environment_handle api::allocate_environment_handle() const
{
    return do_allocate_environment_handle();
}

void api::free_handle(statement_handle & handle) const
{
    do_free_handle(handle);
}

void api::free_handle(connection_handle & handle) const
{
    do_free_handle(handle);
}

void api::free_handle(environment_handle & handle) const
{
    do_free_handle(handle);
}

diagnostic_record api::get_diagnostic_record(statement_handle const & handle) const
{
    return do_get_diagnostic_record(handle);
}

diagnostic_record api::get_diagnostic_record(connection_handle const & handle) const
{
    return do_get_diagnostic_record(handle);
}

diagnostic_record api::get_diagnostic_record(environment_handle const & handle) const
{
    return do_get_diagnostic_record(handle);
}

void api::set_environment_attribute(environment_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    do_set_environment_attribute(handle, attribute, value);
}

void api::set_connection_attribute(connection_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    do_set_connection_attribute(handle, attribute, value);
}

void api::establish_connection(connection_handle & handle, std::string const & connection_string) const
{
    do_establish_connection(handle, connection_string);
}

void api::disconnect(connection_handle & handle) const
{
    do_disconnect(handle);
}

void api::end_transaction(connection_handle const & handle, SQLSMALLINT completion_type) const
{
    do_end_transaction(handle, completion_type);
}

std::string api::get_string_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const
{
    return do_get_string_connection_info(handle, info_type);
}

SQLUINTEGER api::get_integer_connection_info(connection_handle const & handle, SQLUSMALLINT info_type) const
{
    return do_get_integer_connection_info(handle, info_type);
}

void api::bind_column(statement_handle const & handle, SQLUSMALLINT column_id, SQLSMALLINT column_type, multi_value_buffer & column_buffer) const
{
    do_bind_column(handle, column_id, column_type, column_buffer);
}

void api::bind_input_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLSMALLINT digits, multi_value_buffer & parameter_values) const
{
    do_bind_input_parameter(handle, parameter_id, value_type, parameter_type, digits, parameter_values);
}

void api::execute_prepared_statement(statement_handle const & handle) const
{
    do_execute_prepared_statement(handle);
}

void api::execute_statement(statement_handle const & handle, std::string const & sql) const
{
    do_execute_statement(handle, sql);
}

bool api::fetch_scroll(statement_handle const & statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const
{
    return do_fetch_scroll(statement_handle, fetch_orientation, fetch_offset);
}

void api::free_statement(statement_handle const & handle, SQLUSMALLINT option) const
{
do_free_statement(handle, option);
}

intptr_t api::get_integer_column_attribute(cpp_odbc::level2::statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return do_get_integer_column_attribute(handle, column_id, field_identifier);
}

intptr_t api::get_integer_statement_attribute(statement_handle const & handle, SQLINTEGER attribute) const
{
    return do_get_integer_statement_attribute(handle, attribute);
}

std::string api::get_string_column_attribute(cpp_odbc::level2::statement_handle const & handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier) const
{
    return do_get_string_column_attribute(handle, column_id, field_identifier);
}

short int api::number_of_result_columns(statement_handle const & handle) const
{
    return do_number_of_result_columns(handle);
}

short int api::number_of_parameters(statement_handle const & handle) const
{
    return do_number_of_parameters(handle);
}

void api::prepare_statement(statement_handle const & handle, std::string const & sql) const
{
    do_prepare_statement(handle, sql);
}

void api::prepare_statement(statement_handle const & handle, std::u16string const & sql) const
{
    do_prepare_statement(handle, sql);
}

void api::set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, intptr_t value) const
{
    do_set_statement_attribute(handle, attribute, value);
}

void api::set_statement_attribute(statement_handle const & handle, SQLINTEGER attribute, SQLULEN * pointer) const
{
    do_set_statement_attribute(handle, attribute, pointer);
}

SQLLEN api::row_count(statement_handle const & handle) const
{
    return do_row_count(handle);
}

column_description api::describe_column(statement_handle const & handle, SQLUSMALLINT column_id) const
{
    return do_describe_column(handle, column_id);
}

column_description api::describe_column_wide(statement_handle const & handle, SQLUSMALLINT column_id) const
{
    return do_describe_column_wide(handle, column_id);
}

column_description api::describe_parameter(statement_handle const & handle, SQLUSMALLINT parameter_id) const
{
    return do_describe_parameter(handle, parameter_id);
}

bool api::more_results(statement_handle const & handle) const
{
    return do_more_results(handle);
}

bool api::supports_function(connection_handle const & handle, SQLUSMALLINT function_id) const
{
    return do_supports_function(handle, function_id);
}


} }
