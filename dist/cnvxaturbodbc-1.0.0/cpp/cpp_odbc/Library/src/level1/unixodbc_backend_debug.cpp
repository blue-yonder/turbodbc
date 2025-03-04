#include "cpp_odbc/level1/unixodbc_backend_debug.h"
#include "sql.h"
#include "sqlext.h"
#include <iostream>

namespace cpp_odbc { namespace level1 {

SQLRETURN unixodbc_backend_debug::do_allocate_handle(SQLSMALLINT handle_type, SQLHANDLE input_handle, SQLHANDLE * output_handle_ptr) const
{
    std::cout << " *DEBUG* allocate_handle";
    auto const return_code = SQLAllocHandle(handle_type, input_handle, output_handle_ptr);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_free_handle(SQLSMALLINT handle_type, SQLHANDLE handle) const
{
    std::cout << " *DEBUG* free_handle (handle_type = " << handle_type << ")";
    auto const return_code = SQLFreeHandle(handle_type, handle);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_get_diagnostic_record(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record_id, SQLCHAR * status_code_ptr, SQLINTEGER * native_error_ptr, SQLCHAR * message_text, SQLSMALLINT buffer_length, SQLSMALLINT * text_length_ptr) const
{
    std::cout << " *DEBUG* get_diagnostic_record (handle_type = " << handle_type << ")";
    auto const return_code = SQLGetDiagRec(handle_type, handle, record_id, status_code_ptr, native_error_ptr, message_text, buffer_length, text_length_ptr);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_set_environment_attribute(SQLHENV environment_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    std::cout << " *DEBUG* set_environment_attribute";
    auto const return_code = SQLSetEnvAttr(environment_handle, attribute, value_ptr, string_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_set_connection_attribute(SQLHDBC connection_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    std::cout << " *DEBUG* set_connection_attribute";
    auto const return_code = SQLSetConnectAttr(connection_handle, attribute, value_ptr, string_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_establish_connection(SQLHDBC connection_handle, SQLHWND window_handle, SQLCHAR * input_connection_string, SQLSMALLINT input_connection_string_length, SQLCHAR * out_connection_string, SQLSMALLINT output_connection_string_buffer_length, SQLSMALLINT * output_connection_string_length, SQLUSMALLINT driver_completion) const
{
    std::cout << " *DEBUG* establish_connection";
    auto const return_code = SQLDriverConnect(connection_handle, window_handle, input_connection_string, input_connection_string_length, out_connection_string, output_connection_string_buffer_length, output_connection_string_length, driver_completion);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_disconnect(SQLHDBC connection_handle) const
{
    std::cout << " *DEBUG* disconnect";
    auto const return_code = SQLDisconnect(connection_handle);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_end_transaction(SQLSMALLINT handle_type, SQLHANDLE connection_or_environment_handle, SQLSMALLINT completion_type) const
{
    std::cout << " *DEBUG* end_transaction";
    auto const return_code = SQLEndTran(handle_type, connection_or_environment_handle, completion_type);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_get_connection_info(SQLHDBC connection_handle, SQLUSMALLINT info_type, SQLPOINTER info_value_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr) const
{
    std::cout << " *DEBUG* get_connection_info";
    auto const return_code = SQLGetInfo(connection_handle, info_type, info_value_ptr, buffer_length, string_length_ptr);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_bind_column(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLSMALLINT target_type, SQLPOINTER target_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const
{
    std::cout << " *DEBUG* bind_column";
    auto const return_code = SQLBindCol(statement_handle, column_id, target_type, target_value_ptr, buffer_length, length_indicator_buffer);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_bind_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_id, SQLSMALLINT input_output_type, SQLSMALLINT value_type, SQLSMALLINT parameter_type, SQLULEN column_size, SQLSMALLINT decimal_digits, SQLPOINTER parameter_value_ptr, SQLLEN buffer_length, SQLLEN * length_indicator_buffer) const
{
    std::cout << " *DEBUG* bind_parameter";
    auto const return_code = SQLBindParameter(statement_handle, parameter_id, input_output_type, value_type, parameter_type, column_size, decimal_digits, parameter_value_ptr, buffer_length, length_indicator_buffer);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_column_attribute(SQLHSTMT statement_handle, SQLUSMALLINT column_id, SQLUSMALLINT field_identifier, SQLPOINTER character_attribute_ptr, SQLSMALLINT buffer_length, SQLSMALLINT * string_length_ptr, SQLLEN * numeric_attribute_ptr) const
{
    std::cout << " *DEBUG* column_attribute";
    auto const return_code = SQLColAttribute(statement_handle, column_id, field_identifier, character_attribute_ptr, buffer_length, string_length_ptr, numeric_attribute_ptr);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_execute_prepared_statement(SQLHSTMT statement_handle) const
{
    std::cout << " *DEBUG* execute_prepared_statement";
    auto const return_code = SQLExecute(statement_handle);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_execute_statement(SQLHSTMT statement_handle, SQLCHAR * sql_text, SQLINTEGER sql_length) const
{
    std::cout << " *DEBUG* execute_statement";
    auto const return_code = SQLExecDirect(statement_handle, sql_text, sql_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_fetch_scroll(SQLHSTMT statement_handle, SQLSMALLINT fetch_orientation, SQLLEN fetch_offset) const
{
    std::cout << " *DEBUG* fetch_scroll";
    auto const return_code = SQLFetchScroll(statement_handle, fetch_orientation, fetch_offset);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_free_statement(SQLHSTMT statement_handle, SQLUSMALLINT option) const
{
    std::cout << " *DEBUG* free_statement";
    auto const return_code = SQLFreeStmt(statement_handle, option);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_get_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER buffer_length, SQLINTEGER * string_length_ptr) const
{
    std::cout << " *DEBUG* get_statement_attribute";
    auto const return_code = SQLGetStmtAttr(statement_handle, attribute, value_ptr, buffer_length, string_length_ptr);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_number_of_result_columns(SQLHSTMT statement_handle, SQLSMALLINT * destination) const
{
    std::cout << " *DEBUG* number_of_result_columns";
    auto const return_code = SQLNumResultCols(statement_handle, destination);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_number_of_parameters(SQLHSTMT statement_handle, SQLSMALLINT * destination) const
{
    std::cout << " *DEBUG* number_of_parameters";
    auto const return_code = SQLNumParams(statement_handle, destination);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_prepare_statement(SQLHSTMT statement_handle, SQLCHAR * statement_text, SQLINTEGER text_length) const
{
    std::cout << " *DEBUG* prepare_statement";
    auto const return_code = SQLPrepare(statement_handle, statement_text, text_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_prepare_statement(SQLHSTMT statement_handle, SQLWCHAR * statement_text, SQLINTEGER text_length) const
{
    std::cout << " *DEBUG* prepare_statement (wide)";
    auto const return_code = SQLPrepareW(statement_handle, statement_text, text_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_set_statement_attribute(SQLHSTMT statement_handle, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length) const
{
    std::cout << " *DEBUG* set_statement_attribute";
    auto const return_code = SQLSetStmtAttr(statement_handle, attribute, value_ptr, string_length);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_row_count(SQLHSTMT statement_handle, SQLLEN * count) const
{
    std::cout << " *DEBUG* row_count";
    auto const return_code = SQLRowCount(statement_handle, count);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    std::cout << " *DEBUG* describe_column";
    auto const return_code = SQLDescribeCol(statement_handle, column_number, column_name, buffer_length, name_length, data_type, column_size, decimal_digits, nullable);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_describe_column(SQLHSTMT statement_handle, SQLUSMALLINT column_number, SQLWCHAR * column_name, SQLSMALLINT buffer_length, SQLSMALLINT * name_length, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    std::cout << " *DEBUG* describe_column (wide)";
    auto const return_code = SQLDescribeColW(statement_handle, column_number, column_name, buffer_length, name_length, data_type, column_size, decimal_digits, nullable);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_describe_parameter(SQLHSTMT statement_handle, SQLUSMALLINT parameter_number, SQLSMALLINT * data_type, SQLULEN * column_size, SQLSMALLINT * decimal_digits, SQLSMALLINT * nullable) const
{
    std::cout << " *DEBUG* describe_parameter";
    auto const return_code = SQLDescribeParam(statement_handle, parameter_number, data_type, column_size, decimal_digits, nullable);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_more_results(SQLHSTMT statement_handle) const
{
    std::cout << " *DEBUG* more_results";
    auto const return_code = SQLMoreResults(statement_handle);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}

SQLRETURN unixodbc_backend_debug::do_get_functions(SQLHDBC connection_handle, SQLUSMALLINT function_id, SQLUSMALLINT * is_supported) const
{
    std::cout << " *DEBUG* get_functions";
    auto const return_code = SQLGetFunctions(connection_handle, function_id, is_supported);
    std::cout << " (return code " << return_code << ")" << std::endl;
    return return_code;
}




} }
