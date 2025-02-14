#include "cpp_odbc/level1/api.h"

namespace cpp_odbc { namespace level1 {

api::api() = default;
api::~api() = default;

SQLRETURN api::allocate_handle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle_ptr) const
{
    return do_allocate_handle(handle_type, input_handle, output_handle_ptr);
}

SQLRETURN api::free_handle(SQLSMALLINT handle_type, SQLHANDLE handle) const
{
    return do_free_handle(handle_type, handle);
}

SQLRETURN api::get_diagnostic_record(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record_id, SQLCHAR * status_code_ptr, SQLINTEGER * native_error_ptr, SQLCHAR * message_text, SQLSMALLINT buffer_length, SQLSMALLINT * text_length_ptr) const
{
    return do_get_diagnostic_record(handle_type, handle, record_id, status_code_ptr, native_error_ptr, message_text, buffer_length, text_length_ptr);
}

SQLRETURN api::set_environment_attribute(SQLHENV environment_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    return do_set_environment_attribute(environment_handle, attribute, value_ptr, string_length);
}

SQLRETURN api::set_connection_attribute(SQLHDBC connection_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    return do_set_connection_attribute(connection_handle, attribute, value_ptr, string_length);
}

SQLRETURN api::establish_connection(SQLHDBC connection_handle, SQLHWND window_handle, SQLCHAR * input_connection_string, SQLSMALLINT input_connection_string_length, SQLCHAR * out_connection_string, SQLSMALLINT output_connection_string_buffer_length, SQLSMALLINT * output_connection_string_length, SQLUSMALLINT driver_completion) const
{
    return do_establish_connection(connection_handle, window_handle, input_connection_string, input_connection_string_length, out_connection_string, output_connection_string_buffer_length, output_connection_string_length, driver_completion);
}

SQLRETURN api::disconnect(SQLHDBC connection_handle) const
{
    return do_disconnect(connection_handle);
}

SQLRETURN api::end_transaction(SQLSMALLINT handle_type, SQLHANDLE connection_or_environment_handle, SQLSMALLINT completion_type) const
{
    return do_end_transaction(handle_type, connection_or_environment_handle, completion_type);
}

SQLRETURN api::get_connection_info(SQLHDBC connection_handle, SQLUSMALLINT info_type, SQLPOINTER info_value_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr) const
{
    return do_get_connection_info(connection_handle, info_type, info_value_ptr, buffer_length, string_length_ptr);
}

SQLRETURN api::bind_column(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLSMALLINT target_type, SQLPOINTER target_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const
{
    return do_bind_column(statement_handle, column_id, target_type, target_value_ptr, buffer_length, length_indicator_buffer);
}

SQLRETURN api::bind_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_id, SQLSMALLINT input_output_type, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLULEN column_size, SQLSMALLINT decimal_digits, SQLPOINTER parameter_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const
{
    return do_bind_parameter(statement_handle, parameter_id, input_output_type, value_type, parameter_type, column_size, decimal_digits, parameter_value_ptr, buffer_length, length_indicator_buffer);
}

SQLRETURN api::column_attribute(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier, SQLPOINTER character_attribute_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr, SQLLEN * numeric_attribute_ptr) const
{
    return do_column_attribute(statement_handle, column_id, field_identifier, character_attribute_ptr, buffer_length, string_length_ptr, numeric_attribute_ptr);
}

SQLRETURN api::execute_prepared_statement(SQLHSTMT statement_handle) const
{
    return do_execute_prepared_statement(statement_handle);
}

SQLRETURN api::execute_statement(SQLHSTMT statement_handle, SQLCHAR * sql_text, SQLINTEGER sql_length) const
{
    return do_execute_statement(statement_handle, sql_text, sql_length);
}

SQLRETURN api::fetch_scroll(SQLHSTMT statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const
{
    return do_fetch_scroll(statement_handle, fetch_orientation, fetch_offset);
}

SQLRETURN api::free_statement(SQLHSTMT statement_handle, SQLUSMALLINT option) const
{
    return do_free_statement(statement_handle, option);
}

SQLRETURN api::get_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER buffer_length, SQLINTEGER * string_length_ptr) const
{
    return do_get_statement_attribute(statement_handle, attribute, value_ptr, buffer_length, string_length_ptr);
}

SQLRETURN api::number_of_result_columns(SQLHSTMT statement_handle, SQLSMALLINT * destination) const
{
    return do_number_of_result_columns(statement_handle, destination);
}

SQLRETURN api::number_of_parameters(SQLHSTMT statement_handle, SQLSMALLINT * destination) const
{
    return do_number_of_parameters(statement_handle, destination);
}

SQLRETURN api::prepare_statement(SQLHSTMT statement_handle, SQLCHAR * statement_text, SQLINTEGER text_length) const
{
    return do_prepare_statement(statement_handle, statement_text, text_length);
}

SQLRETURN api::prepare_statement(SQLHSTMT statement_handle, SQLWCHAR * statement_text, SQLINTEGER text_length) const
{
    return do_prepare_statement(statement_handle, statement_text, text_length);
}

SQLRETURN api::set_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    return do_set_statement_attribute(statement_handle, attribute, value_ptr, string_length);
}

SQLRETURN api::row_count(SQLHSTMT statement_handle, SQLLEN * count) const
{
    return do_row_count(statement_handle, count);
}

SQLRETURN api::describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    return do_describe_column(statement_handle, column_number, column_name, buffer_length, name_length, data_type, column_size, decimal_digits, nullable);
}

SQLRETURN api::describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLWCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    return do_describe_column(statement_handle, column_number, column_name, buffer_length, name_length, data_type, column_size, decimal_digits, nullable);
}

SQLRETURN api::describe_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_number, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    return do_describe_parameter(statement_handle, parameter_number, data_type, column_size, decimal_digits, nullable);
}

SQLRETURN api::more_results(SQLHSTMT statement_handle) const
{
    return do_more_results(statement_handle);
}

SQLRETURN api::get_functions(SQLHDBC connection_handle, SQLUSMALLINT function_id, SQLUSMALLINT * is_supported) const
{
    return do_get_functions(connection_handle, function_id, is_supported);
}



} }
