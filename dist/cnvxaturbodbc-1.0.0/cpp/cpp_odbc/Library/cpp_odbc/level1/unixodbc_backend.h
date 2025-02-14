#pragma once

#include "cpp_odbc/level1/api.h"

namespace cpp_odbc { namespace level1 {

class unixodbc_backend : public api {
public:
    unixodbc_backend() = default;
    virtual ~unixodbc_backend() = default;
private:
    SQLRETURN do_allocate_handle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle_ptr) const final;
    SQLRETURN do_free_handle(SQLSMALLINT handle_type, SQLHANDLE handle) const final;
    SQLRETURN do_get_diagnostic_record(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record_id, SQLCHAR * status_code_ptr, SQLINTEGER * native_error_ptr, SQLCHAR * message_text, SQLSMALLINT buffer_length, SQLSMALLINT * text_length_ptr) const final;
    SQLRETURN do_set_environment_attribute(SQLHENV environment_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const final;
    SQLRETURN do_set_connection_attribute(SQLHDBC connection_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const final;
    SQLRETURN do_establish_connection(SQLHDBC connection_handle, SQLHWND window_handle, SQLCHAR * input_connection_string, SQLSMALLINT input_connection_string_length, SQLCHAR * out_connection_string, SQLSMALLINT output_connection_string_buffer_length, SQLSMALLINT * output_connection_string_length, SQLUSMALLINT driver_completion) const final;
    SQLRETURN do_disconnect(SQLHDBC connection_handle) const final;
    SQLRETURN do_end_transaction(SQLSMALLINT handle_type, SQLHANDLE connection_or_environment_handle, SQLSMALLINT completion_type) const final;
    SQLRETURN do_get_connection_info(SQLHDBC connection_handle, SQLUSMALLINT info_type, SQLPOINTER info_value_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr) const final;
    SQLRETURN do_bind_column(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLSMALLINT target_type, SQLPOINTER target_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const final;
    SQLRETURN do_bind_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_id, SQLSMALLINT input_output_type, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLULEN column_size, SQLSMALLINT decimal_digits, SQLPOINTER parameter_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const final;
    SQLRETURN do_column_attribute(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier, SQLPOINTER character_attribute_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr, SQLLEN * numeric_attribute_ptr) const final;
    SQLRETURN do_execute_prepared_statement(SQLHSTMT statement_handle) const final;
    SQLRETURN do_execute_statement(SQLHSTMT statement_handle, SQLCHAR * sql_text, SQLINTEGER sql_length) const final;
    SQLRETURN do_fetch_scroll(SQLHSTMT statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const final;
    SQLRETURN do_free_statement(SQLHSTMT statement_handle, SQLUSMALLINT option) const final;
    SQLRETURN do_get_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER buffer_length, SQLINTEGER * string_length_ptr) const final;
    SQLRETURN do_number_of_result_columns(SQLHSTMT statement_handle, SQLSMALLINT * destination) const final;
    SQLRETURN do_number_of_parameters(SQLHSTMT statement_handle, SQLSMALLINT * destination) const final;
    SQLRETURN do_prepare_statement(SQLHSTMT statement_handle, SQLCHAR * statement_text, SQLINTEGER text_length) const final;
    SQLRETURN do_prepare_statement(SQLHSTMT statement_handle, SQLWCHAR * statement_text, SQLINTEGER text_length) const final;
    SQLRETURN do_set_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const final;
    SQLRETURN do_row_count(SQLHSTMT statement_handle, SQLLEN * count) const final;
    SQLRETURN do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const final;
    SQLRETURN do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLWCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const final;
    SQLRETURN do_describe_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_number, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const final;
    SQLRETURN do_more_results(SQLHSTMT statement_handle) const final;
    SQLRETURN do_get_functions(SQLHDBC connection_handle, SQLUSMALLINT function_id, SQLUSMALLINT * is_supported) const final;
};

} }
